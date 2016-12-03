/*******************************************************************************
**                                  LibChaos                                  **
**                                  zstring.h                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZSTRING_H
#define ZSTRING_H

#include "ztypes.h"
#include "zaccessor.h"
#include "zallocator.h"
#include "zarray.h"
#include "zlist.h"
#include "zassoc.h"
#include "zhash.h"

// Needed for std::ostream overload
#include <iosfwd>

#if PLATFORM == WINDOWS || PLATFORM == CYGWIN
    #define UNICODE
    #define _UNICODE
    #include <string>
#endif

namespace LibChaos {

class ZString;
typedef ZArray<ZString> ArZ;
typedef ZAssoc<ZString, ZString> AsArZ;

/*! UTF-8 contiguous string container.
 *  Parses UTF-8, UTF-16, or UTF-32 strings and encodes parsed code points in normalized UTF-8.
 *  Since unicode normalization can only be done on real unicode code points, invalid code unit sequences
 *  and code points will be discarded. Non-unicode bytes in input will also be discarded.
 *
 *  Internal buffer will always be valid null-terminated UTF-8 representing a sequence of valid Unicode code points.
 */
class ZString : public ZAccessor<char> {
public:
    typedef zu32 codepoint;
    typedef zbyte codeunit8;
    typedef zu16 codeunit16;
    typedef zu32 codeunit32;
    typedef codeunit8 codeunit;

    enum { NONE = ZU64_MAX };

public:
    //! Default constructor with optional user allocator.
    ZString(ZAllocator<codeunit> *alloc = new ZAllocator<codeunit>);

    //! Copy constructor.
    ZString(const ZString &other);

    //! Destructor.
    ~ZString();

    //! Construct from UTF-8 null-terminated C-string.
    ZString(const char *str, zu64 max = ZU64_MAX);
    //! Construct from UTF-8 character array.
    ZString(const ZArray<char> &array);

    //! Get pointer to data as C-string.
    inline char *c() const { return reinterpret_cast<char*>(_data); }
    //! Get constant pointer to data as C-string.
    inline const char *cc() const { return reinterpret_cast<const char*>(_data); }

    //! Construct from UTF-8 zero-terminated bytes.
    ZString(const zbyte *str, zu64 max = ZU64_MAX);
    //! Construct from UTF-8 byte array.
    ZString(const ZArray<zbyte> &array);

    //! Get pointer to raw bytes.
    inline zbyte *bytes(){ return reinterpret_cast<zbyte*>(_data); }
    //! Get constant pointer to raw bytes.
    inline const zbyte *bytes() const { return reinterpret_cast<const zbyte*>(_data); }

    //! Get reference to byte \a i.
    inline zbyte &byte(zu64 i){ return bytes()[i]; }
    //! Get constant reference to byte \a i.
    inline const zbyte &byte(zu64 i) const { return bytes()[i]; }

    //! Construct from UTF-8 STL string.
    ZString(std::string str);

    //! Get UTF-8 STL string.
    std::string str() const;

    //! Construct from UTF-16 null-terminated wide C-string.
    ZString(const wchar_t *wstr, zu64 max = ZU64_MAX);
    //! Construct from UTF-16 wide character array.
    ZString(const ZArray<wchar_t> &array);

    //! Construct from UTF-16 STL wide string.
    ZString(std::wstring wstr);

    //! Get UTF-16 STL wide string.
    std::wstring wstr() const;

    //! Construct string filled with \a ch to length \a len.
    ZString(char ch, zu64 len = 1);

    //! Get string representation of unsigned integer \a num with \a base, padded to \a pad characters, using uppercase letters if \a upper.
    static ZString ItoS(zu64 num, zu8 base = 10, zu64 pad = 0, bool upper = false);
    //! Get string representation of signed integer \a num with \a base.
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

    /*! Determine if string is an integer.
     */
    bool isInteger(zu8 base = 10) const;
    int tint() const;

    /*! Parse string as a signed integer.
     *  Supports up to base 16.
     *  Returns 0 on failure.
     *  Returns ZS64_MAX on overflow.
     */
    zs64 toSint(zu8 base = 10) const;

    /*! Parse string as an unsigned integer.
     *  Supports up to base 16.
     *  Returns 0 on failure.
     *  Returns ZU64_MAX on overflow.
     */
    zu64 toUint(zu8 base = 10) const;

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
    inline void clear(){ _resize(0); }

    // Basic String Manipulation

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

    //! Get \a len characters after \a pos of string.
    ZString &substr(zu64 pos, zu64 len);
    //! Get \a len characters after \a pos of \a str.
    static ZString substr(ZString str, zu64 pos, zu64 len);

    //! Get portion of string from \a pos to end.
    ZString &substr(zu64 pos);
    //! Get portion of \a str from \a pos to end.
    static ZString substr(ZString str, zu64 pos);

    //! Insert \a txt at \a pos in string.
    ZString &insert(zu64 pos, const ZString &txt);
    //! Insert \a txt at \a pos in \a str.
    static ZString insert(ZString str, zu64 pos, const ZString &txt);

