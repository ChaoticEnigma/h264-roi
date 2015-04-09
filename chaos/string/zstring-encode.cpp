/*******************************************************************************
**                                  LibChaos                                  **
**                             zstring-encode.cpp                             **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#include "zstring.h"

//#include <nowide/convert.hpp>
#include <stdexcept>

#ifdef __GNUC__
#   define NOWIDE_LIKELY(x)   __builtin_expect((x),1)
#   define NOWIDE_UNLIKELY(x) __builtin_expect((x),0)
#else
#   define NOWIDE_LIKELY(x)   (x)
#   define NOWIDE_UNLIKELY(x) (x)
#endif

namespace LibChaos {

namespace wideconvert {

class conversion_error : public std::runtime_error {
public:
    conversion_error() : std::runtime_error("Conversion failed") {}
};

typedef enum {
    skip            = 0,    ///< Skip illegal/unconvertable characters
    stop            = 1,    ///< Stop conversion and throw conversion_error
    default_method  = skip  ///< Default method - skip
} method_type;

namespace utf {
    typedef uint32_t code_point;

    static const code_point illegal = 0xFFFFFFFFu;
    static const code_point incomplete = 0xFFFFFFFEu;

    inline bool is_valid_codepoint(code_point v){
        if(v>0x10FFFF)
            return false;
        if(0xD800 <=v && v<= 0xDFFF) // surragates
            return false;
        return true;
    }

    template<typename CharType, int size = sizeof(CharType)> struct utf_traits;

    // UTF-8
    template<typename CharType> struct utf_traits<CharType, 1> {
        typedef CharType char_type;

        static int trail_length(char_type ci){
            unsigned char c = (unsigned char)ci;
            if(c < 128)
                return 0;
            if(NOWIDE_UNLIKELY(c < 194))
                return -1;
            if(c < 224)
                return 1;
            if(c < 240)
                return 2;
            if(NOWIDE_LIKELY(c <=244))
                return 3;
            return -1;
        }

        static const int max_width = 4;

        static int width(code_point value){
            if(value <=0x7F){
                return 1;
            } else if(value <=0x7FF){
                return 2;
            } else if(NOWIDE_LIKELY(value <=0xFFFF)) {
                return 3;
            } else {
                return 4;
            }
        }

        static bool is_trail(char_type ci){
            unsigned char c=ci;
            return (c & 0xC0)==0x80;
        }

        static bool is_lead(char_type ci){
            return !is_trail(ci);
        }

        template<typename Iterator> static code_point decode(Iterator &p, Iterator e){
            if(NOWIDE_UNLIKELY(p == e))
                return incomplete;

            unsigned char lead = (unsigned char)*p++;

            // First byte is fully validated here
            int trail_size = trail_length(lead);

            if(NOWIDE_UNLIKELY(trail_size < 0))
                return illegal;

            //
            // Ok as only ASCII may be of size = 0
            // also optimize for ASCII text
            //
            if(trail_size == 0)
                return lead;

            code_point c = lead & ((1 << (6 - trail_size)) - 1);

            // Read the rest
            unsigned char tmp;
            switch(trail_size){
                case 3:
                    if(NOWIDE_UNLIKELY(p==e))
                        return incomplete;
                    tmp = (unsigned char)*p++;
                    c = (c << 6) | (tmp & 0x3F);
                case 2:
                    if(NOWIDE_UNLIKELY(p==e))
                        return incomplete;
                    tmp = (unsigned char)*p++;
                    c = (c << 6) | (tmp & 0x3F);
                case 1:
                    if(NOWIDE_UNLIKELY(p==e))
                        return incomplete;
                    tmp = (unsigned char)*p++;
                    c = (c << 6) | (tmp & 0x3F);
                default:
                    break;
            }

            // Check code point validity: no surrogates and
            // valid range
            if(NOWIDE_UNLIKELY(!is_valid_codepoint(c)))
                return illegal;

            // make sure it is the most compact representation
            if(NOWIDE_UNLIKELY(width(c)!=trail_size + 1))
                return illegal;

            return c;
        }

        template<typename Iterator> static code_point decode_valid(Iterator &p){
            unsigned char lead = *p++;
            if(lead < 192)
                return lead;

            int trail_size;

            if(lead < 224)
                trail_size = 1;
            else if(NOWIDE_LIKELY(lead < 240)) // non-BMP rare
                trail_size = 2;
            else
                trail_size = 3;

            code_point c = lead & ((1<<(6-trail_size))-1);

            switch(trail_size) {
            case 3:
                c = (c << 6) | ( static_cast<unsigned char>(*p++) & 0x3F);
            case 2:
                c = (c << 6) | ( static_cast<unsigned char>(*p++) & 0x3F);
            case 1:
                c = (c << 6) | ( static_cast<unsigned char>(*p++) & 0x3F);
            }

            return c;
        }

        template<typename Iterator> static Iterator encode(code_point value, Iterator out){
            if(value <=0x7F) {
                *out++ = value;
            }
            else if(value <=0x7FF) {
                *out++=(value >> 6) | 0xC0;
                *out++=(value & 0x3F) | 0x80;
            }
            else if(NOWIDE_LIKELY(value <=0xFFFF)) {
                *out++=(value >> 12) | 0xE0;
                *out++=((value >> 6) & 0x3F) | 0x80;
                *out++=(value & 0x3F) | 0x80;
            }
            else {
                *out++=(value >> 18) | 0xF0;
                *out++=((value >> 12) & 0x3F) | 0x80;
                *out++=((value >> 6) & 0x3F) | 0x80;
                *out++=(value & 0x3F) | 0x80;
            }
            return out;
        }
    }; // utf8

    // UTF-16
    template<typename CharType> struct utf_traits<CharType, 2> {
        typedef CharType char_type;

        // See RFC 2781
        static bool is_first_surrogate(uint16_t x){
            return 0xD800 <=x && x<= 0xDBFF;
        }
        static bool is_second_surrogate(uint16_t x){
            return 0xDC00 <=x && x<= 0xDFFF;
        }
        static code_point combine_surrogate(uint16_t w1,uint16_t w2){
            return ((code_point(w1 & 0x3FF) << 10) | (w2 & 0x3FF)) + 0x10000;
        }
        static int trail_length(char_type c){
            if(is_first_surrogate(c))
                return 1;
            if(is_second_surrogate(c))
                return -1;
            return 0;
        }

        // Returns true if c is trail code unit, always false for UTF-32
        static bool is_trail(char_type c){
            return is_second_surrogate(c);
        }

        // Returns true if c is lead code unit, always true of UTF-32
        static bool is_lead(char_type c){
            return !is_second_surrogate(c);
        }

        template<typename It> static code_point decode(It &current,It last){
            if(NOWIDE_UNLIKELY(current == last))
                return incomplete;
            uint16_t w1=*current++;
            if(NOWIDE_LIKELY(w1 < 0xD800 || 0xDFFF < w1)) {
                return w1;
            }
            if(w1 > 0xDBFF)
                return illegal;
            if(current==last)
                return incomplete;
            uint16_t w2=*current++;
            if(w2 < 0xDC00 || 0xDFFF < w2)
                return illegal;
            return combine_surrogate(w1,w2);
        }

        template<typename It> static code_point decode_valid(It &current){
            uint16_t w1=*current++;
            if(NOWIDE_LIKELY(w1 < 0xD800 || 0xDFFF < w1)) {
                return w1;
            }
            uint16_t w2=*current++;
            return combine_surrogate(w1,w2);
        }

        static const int max_width = 2;
        static int width(code_point u){
            return u>=0x10000 ? 2 : 1;
        }
        template<typename It> static It encode(code_point u,It out){
            if(NOWIDE_LIKELY(u<=0xFFFF)) {
                *out++ = u;
            } else {
                u-=0x10000;
                *out++=0xD800 | (u>>10);
                *out++=0xDC00 | (u & 0x3FF);
            }
            return out;
        }
    }; // utf16;

    // UTF-32
    template<typename CharType> struct utf_traits<CharType, 4> {
        typedef CharType char_type;

        static int trail_length(char_type c){
            if(is_valid_codepoint(c))
                return 0;
            return -1;
        }
        static bool is_trail(char_type /*c*/){
            return false;
        }
        static bool is_lead(char_type /*c*/){
            return true;
        }

        template<typename It> static code_point decode_valid(It &current){
            return *current++;
        }

        template<typename It> static code_point decode(It &current,It last){
            if(NOWIDE_UNLIKELY(current == last))
                return incomplete;
            code_point c = *current++;
            if(NOWIDE_UNLIKELY(!is_valid_codepoint(c)))
                return illegal;
            return c;
        }

        static const int max_width = 1;
        static int width(code_point /*u*/){
            return 1;
        }
        template<typename It>
        static It encode(code_point u,It out){
            *out++ = u;
            return out;
        }

    }; // utf32;
}

