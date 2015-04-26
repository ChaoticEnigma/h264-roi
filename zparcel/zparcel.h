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

    struct FieldData {
        zu64 uint;
        zs64 sint;
        double flt;
        ZUID uid;
        ZString str;
        ZBinary data;
        ZPath file;
    };

    struct Field : public FieldData {
        fieldid id;
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

    void addRecord(fieldtype type, );

    static fieldtype fieldType(ZString name);
    static ZString fieldName(fieldtype type);

private:
    // Parcel file
    ZPath _path;
    ZFile _file;
    parceltype _version;
    ZParcel4Parser *_parser;
};

} // namespace LibChaos

#endif // ZPARCEL_H
