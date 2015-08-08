/*******************************************************************************
**                                  LibChaos                                  **
**                                  zstring.h                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZSTRING_H
#define ZSTRING_H

#include "ztypes.h"
#include "zallocator.h"
#include "zassoc.h"
#include "zhash.h"
#include "ziterable.h"

// Needed for std::ostream overload
#include <iosfwd>

#if PLATFORM == WINDOWS
    #define UNICODE
    #define _UNICODE
#endif

namespace LibChaos {

class ZString;
typedef ZIterator<ZString> ZStringIterator;

typedef ZArray<ZString> ArZ;
typedef ZAssoc<ZString, ZString> AsArZ;

typedef char zstringchartype;

/*! UTF-8 contiguous string container.
 *  Wide characters are narrowed and encoded in UTF-8.
 *  Internal array is always null terminated.
 */
class ZString : public ZIterable<zstringchartype> {
public:
    typedef zstringchartype chartype;
    enum {
        none = ZU64_MAX
    };

public:
    ZString();
    ~ZString();

//    ZString(ZStorage *stor) : ZString(){
//        if(stor != nullptr){
//            resize(stor->size());
//            stor->copyBlockTo(0, stor->size(), (zbyte *)_data);
//        }
//    }

    ZString(const ZString &other);

    // Assumed UTF-8
    ZString(const chartype *str); // Null-terminated C-string
    ZString(const chartype *ptr, zu64 length);
    ZString(const ZArray<chartype> &array); //  ZArray<char>

    // std strings
    // Assumed UTF-8
    ZString(std::string str);
    std::string str() const;

    // Null-terminated wide string
    // Assumed UTF-16
    ZString(const wchar_t *wstr);
    ZString(const wchar_t *wstr, zu64 length);
    ZString(const ZArray<wchar_t> &array); //  ZArray<wchar_t>

    // std wide strings
    // Assumed UTF-16 and converted to UTF-8
    ZString(std::wstring wstr);
    std::wstring wstr() const;

    // Pointer to data
    inline const chartype *cc() const { return _data; }
    inline chartype *c() const { return _data; }

    // Fill constructor
    ZString(chartype ch, zu64 len = 1);

    // Interger to string
    static ZString ItoS(zu64 num, zu8 base = 10, zu64 pad = 0);
    static ZString ItoS(zs64 num, zu8 base = 10);

    ZString(zuc num) : ZString((zull)num){}
    ZString(zsc num) : ZString((zsll)num){}
    ZString(zus num) : ZString((zull)num){}
    ZString(zss num) : ZString((zsll)num){}
    ZString(zuint num) : ZString((zull)num){}
    ZString(zint num) : ZString((zsll)num){}
    ZString(zul num) : ZString((zull)num){}
    ZString(zsl num) : ZString((zsll)num){}

    ZString(zull num) : ZString(ItoS((zu64)num, 10)){}
    ZString(zsll num) : ZString(ItoS((zs64)num, 10)){}

    // To integer
    bool isInteger(zu8 base = 10) const;
    int tint() const;
    zu64 tozu64(zu8 fromBase = 10) const;

    // To floating point
    bool isFloat() const;
    float toFloat() const;

    // Construct from double with <places> decimal points, 0 means all
    ZString(double flt, unsigned places = 0);

    // Assignment
    ZString &assign(const ZString &other);
    inline ZString &operator=(const ZString &rhs){ return assign(rhs); }

    // Comparison
    friend bool operator==(const ZString &lhs, const ZString &rhs);
    friend bool operator!=(const ZString &lhs, const ZString &rhs);

    // Clear
    inline void clear(){ resize(0); }

    // Resize (IMPORTANT: memory is only allocated and initialized here)
    void reserve(zu64 size);

    //! Append \a str to string.
    ZString &append(const ZString &str);
    //! Operator overload for append().
    inline ZString &operator+=(const ZString &str){ return append(str); }
    //! Operator overload for append().
    inline ZString &operator<<(const ZString &str){ return append(str); }

    //! Concatenate string and \a str.
    ZString concat(const ZString &str) const;
    //! Concatenate \a lhs and \a rhs.
    friend ZString operator+(const ZString &lhs, const ZString &rhs);

    //! Prepend \a str to string.
    ZString &prepend(const ZString &str);