template<typename CharOut, typename CharIn> std::basic_string<CharOut> utf_to_utf(CharIn const *begin, CharIn const *end, method_type how = default_method){
    typedef std::back_insert_iterator< std::basic_string<CharOut> > inserter_type;

    std::basic_string<CharOut> result;
    result.reserve(end - begin);
    inserter_type inserter(result);
    utf::code_point codepoint;
    while(begin != end){
        codepoint = utf::utf_traits<CharIn>::template decode<CharIn const *>(begin, end);
        if(codepoint == utf::illegal || codepoint == utf::incomplete){
            if(how == stop)
                throw conversion_error();
        } else{
            utf::utf_traits<CharOut>::template encode<inserter_type>(codepoint, inserter);
        }
    }
    return result;
}

template<typename CharOut, typename CharIn> std::basic_string<CharOut> utf_to_utf(std::basic_string<CharIn> const &str, method_type how = default_method){
    return utf_to_utf<CharOut, CharIn>(str.c_str(), str.c_str() + str.size(), how);
}

inline std::string narrow(std::wstring const &s){
    return utf_to_utf<char>(s);
}

inline std::wstring widen(std::string const &s){
    return utf_to_utf<wchar_t>(s);
}

}

void ZString::fromUtf16(std::wstring wstr){
//    assign(nowide::narrow(wstr));
    assign(wideconvert::narrow(wstr));
}