    //! Substitute \a len characters starting at \a pos with \a after in string.
    ZString &substitute(zu64 pos, zu64 len, const ZString &after);
    //! Substitute \a len characters starting at \a pos with \a after in \a str.
    static ZString substitute(ZString str, zu64 pos, zu64 len, const ZString &after);

    //! Reverse the characters of the string.
    ZString &reverse();
    //! Reverse the characters of \a str.
    static ZString reverse(ZString str);

    // Basic String Parsing

    //! Count occurrences of \a test.
    zu64 count(ZString test) const;

    //! Tests if \a str begins with \a test. Ignores whitespace at beginning of string if \a ignore_whitespace.
    bool beginsWith(const ZString &test, bool ignorewhitespace = false) const;
    //! Tests if string begins with \a test. Ignores whitespace at beginning of string if \a ignore_whitespace.
    static bool beginsWith(const ZString &str, const ZString &test, bool ignorewhitespace = false);

    //! Tests if string ends with \a test.
    bool endsWith(ZString test) const;

    /*! Get location of of first occurrence of \a find in string after \a start.
     *  \return Index of first character of \a find if found, else \ref none.
     */
    zu64 findFirst(const ZString &find, zu64 start = 0) const;
    /*! Get location of of first occurrence of \a find in \a str after \a start.
     *  \return Index of first character of \a find if found, else \ref none.
     */
    static zu64 findFirst(const ZString &str, const ZString &find, zu64 start = 0);

    //! Get locations of of all non-overlapping occurrences of \a find in string.
    ZArray<zu64> findAll(const ZString &find) const;
    //! Get locations of of all non-overlapping occurrences of \a find in \a str.
    static ZArray<zu64> findAll(const ZString &str, const ZString &find);

    // Advanced String Manipulation

    /*! Replace first occurence of \a before in string with \a after after \a start.
     *  \param before String to search for.
     *  \param after Replacement string.
     *  \param start Position to start searching.
     */
    ZString &replaceFirst(const ZString &before, const ZString &after, zu64 start = 0);
    /*! Replace first occurence of \a before in \a str with \a after after \a start.
     *  \param str String object.
     *  \param before String to search for.
     *  \param after Replacement string.
     *  \param start Position to start searching.
     */
    static ZString replaceFirst(ZString str, const ZString &before, const ZString &after, zu64 start = 0);

    /*! Replace up to \a max occurrences of \a before in string with \a after.
     *  \param before String to search for.
     *  \param after Replacement string.
     *  \param max Number of occurrences to replace (0 for unlimited).
     */
    ZString &replace(const ZString &before, const ZString &after, zu64 max = 0);
    /*! Replace up to \a max occurrences of \a before in \a str with \a after.
     *  \param str String object.
     *  \param before String to search for.
     *  \param after Replacement string.
     *  \param max Number of occurrences to replace (0 for unlimited)
     */
    static ZString replace(ZString str, const ZString &before, const ZString &after, zu64 max = 0);

    /*! Replace the first occurrence of \a before in string with \a after, up to \a max times.
     *  Replaces recursively, so will search entire string each replace.
     *  \param before String to search for.
     *  \param after Replacement string.
     *  \param max Number of occurrences to replace (0 for unlimited)
     */
    ZString &replaceRecursive(const ZString &before, const ZString &after, zu64 max = 1000);
    /*! Replace the first occurrence of \a before in \a str with \a after, up to \a max times.
     *  Replaces recursively, so will search entire string each replace.
     *  \param str String object.
     *  \param before String to search for.
     *  \param after Replacement string.
     *  \param max Number of occurrences to replace (0 for unlimited)
     */
    static ZString replaceRecursive(ZString str, const ZString &before, const ZString &after, zu64 max = 1000);

    //! Get sub-string of \a str before first occurence of \a find in \a str.
    static ZString getUntil(ZString str, const ZString &find);

    // Advanced String Parsing

    //! Get first string between \a start and \a end in string.
    ZString findFirstBetween(ZString start, ZString end);
    //! Replace first string between \a start and \a end in string with \a after.
    ZString replaceBetween(ZString start, ZString end, ZString after);

    ZString findFirstXmlTagCont(ZString tag);
    ZString replaceXmlTagCont(ZString tag, ZString after);

    ZString &label(const ZString &label, const ZString &value);
    ZString &label(const AsArZ &values);

    //! Pad left of string to minimum \a length with \a ch.
    ZString &lpad(char ch, zu64 length);
    //! Pad right of string to minimum \a length with \a ch.
    ZString &rpad(char ch, zu64 length);
    //! Alias for rpad().
    ZString &pad(char ch, zu64 length){ return rpad(ch, length); }

    //! Strip occurences of \a target from beginning of string.
    ZString &stripFront(char target);
    //! Strip occurences of \a target from beginning of \a str.
    static ZString stripFront(ZString str, char target);