    //! Get reference to character at \a i.
    inline char &operator[](zu64 i){ return _data[i]; }
    //! Get constant reference to character at \a i.
    inline const char &operator[](zu64 i) const { return _data[i]; }

    //! Count occurrences of \a test.
    zu64 count(ZString test) const;

    //! Tests if \a str begins with \a test. Ignores whitespace at beginning of string if \a ignore_whitespace.
    bool startsWith(const ZString &test, bool ignore_whitespace = true) const;
    //! Tests if string begins with \a test. Ignores whitespace at beginning of string if \a ignore_whitespace.
    static bool startsWith(const ZString &str, const ZString &test, bool ignore_whitespace = true);

    //! Alias for startsWith, always ignores whitespace
    inline bool beginsWith(ZString test) const { return startsWith(test, false); }

    //! Tests if string ends with \a test.
    bool endsWith(ZString test) const;

    //! Insert \a txt at \a pos in string.
    ZString &insert(zu64 pos, const ZString &txt);
    //! Insert \a txt at \a pos in \a str.
    static ZString insert(ZString str, zu64 pos, const ZString &txt);

    //! Get portion of string from \a pos to end.
    ZString &substr(zu64 pos);
    //! Get portion of \a str from \a pos to end.
    static ZString substr(ZString str, zu64 pos);

    //! Get \a len characters after \a pos of string.
    ZString &substr(zu64 pos, zu64 len);
    //! Get \a len characters after \a pos of \a str.
    static ZString substr(ZString str, zu64 pos, zu64 len);

    //! Get location of of first occurrence of \a find in string after \a start.
    zu64 findFirst(const ZString &find, zu64 start = 0) const;
    //! Get location of of first occurrence of \a find in \a str after \a start.
    static zu64 findFirst(const ZString &str, const ZString &find, zu64 start = 0);

    //! Get locations of of all non-overlapping occurrences of \a find in string.
    ZArray<zu64> findAll(const ZString &find) const;
    //! Get locations of of all non-overlapping occurrences of \a find in \a str.
    static ZArray<zu64> findAll(const ZString &str, const ZString &find);

    //! Replace section \a len characters long at \a pos with \a after in string.
    ZString &replacePos(zu64 pos, zu64 len, const ZString &after);
    //! Replace section \a len characters long at \a pos with \a after in \a str.
    static ZString replacePos(ZString str, zu64 pos, zu64 len, const ZString &after);

    //! Replace first occurence of \a before in string with \a after after \a start.
    ZString &replaceFirst(const ZString &before, const ZString &after, zu64 start = 0);
    //! Replace first occurence of \a before in \a str with \a after after \a start.
    static ZString replaceFirst(ZString str, const ZString &before, const ZString &after, zu64 start = 0);

    /*! Replace up to \a max occurrences of \a before in string with \a after.
     *  \param max Number of occurrences to replace (0 for unlimited)
     */
    ZString &replace(const ZString &before, const ZString &after, zu64 max = 0);
    /*! Replace up to \a max occurrences of \a before in \a str with \a after.
     *  \param max Number of occurrences to replace (0 for unlimited)
     */
    static ZString replace(ZString str, const ZString &before, const ZString &after, zu64 max = 0);

    /*! Replace the first occurrence of \a before in string with \a after, up to \a max times.
     *  Replaces recursively, so will search entire string each replace.
     *  \param max Number of occurrences to replace (0 for unlimited)
     */
    ZString &replaceRecursive(const ZString &before, const ZString &after, zu64 max = 1000);
    /*! Replace the first occurrence of \a before in \a str with \a after, up to \a max times.
     *  Replaces recursively, so will search entire string each replace.
     *  \param max Number of occurrences to replace (0 for unlimited)
     */
    static ZString replaceRecursive(ZString str, const ZString &before, const ZString &after, zu64 max = 1000);

    //! Get sub-string of \a str before first occurence of \a find in \a str.
    static ZString getUntil(ZString str, const ZString &find);

    //! Get first string between \a start and \a end in string.
    ZString findFirstBetween(ZString start, ZString end);
    //! Replace first string between \a start and \a end in string with \a after.
    ZString replaceBetween(ZString start, ZString end, ZString after);

    ZString findFirstXmlTagCont(ZString tag);
    ZString replaceXmlTagCont(ZString tag, ZString after);

