#ifndef ZPARCEL_H
#define ZPARCEL_H

#include "zuid.h"
#include "zstring.h"
#include "zbinary.h"
#include "zpath.h"

#include "zparcelrecord.h"
#include "zparcel4parser.h"

namespace LibChaos {

class ZParcel {
public:
    enum parceltype {
        versionunknown = 0,
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
        ZString id;
        ZString data;
    };

public:
    ZParcel();

    ~ZParcel();

    //! Create new parcel file and open it
    //! This will overwrite an existing file
    //! \exception ZException Failed to create file
    void create(ZPath path);

    //! Open existing parcel
    //! \exception ZException Failed to open file
    bool open(ZPath file);

    //! Close file handles
    void close();

    //! Add new field to parcel
    void addField(ZString name, fieldtype type);

    //! Add a record to the parcel
    //! \exception ZException Invalid field type
    void addRecord(ZParcelRecord record);

    //! Add multiple records to the parcel
    //! \exception ZException Invalid field type
    void addRecords(ZList<ZParcelRecord> records);

    fieldid getFieldId(ZString name);

    static fieldtype typeType(ZString name);
    static ZString typeName(fieldtype type);

private:
    ZFile _file;
    ZParcel4Parser *_parser;
    parceltype _version;
};

} // namespace LibChaos

#endif // ZPARCEL_H
