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
        boolfield,
        uintfield,
        sintfield,
        floatfield,
        zuidfield,
        stringfield,
        blobfield,
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

    //! Add new data field to parcel
    void addField(ZString name, fieldtype type);

    //! Add records to the parcel
    //! \exception ZException Invalid field type
    void addRecords(ZList<Field> fields);

    void addBoolRecord(ZString field, bool tf);
    void addUintRecord(ZString field, zu64 num);
    void addSintRecord(ZString field, zs64 num);
    void addZUIDRecord(ZString field, ZUID uid);
    void addFloatRecord(ZString field, double flt);
    void addStringRecord(ZString field, ZString str);
    void addBinaryRecord(ZString field, ZBinary bin);
    void addFileRecord(ZString field, ZPath file);

    fieldid getFieldId(ZString name);

    static fieldtype fieldType(ZString name);
    static ZString fieldName(fieldtype type);

private:
    ZFile _file;
    ZParcel4Parser *_parser;
    parceltype _version;
};

} // namespace LibChaos

#endif // ZPARCEL_H