    ZString &label(const ZString &label, const ZString &value);
    ZString &label(const AsArZ &values);

    //! Strip occurences of \a target from beginning of string.
    ZString &stripFront(chartype target);
    //! Strip occurences of \a target from beginning of \a str.
    static ZString stripFront(ZString str, chartype target);

    //! Strip occurences of \a target from end of string.
    ZString &stripBack(chartype target);
    //! Strip occurences of \a target from end of \a str.
    static ZString stripBack(ZString str, chartype target);

    //! Strip occurences of \a target from beginning and end of string.
    ZString &strip(chartype target);
    //! Strip occurences of \a target from beginning and end of \a str.
    static ZString strip(ZString str, chartype target);

    ZString removeWhitespace();

    ZString &reverse();
    static ZString reverse(ZString str);

    // Convert UPPERCASE characters to lowercase equivalents in <str>
    ZString &toLower();
    static ZString toLower(ZString str);

    // What is this for...?
    ZString &duplicate(zu64 iterate);

    ArZ split(ZString delim) const;

    // Explode a string into any array of substrings
    // All explode functions will treat consecutive delimiters as one delimitier
    // Delimiters at the beginning or end of a string are discarded
    ArZ explode(chartype delim) const;
    ArZ strExplode(const ZString &delim) const;
    ArZ quotedExplode(chartype delim) const;
    ArZ escapedExplode(chartype delim) const;
    ArZ explodeList(unsigned nargs, ...) const;
    //ArZ explode();

    static ZString compound(ArZ parts, ZString delim);

    //ZString format(ZString fmt_str, ...);
    //ZString &format(...);

    static bool charIsAlphabetic(chartype ch);

    static bool alphaTest(ZString str1, ZString str2);

    // Allows ZString to be used with std streams
    friend std::ostream &operator<<(std::ostream &lhs, ZString rhs);

    void swap(ZString &other);

    inline bool isEmpty() const { return (size() == 0); }

    // Number of bytes (code units)
    inline zu64 size() const { return _size; }
    inline zu64 realSize() const { return _realsize; }

    inline const zbyte *bytes() const { return reinterpret_cast<zbyte*>(_data); }

    // Number of *characters* (code points)
    zu64 length() const;

    // ZString Iterator
    ZIterator<chartype> iterator(){
        return ZIterator<chartype>(new ZStringAccessor(this));
    }

    // On empty string, will return null terminator
    inline chartype &first(){ return _data[0]; }
    inline const chartype &first() const { return _data[0]; }
    inline chartype &last(){ return _data[size() - 1]; }
    inline const chartype &last() const { return _data[size() - 1]; }

private:
    void resize(zu64 len);
    static bool _charIsWhitespace(chartype ch);
    zu64 _strReplace(const ZString &before, const ZString &after, zu64 startpos);

    // Unicode Encoding
    void fromUtf16(std::wstring wstr);
    std::wstring toUtf16() const;

    static bool isUtf8(ZString str);

private:
    class ZStringAccessor : public ZAccessor<chartype> {
    public:
        ZStringAccessor(ZString *str, zu64 pos = 0) : _str(str), _pos(pos){}

        bool atEnd() const {
            return _pos >= _str->size();
        }

        bool atFront() const {
            return _pos <= 0;
        }

        void forward(){
            if(!atEnd())
                ++_pos;
        }

        void back(){
            if(!atFront())
                --_pos;
        }

        ZString::chartype &current(){
            return _str->operator[](_pos);
        }

    private:
        ZString *_str;
        zu64 _pos;
    };

private:
    ZAllocator<chartype> _alloc;
    zu64 _size;
    zu64 _realsize;
    chartype *_data;
};

inline ZString operator+(const ZString &lhs, const ZString &rhs){
    return lhs.concat(rhs);
}

inline bool operator==(const ZString &lhs, const ZString &rhs){
    if(lhs.size() != rhs.size())
        return false;
    return memcmp(lhs._data, rhs._data, lhs.size()) == 0;
}
inline bool operator!=(const ZString &lhs, const ZString &rhs){
    return !operator==(lhs, rhs);
}

// ZString specialization ZHash
ZHASH_USER_SPECIALIAZATION(ZString, (const ZString &str), (str.bytes(), str.size()), {})

} // namespace LibChaos

#endif // ZSTRING_H
