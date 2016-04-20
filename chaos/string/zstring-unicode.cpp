/*******************************************************************************
**                                  LibChaos                                  **
**                             zstring-unicode.cpp                            **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zstring.h"
#include "zlog.h"
#include <stdio.h>

namespace LibChaos {

void ZString::parseUTF8(const codeunit8 *units, zu64 max){
    clear();
    if(units && max){
        // Read and add code points
        while(*units && max){
            _appendCodePoint(_nextUTF8(&units, &max));
        }
    }
}

void ZString::parseUTF16(const codeunit16 *units, zu64 max){
    clear();
    if(units && max){
        // Read and add code points
        while(*units && max){
            _appendCodePoint(_nextUTF16(&units, &max));
        }
    }
}

void ZString::parseUTF32(const ZString::codeunit32 *units, zu64 max){
    clear();
    if(units && max){
        // Read and add code points
        while(*units && max){
            _appendCodePoint(_nextUTF32(&units, &max));
        }
    }
}

void ZString::unicode_debug(const codeunit *bytes){
    //printf("\"%s\"\n", (const char *)bytes);

    while(*bytes){
        zu32 cp = 0;

        if((bytes[0] & 0xF8) == 0xF0){
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            // 4 bytes

            if((bytes[1] & 0xC0) != 0x80)
                LOG("Invalid byte 2: 0x" << ZString::ItoS(bytes[1],16,2));
            if((bytes[2] & 0xC0) != 0x80)
                LOG("Invalid byte 3: 0x" << ZString::ItoS(bytes[2],16,2));
            if((bytes[3] & 0xC0) != 0x80)
                LOG("Invalid byte 4: 0x" << ZString::ItoS(bytes[3],16,2));

            cp = ((zu32)bytes[0] & 0x07) << 18 |
                      ((zu32)bytes[1] & 0x3F) << 12 |
                      ((zu32)bytes[2] & 0x3F) << 6 |
                      (bytes[3] & 0x3F);

            LOG("(" << codePointStr(cp) << ") [" << ZBinary(bytes, 4).strBytes() << "] '" << ZString(bytes, 4) << "'");
            bytes += 4;

        } else if((bytes[0] & 0xF0) == 0xE0){
            // 1110xxxx 10xxxxxx 10xxxxxx
            // 3 bytes

            if((bytes[1] & 0xC0) != 0x80)
                LOG("Invalid byte 2: 0x" << ZString::ItoS(bytes[1],16,2));
            if((bytes[2] & 0xC0) != 0x80)
                LOG("Invalid byte 3: 0x" << ZString::ItoS(bytes[2],16,2));

            cp = ((zu32)bytes[0] & 0x0F) << 12 |
                      ((zu32)bytes[1] & 0x3F) << 6 |
                      (bytes[2] & 0x3F);

            LOG("(" << codePointStr(cp) << ") [" << ZBinary(bytes, 3).strBytes() << "] '" << ZString(bytes, 3) << "'");
            bytes += 3;

        } else if((bytes[0] & 0xE0) == 0xC0){
            // 110xxxxx 10xxxxxx
            // 2 bytes

            if((bytes[1] & 0xC0) != 0x80)
                LOG("Invalid byte 2: 0x" << ZString::ItoS(bytes[1],16,2));

            cp = ((zu32)bytes[0] & 0x1F) << 6 |
                      (bytes[1] & 0x3F);

            LOG("(" << codePointStr(cp) << ") [" << ZBinary(bytes, 2).strBytes() << "] '" << ZString(bytes, 2) << "'");
            bytes += 2;

        } else if((bytes[0] & 0x80) == 0){
            // 0xxxxxxx
            // 1 byte

            cp = bytes[0];

            LOG("(" << codePointStr(cp) << ") [" << ZBinary(bytes, 1).strBytes() << "] '" << ZString(bytes, 1) << "'");
            bytes += 1;

        } else {
            LOG("Invalid byte: 0x" << ZString::ItoS(bytes[0],16,2));
            bytes += 1;
            continue;
        }

        if((0x15000 <= cp && cp <= 0x15FFF) ||
           (0x17000 <= cp && cp <= 0x1AFFF) ||
           (0x1C000 <= cp && cp <= 0x1CFFF)){
            LOG("Invalid code point in plane 1");
        } else if(0x2D000 <= cp && cp <= 0x2EFFF){
            LOG("Invalid code point in plane 2");
        } else if(0x30000 <= cp && cp <= 0xDFFFF){
            LOG("Invalid code point in plane 3-13");
        } else if(0xE1000 <= cp && cp <= 0xEFFFF){
            LOG("Invalid code point in plane 14");
        } else if(cp > 0x10FFFF){
            LOG("Code point out of range");
        }

    }
}

ZString ZString::codePointStr(zu64 cp){
    return ZString("U+") + ZString::ItoS(cp,16,5,true);
}

// ///////////////////////////////////////////////////////////////////////////////
// Private functions
// ///////////////////////////////////////////////////////////////////////////////

void ZString::_appendCodePoint(codepoint cp){
    if(cp == 0){
        // Skip invalid code points

    } else if(cp <= 0x7F){
        // 1 byte
       _resize(size()+1);
        _data[size()-1] = (zu8)(cp & 0x7F);

    } else if(cp <= 0x7FF){
        // 2 bytes
       _resize(size()+2);
        _data[size()-2] = (zu8)(cp >> 6 & 0x1F) | 0xC0;
        _data[size()-1] = (zu8)(cp      & 0x3F) | 0x80;

    } else if(cp <= 0xFFFF){
        // 3 bytes
       _resize(size()+3);
        _data[size()-3] = (zu8)(cp >> 12 & 0x0F) | 0xE0;
        _data[size()-2] = (zu8)(cp >> 6  & 0x3F) | 0x80;
        _data[size()-1] = (zu8)(cp       & 0x3F) | 0x80;

    } else if(cp <= 0x1FFFFF){
        // 4 bytes
       _resize(size()+4);
        _data[size()-4] = (zu8)(cp >> 18 & 0x07) | 0xF0;
        _data[size()-3] = (zu8)(cp >> 12 & 0x3F) | 0x80;
        _data[size()-2] = (zu8)(cp >> 6  & 0x3F) | 0x80;
        _data[size()-1] = (zu8)(cp       & 0x3F) | 0x80;

    } else if(cp <= 0x3FFFFFF){
        // 4 bytes
       _resize(size()+5);
        _data[size()-5] = (zu8)(cp >> 24 & 0x03) | 0xF8;
        _data[size()-4] = (zu8)(cp >> 18 & 0x3F) | 0x80;
        _data[size()-3] = (zu8)(cp >> 12 & 0x3F) | 0x80;
        _data[size()-2] = (zu8)(cp >> 6  & 0x3F) | 0x80;
        _data[size()-1] = (zu8)(cp       & 0x3F) | 0x80;

    } else if(cp <= 0x7FFFFFFF){
        // 4 bytes
       _resize(size()+6);
        _data[size()-6] = (zu8)(cp >> 30 & 0x01) | 0xFC;
        _data[size()-5] = (zu8)(cp >> 24 & 0x3F) | 0x80;
        _data[size()-4] = (zu8)(cp >> 18 & 0x3F) | 0x80;
        _data[size()-3] = (zu8)(cp >> 12 & 0x3F) | 0x80;
        _data[size()-2] = (zu8)(cp >> 6  & 0x3F) | 0x80;
        _data[size()-1] = (zu8)(cp       & 0x3F) | 0x80;

    } else {
        // Cannot encode code points larger than 31 bits with UTF-8
    }
}

ZString::codepoint ZString::_nextUTF8(const codeunit8 **punits, zu64 *maxunits){
    // UTF-8: RFC-3629
    // Non-standard 5-byte and 6-byte UTF-8 sequences are supported for compatibility, although they may never be used.
    // On invalid bytes, return 0.
    // On invalid continuation bytes, set *puints to the offending byte and return 0.

    const codeunit8 *units = *punits;
    codepoint cp = 0;

    // Check leading byte
    if(*maxunits >= 1 && (units[0] & 0x80) == 0){
        // 1 byte
        cp |= units[0];
        *punits += 1;
        *maxunits -= 1;

    } else if(*maxunits >= 2 && (units[0] & 0xE0) == 0xC0){
        // 2 bytes
        // Check continuation bytes
        if((units[1] & 0xC0) != 0x80){
            *punits += 1;
            *maxunits -= 1;
            return 0;
        }
        // Decode code point
        cp |= ((zu32)units[0] & 0x1F) << 6;
        cp |= ((zu32)units[1] & 0x3F);
        *punits += 2;
        *maxunits -= 2;

    } else if(*maxunits >= 3 && (units[0] & 0xF0) == 0xE0){
        // 3 bytes
        // Check continuation bytes
        for(zu8 i = 1; i < 3; ++i){
            if((units[i] & 0xC0) != 0x80){
                *punits += i;
                *maxunits -= i;
                return 0;
            }
        }
        // Decode code point
        cp |= ((zu32)units[0] & 0x0F) << 12;
        cp |= ((zu32)units[1] & 0x3F) << 6;
        cp |= ((zu32)units[2] & 0x3F);
        *punits += 3;
        *maxunits -= 3;

    } else if(*maxunits >= 4 && (units[0] & 0xF8) == 0xF0){
        // 4 bytes
        // Check continuation bytes
        for(zu8 i = 1; i < 4; ++i){
            if((units[i] & 0xC0) != 0x80){
                *punits += i;
                *maxunits -= i;
                return 0;
            }
        }
        // Decode code point
        cp |= ((zu32)units[0] & 0x07) << 18;
        cp |= ((zu32)units[1] & 0x3F) << 12;
        cp |= ((zu32)units[2] & 0x3F) << 6;
        cp |= ((zu32)units[3] & 0x3F);
        *punits += 4;
        *maxunits -= 4;

    } else if(*maxunits >= 5 && (units[0] & 0xFC) == 0xF8){
        // 5 bytes (non-standard)
        // Check continuation bytes
        for(zu8 i = 1; i < 5; ++i){
            if((units[i] & 0xC0) != 0x80){
                *punits += i;
                *maxunits -= i;
                return 0;
            }
        }
        // Decode code point
        cp |= ((zu32)units[0] & 0x03) << 24;
        cp |= ((zu32)units[1] & 0x3F) << 18;
        cp |= ((zu32)units[2] & 0x3F) << 12;
        cp |= ((zu32)units[3] & 0x3F) << 6;
        cp |= ((zu32)units[4] & 0x3F);
        *punits += 5;
        *maxunits -= 5;

    } else if(*maxunits >= 6 && (units[0] & 0xFE) == 0xFC){
        // 6 bytes (non-standard)
        // Check continuation bytes
        for(zu8 i = 1; i < 6; ++i){
            if((units[i] & 0xC0) != 0x80){
                *punits += i;
                *maxunits -= i;
                return 0;
            }
        }
        // Decode code point
        cp |= ((zu32)units[0] & 0x01) << 30;
        cp |= ((zu32)units[1] & 0x3F) << 24;
        cp |= ((zu32)units[2] & 0x3F) << 18;
        cp |= ((zu32)units[3] & 0x3F) << 12;
        cp |= ((zu32)units[4] & 0x3F) << 6;
        cp |= ((zu32)units[5] & 0x3F);
        *punits += 6;
        *maxunits -= 6;

    } else {
        // invalid byte
        *punits += 1;
        *maxunits -= 1;
        return 0;
    }

    return cp;
}

ZString::codepoint ZString::_nextUTF16(const codeunit16 **punits, zu64 *maxunits){
    // UTF-16: RFC-2781

    const codeunit16 *units = *punits;
    codepoint cp = 0;

    // Check for surrogate pairs
    if(*maxunits >= 1 && (units[0] & 0xFC00) != 0xD800){
        // Single
        cp |= units[0];
        *punits += 1;
        *maxunits -= 1;

    } else if(*maxunits >= 2 && (units[1] & 0xFC00) == 0xDC00){
        // Pair
        cp |= ((zu32)units[0] & 0x03FF) << 10;
        cp |= ((zu32)units[1] & 0x03FF);
        cp += 0x10000;
        *punits += 2;
        *maxunits -= 2;

    } else {
        // Invalid sequence
        *punits += 2;
        *maxunits -= 2;
        return 0;
    }

    return cp;
}

ZString::codepoint ZString::_nextUTF32(const codeunit32 **punits, zu64 *maxunits){
    // UTF-32

    const codeunit32 *units = *punits;
    codepoint cp = 0;

    // Check for bit 32
    if((units[0] & 0x80000000) == 0){
        // 31 or less bits
        cp |= units[0];
        *punits += 1;
        *maxunits -= 1;
    } else {
        // 32nd bit set
        *punits += 1;
        *maxunits -= 1;
        return 0;
    }

    return cp;
}

bool ZString::isUTF8(const char *str){
    const unsigned char *bytes = (const unsigned char *)str;
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

void ZString::unicode_normalize(){

}

}