std::wstring ZString::toUtf16() const{
//    return nowide::widen(_data);
    return wideconvert::widen(_data);
}

bool ZString::isUtf8(ZString str){
    const char *string = str.cc();
    const unsigned char *bytes = (const unsigned char *)string;
    while(*bytes){
        if( // ASCII: use bytes[0] <= 0x7F to allow ASCII control characters
            bytes[0] == 0x09 ||
            bytes[0] == 0x0A ||
            bytes[0] == 0x0D ||
            (0x20 <= bytes[0] && bytes[0] <= 0x7E)
        ){
            bytes += 1;
            continue;
        }

        if( // non-overlong 2-byte
            (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
            (0x80 <= bytes[1] && bytes[1] <= 0xBF)
        ){
            bytes += 2;
            continue;
        }

        if(( // excluding overlongs
            bytes[0] == 0xE0 &&
            (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) || ( // straight 3-byte
                ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
            bytes[0] == 0xEE ||
            bytes[0] == 0xEF) &&
            (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) || ( // excluding surrogates
            bytes[0] == 0xED &&
            (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF)
        )){
            bytes += 3;
            continue;
        }

        if(( // planes 1-3
            bytes[0] == 0xF0 &&
            (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
            (0x80 <= bytes[3] && bytes[3] <= 0xBF)
        ) || ( // planes 4-15
            (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
            (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
            (0x80 <= bytes[3] && bytes[3] <= 0xBF)
        ) || ( // plane 16
                bytes[0] == 0xF4 &&
            (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
            (0x80 <= bytes[3] && bytes[3] <= 0xBF)
        )){
            bytes += 4;
            continue;
        }
        return false;
    }
    return true;
}

}