    //! Strip occurences of \a target from end of string.
    ZString &stripBack(char target);
    //! Strip occurences of \a target from end of \a str.
    static ZString stripBack(ZString str, char target);

    //! Strip occurences of \a target from beginning and end of string.
    ZString &strip(char target);
    //! Strip occurences of \a target from beginning and end of \a str.
    static ZString strip(ZString str, char target);

    ZString removeWhitespace();

    //! Convert uppercase ASCII characters to lowercase ASCII equivalents.
    ZString &toLower();
    static ZString toLower(ZString str);

    //! Convert lowercase ASCII characters to uppercase ASCII equivalents.
    ZString &toUpper();
    static ZString toUpper(ZString str);

    // What is this for...?
    ZString &duplicate(zu64 iterate);

    ArZ split(ZString delim) const;

    // Explode a string into any array of substrings
    // All explode functions will treat consecutive delimiters as one delimitier
    // Delimiters at the beginning or end of a string are discarded
    ArZ explode(char delim) const;
    ArZ strExplode(const ZString &delim) const;
    ArZ quotedExplode(char delim) const;
    ArZ escapedExplode(char delim) const;
    ArZ explodeList(unsigned nargs, ...) const;
    //ArZ explode();

    static ZString compound(ArZ parts, ZString delim);

    //! Format string with a variable number arguments in \a args.
    ZString &format(ZList<ZString> args);
    //! Create a formatted string with \a format and variable number arguments in \a args.
    static ZString format(ZString formatstr, ZList<ZString> args);

    ZString &fmtarg(ZString str);
    ZString &operator%(ZString str){ return fmtarg(str); }

    static bool charIsAlphabetic(char ch);

    static bool alphaTest(ZString str1, ZString str2);

    //! Parse UTF-8 string at \a units and replace this string with normalized UTF-8.
    void parseUTF8(const codeunit8 *units, zu64 max);
    //! Parse UTF-16 string at \a units and replace this string with normalized UTF-8.
    void parseUTF16(const codeunit16 *units, zu64 max);
    //! Parse UTF-32 string at \a units and replace this string with normalized UTF-8.
    void parseUTF32(const codeunit32 *units, zu64 max);

    //! Get debug information on a UTF-8 string.
    static void debugUTF8(const codeunit *bytes);

    //! Get a unicode character reference string from a code point.
    static ZString codePointStr(zu64 cp);

    //! Allow ZString to be used with std streams.
    friend std::ostream &operator<<(std::ostream &lhs, ZString rhs);

    /*! Swap the contents of this string with another.
     *  Does not swap allocators.
     */
    void swap(ZString &other);

    inline bool isEmpty() const { return (size() == 0); }

    //! Number of bytes (code units).
    inline zu64 realSize() const { return _realsize; }

    //! Number of *characters* (code points).
    zu64 length() const;

    // On empty string, will return null terminator
    //! Get reference to the first character.
    inline char &first(){ return c()[0]; }
    //! Get const reference to the first character.
    inline const char &first() const { return cc()[0]; }
    //! Get reference to the last character.
    inline char &last(){ return c()[size() - 1]; }
    //! Get const reference to the last character.
    inline const char &last() const { return cc()[size() - 1]; }

    // ZAccessor interface
    //! Get reference to character at \a i.
    inline char &at(zu64 i){ return c()[i]; }
    //! Get constant reference to character at \a i.
    inline const char &at(zu64 i) const { return cc()[i]; }

    char *raw(){ return c(); }
    const char *raw() const { return cc(); }
    inline zu64 size() const { return _size; }

private:
    //! Resize buffer. (IMPORTANT: memory is only allocated and initialized here)
    void _reserve(zu64 size);
    //! Resize container.
    void _resize(zu64 len);

    static bool _charIsWhitespace(char ch);
    /*! Replace first occurrence of \a before with \a after starting at \a startpos.
     *  \return Index of the next character after replacement.
     */
    zu64 _strReplace(const ZString &before, const ZString &after, zu64 startpos);

    //! Append a UTF-8 encoded code point to the container.
    void _appendCodePoint(codepoint cp);
    static void _appendUTF16(std::wstring &str, codepoint cp);

    //! Decode the next UTF-8 code point.
    static codepoint _nextUTF8(const codeunit8 **units, zu64 *maxunits);
    //! Decode the next UTF-16 code point.
    static codepoint _nextUTF16(const codeunit16 **units, zu64 *maxunits);
    //! Decode the next UTF-32 code point.
    static codepoint _nextUTF32(const codeunit32 **units, zu64 *maxunits);

    //! Determine if \a str is valid UTF-8.
    static bool isUTF8(const char *str);

    //! Normalize UTF-8 in this string container.
    void unicode_normalize();

private:
    //! Allocator.
    ZAllocator<codeunit> *_alloc;
    //! Length of string in bytes.
    zu64 _size;
    //! Size of real buffer.
    zu64 _realsize;
    //! String buffer.
    codeunit *_data;
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
