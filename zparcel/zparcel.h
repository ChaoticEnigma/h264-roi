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
        UNKNOWN = 0,
        VERSION1,
    };

    enum objtype {
        NULLOBJ,
        BOOLOBJ,
        UINTOBJ,
        SINTOBJ,
        FLOATOBJ,
        ZUIDOBJ,
        STRINGOBJ,
        BLOBOBJ,
        FILEOBJ,
    };

public:
    ZParcel();
    ~ZParcel();

    /*! Create new parcel file and open it.
     *  This will overwrite an existing file.
     *  \exception ZException Failed to create file.
     */
    void create(ZPath path);

    /*! Open existing parcel.
     *  \exception ZException Failed to open file.
     */
    bool open(ZPath file);
    //! Close file handles.
    void close();

    /*! Store null in parcel.
     * \exception ZException Parcel not open.
     */
    void storeNull(ZUID id);
    /*! Store bool in parcel.
     * \exception ZException Parcel not open.
     */
    void storeBool(ZUID id, bool bl);
    /*! Store unsigned int in parcel.
     * \exception ZException Parcel not open.
     */
    void storeUint(ZUID id, zu64 num);
    /*! Store signed int in parcel.
     * \exception ZException Parcel not open.
     */
    void storeSint(ZUID id, zs64 num);
    /*! Store float in parcel.
     * \exception ZException Parcel not open.
     */
    void storeFloat(ZUID id, double num);
    /*! Store ZUID in parcel.
     * \exception ZException Parcel not open.
     */
    void storeZUID(ZUID id, ZUID uid);
    /*! Store string in parcel.
     * \exception ZException Parcel not open.
     */
    void storeString(ZUID id, ZString str);
    /*! Store blob in parcel.
     * \exception ZException Parcel not open.
     */
    void storeBlob(ZUID id, ZBinary blob);
    /*! Store file reference in parcel.
     * \exception ZException Parcel not open.
     */
    void storeFile(ZUID id, ZFile file);

    /*! Fetch bool from parcel.
     * \exception ZException Parcel not open.
     * \exception ZException Object does not exist.
     * \exception ZException Object has wrong type.
     */
    bool fetchBool(ZUID id);
    /*! Fetch unsigned int from parcel.
     * \exception ZException Parcel not open.
     * \exception ZException Object does not exist.
     * \exception ZException Object has wrong type.
     */
    zu64 fetchUint(ZUID id);
    /*! Fetch signed int from parcel.
     * \exception ZException Parcel not open.
     * \exception ZException Object does not exist.
     * \exception ZException Object has wrong type.
     */
    zs64 fetchSint(ZUID id);
    /*! Fetch float from parcel.
     * \exception ZException Parcel not open.
     * \exception ZException Object does not exist.
     * \exception ZException Object has wrong type.
     */
    double fetchFloat(ZUID id);
    /*! Fetch ZUID from parcel.
     * \exception ZException Parcel not open.
     * \exception ZException Object does not exist.
     * \exception ZException Object has wrong type.
     */
    ZUID fetchZUID(ZUID id);
    /*! Fetch string from parcel.
     * \exception ZException Parcel not open.
     * \exception ZException Object does not exist.
     * \exception ZException Object has wrong type.
     */
    ZString fetchString(ZUID id);
    /*! Fetch blob from parcel.
     * \exception ZException Parcel not open.
     * \exception ZException Object does not exist.
     * \exception ZException Object has wrong type.
     */
    ZBinary fetchBlob(ZUID id);
    /*! Fetch file reference from parcel.
     * \exception ZException Parcel not open.
     * \exception ZException Object does not exist.
     * \exception ZException Object has wrong type.
     */
    ZFile fetchFile(ZUID id);

    //! Get type of parcel object.
    objtype getType(ZUID id);

    //! Get string name of object type.
    static ZString typeName(objtype type);

private:
    ZFile _file;
    ZParcel4Parser *_parser;
    parceltype _version;
};

} // namespace LibChaos

#endif // ZPARCEL_H
