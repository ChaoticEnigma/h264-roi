#ifndef ZSTRING_H
#define ZSTRING_H

#include <string>
#include <sstream>
#include <cstring>
#ifdef ZSTRING_USE_QT
    #include <QString>
    #include <QByteArray>
#endif
#include "zarray.h"
#include <cstdlib>

class ZString {
public:
    ZString();
    ZString(ZArray<char>);

    char &operator[](long num){ return data[num]; }

    ZString &operator=(ZString);
    bool operator==(ZString);
    bool operator!=(ZString);
    ZString operator+(ZString);
    ZString &operator+=(ZString);
    ZString &operator<<(ZString);

    ZArray<char> ZAc(){ return data; }

    ZString(std::string);
    ZString &operator=(const std::string str){ return operator=(ZString(str)); }
    bool operator==(const std::string str){ return operator==(ZString(str)); }
    bool operator!=(const std::string str){ return operator!=(ZString(str)); }
    ZString operator+(const std::string str){ return operator+(ZString(str)); }
    ZString &operator+=(const std::string str){ return operator+=(ZString(str)); }
    ZString &operator<<(const std::string str){ return operator<<(ZString(str)); }
    std::string &str();

#ifdef ZSTRING_USE_QT
    ZString(QString);
    ZString &operator=(QString);
    inline bool operator==(const QString);
    ZString operator+(QString);
    ZString &operator+=(QString);
    ZString &operator<<(QString);
    QString QS();

    ZString(QByteArray);
    ZString &operator=(QByteArray);
    inline bool operator==(const QByteArray);
    ZString operator+(QByteArray);
    ZString &operator+=(QByteArray);
    ZString &operator<<(QByteArray);
    QByteArray QBA();
#endif

    ZString(char*);
    ZString &operator=(char *str){ return operator=(ZString(str)); }
    bool operator==(char *str){ return operator==(ZString(str)); }
    bool operator!=(char *str){ return operator!=(ZString(str)); }
    ZString operator+(char *str){ return operator+(ZString(str)); }
    ZString &operator+=(char *str){ return operator+=(ZString(str)); }
    ZString &operator<<(char *str){ return operator<<(ZString(str)); }
    char *c();

    ZString(const char*);
    ZString &operator=(const char *str){ return operator=(ZString(str)); }
    bool operator==(const char *str){ return operator==(ZString(str)); }
    bool operator!=(const char *str){ return operator!=(ZString(str)); }
    ZString operator+(const char *str){ return operator+(ZString(str)); }
    ZString &operator+=(const char *str){ return operator+=(ZString(str)); }
    ZString &operator<<(const char *str){ return operator<<(ZString(str)); }
    const char *cc();

    ZString(char);
    ZString &operator=(char str){ return operator=(ZString(str)); }
    ZString operator+(char str){ return operator+(ZString(str)); }
    ZString &operator+=(char str){ return operator+=(ZString(str)); }
    ZString &operator<<(char str){ return operator<<(ZString(str)); }

#ifdef ZSTRING_USE_QT
    ZString(qint64);
#endif

    ZString(int);
    int tint();

    int size();
    int length();
    int count(std::string);

    ZString replace(std::string before, std::string after, bool modify = true);
    ZString label(std::string label, ZString value, bool modify = true);
    ZString strip(char target, bool modify = true);
    ZString substr(int, bool modify = true);
    ZString substr(int, int, bool modify = true);
    ZString invert(bool modify = true);
    ZString toLower(bool modify = true);

    friend std::ostream &operator<<(std::ostream& lhs, ZString rhs);
private:
    ZArray<char> data;
};

#endif // ZSTRING_H
