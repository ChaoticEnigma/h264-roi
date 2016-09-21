#ifndef ZPARCEL_H
#define ZPARCEL_H

#include "zuid.h"
#include "zstring.h"
#include "zbinary.h"
#include "zpath.h"
#include "zfile.h"

namespace LibChaos {

/*! Interface for storing and fetching objects from the LibChaos parcel file format.
 *  Each object is stored, fetched or updated through a unique UUID.
 */
class ZParcel {
public:
    enum parceltype {
        UNKNOWN = 0,
        VERSION1,
        MAX_PARCELTYPE,
    };

    enum {
        NULLOBJ = 0,
        BOOLOBJ,
        UINTOBJ,
        SINTOBJ,
        FLOATOBJ,
        ZUIDOBJ,
        STRINGOBJ,
        BLOBOBJ,
        FILEOBJ,
        MAX_OBJTYPE,
    };

    typedef zu8 objtype;

    enum objerr {
        OK = 0,
        ERR_READ,
        ERR_NOEXIST,
        ERR_BADCRC,
        ERR_TRUNC,
        ERR_FILEREF,
        ERR_TREE,
    };

    struct ObjectInfo {
        objerr error;
        objtype type;
        zu64 offset;
        zu64 length;
    };

public:
    ZParcel();
    ~ZParcel();

    /*! Create new parcel file and open it.
     *  This will overwrite an existing file.
     *  \exception ZException Failed to create file.
     */
    bool create(ZPath file);

    /*! Open existing parcel.
     *  \exception ZException Failed to open file.
     */
    bool open(ZPath file);
    //! Close file handles.
    void close();

    /*! Store null in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeNull(ZUID id);
    /*! Store bool in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeBool(ZUID id, bool bl);
    /*! Store unsigned int in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeUint(ZUID id, zu64 num);
    /*! Store signed int in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeSint(ZUID id, zs64 num);
    /*! Store float in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeFloat(ZUID id, double num);
    /*! Store ZUID in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeZUID(ZUID id, ZUID uid);
    /*! Store string in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeString(ZUID id, ZString str);
    /*! Store blob in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeBlob(ZUID id, ZBinary blob);
    /*! Store file reference in parcel.
     * \exception ZException Parcel not open.
     */
    objerr storeFile(ZUID id, ZPath path);

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
    ZString fetchFile(ZUID id);

    //! Get type of parcel object.
    objtype getType(ZUID id);

    //! Get string name of object type.
    static ZString typeName(objtype type);

private:
    //! Get the size of an object node a payload.
    zu64 _objectSize(objtype type, zu64 size);

    //! Store an object.
    void _storeObject(ZUID id, objtype type, const ZBinary &data, zu64 trailsize = 0);
    //! Get object info struct.
    void _getObjectInfo(ZUID id, ObjectInfo &info);

    bool _writeHeader(zu64 offset);

    //! Get reader at payload offset.
    ZReader *_getReader(ZUID id);

private:
    ZFile _file;
    parceltype _version;
    zu64 _treehead;
    zu64 _freelist;
    zu64 _tail;
};

} // namespace LibChaos

#endif // ZPARCEL_H
