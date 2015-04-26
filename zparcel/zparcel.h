#ifndef ZPARCEL_H
#define ZPARCEL_H

#include "zuid.h"
#include "zstring.h"
#include "zbinary.h"
#include "zpath.h"

#include "zparcel4parser.h"

namespace LibChaos {

class ZParcel {
public:
    enum parceltype {
        versionunknown = 0,
//        version2,
//        version3,
        version4 = 4,
    };

    enum fieldtype {
        nullfield,
        unsignedintfield,
        signedintfield,
        floatfield,
        zuidfield,
        stringfield,
        binaryfield,
        filefield,
    };

    typedef zu16 fieldid;

    struct Field {
        fieldid id;
        union {
            zu64 uint;
            zs64 sint;
            double flt;
            ZUID uid;
            ZString str;
            ZBinary data;
            ZPath file;
        };
    };

public:
    ZParcel();

    ~ZParcel();

    //! Create new parcel file and open it
    //! This will overwrite an existing file
    //! \exception ZException Failed to create file
    void create(ZPath path);

    //! Open existing parcel
    bool open(ZPath file);

    //! Close file handles
    void close();

    void addField(ZString name, fieldtype type);

    //! Add records to the parcel
    //! \exception ZException Invalid field type
    void addRecords(ZList<Field> fields);

    void addUintRecord(fieldid field, zu64 num);
    void addSintRecord(fieldid field, zs64 num);
    void addZUIDRecord(fieldid field, ZUID uid);
    void addFloatRecord(fieldid field, double flt);
    void addStringRecord(fieldid field, ZString str);
    void addBinaryRecord(fieldid field, ZBinary bin);
    void addFileRecord(fieldid field, ZPath file);

    static fieldtype fieldType(ZString name);
    static ZString fieldName(fieldtype type);

    static ZBinary toFile8Bits(zu8 num);
    static ZBinary toFile16Bits(zu16 num);
    static ZBinary toFile32Bits(zu32 num);
    static ZBinary toFile64Bits(zu64 num);

    static zu8 fromFile8Bits(ZBinary num);
    static zu16 fromFile16Bits(ZBinary num);
    static zu32 fromFile32Bits(ZBinary num);
    static zu64 fromFile64Bits(ZBinary num);

private:
    ZFile _file;
    ZParcel4Parser *_parser;
    parceltype _version;
};

} // namespace LibChaos

#endif // ZPARCEL_H
