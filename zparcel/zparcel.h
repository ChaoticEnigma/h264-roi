/*******************************************************************************
**                                  LibChaos                                  **
**                                  zparcel.h                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZPARCEL_H
#define ZPARCEL_H

#include "zuid.h"
#include "zstring.h"
#include "zbinary.h"
#include "zpath.h"
#include "zfile.h"
#include "zmap.h"

namespace LibChaos {

/*! Interface for storing and fetching objects from the LibChaos parcel file format.
 *  Each object is stored, fetched or updated through a unique UUID.
 */
class ZParcel {
public:
    enum parceltype {
        UNKNOWN = 0,
        VERSION1,       //!< Type 1 parcel. No pages, payload in tree node.
        MAX_PARCELTYPE,
    };

    enum parcelstate {
        OPEN,
        CLOSED,
        LOCKED,
    };

    enum {
        NULLOBJ = 0,
        BOOLOBJ,        //!< Boolean object. 1-bit.
        UINTOBJ,        //!< Unsigned integer object. 64-bit.
        SINTOBJ,        //!< Signed integer object. 64-bit.
        FLOATOBJ,       //!< Floating point number object. Double precision.
        ZUIDOBJ,        //!< UUID object.
        STRINGOBJ,      //!< String object.
        BLOBOBJ,        //!< Binary blob object.
        FILEOBJ,        //!< File object. Includes embedded filename and file content.
        /*! User-defined object types can be created by subclassing ZParcel and defining new types starting with MAX_OBJTYPE.
         *  Example:
         *  \code
         *  enum {
         *      CUSTOM1 = MAX_OBJTYPE,
         *      CUSTOM2,
         *  };
         *  \endcode
         */
        MAX_OBJTYPE,
        UNKNOWNOBJ = 255,
    };

    typedef zu8 objtype;

    enum parcelerror {
        OK = 0,         //!< No error.
        ERR_OPEN,       //!< Error opening file.
        ERR_SEEK,       //!< Error seeking file.
        ERR_READ,       //!< Error reading file.
        ERR_WRITE,      //!< Error writing file.
        ERR_EXISTS,     //!< Object exists.
        ERR_NOEXIST,    //!< Object does not exist.
        ERR_BADCRC,     //!< CRC mismatch.
        ERR_TRUNC,      //!< Payload is truncated by end of file.
        ERR_TREE,       //!< Bad tree structure.
        ERR_FREELIST,   //!< Bad freelist structure.
        ERR_SIG,        //!< Bad file signature.
        ERR_VERSION,    //!< Bad file header version.
        ERR_MAX_DEPTH,  //!< Exceeded maximum tree depth.
    };

    struct ObjectInfo {
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
    parcelerror create(ZPath file);

    /*! Open existing parcel.
     *  \exception ZException Failed to open file.
     */
    parcelerror open(ZPath file);

    //! Close file handles.
    void close();

    //! Check if \a id exists in the parcel.
    bool exists(ZUID id);
    //! Get type of parcel object.
    objtype getType(ZUID id);

    /*! Store null in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeNull(ZUID id);
    /*! Store bool in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeBool(ZUID id, bool bl);
    /*! Store unsigned int in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeUint(ZUID id, zu64 num);
    /*! Store signed int in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeSint(ZUID id, zs64 num);
    /*! Store float in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeFloat(ZUID id, double num);
    /*! Store ZUID in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeZUID(ZUID id, ZUID uid);
    /*! Store string in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeString(ZUID id, ZString str);
    /*! Store blob in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeBlob(ZUID id, ZBinary blob);
    /*! Store file reference in parcel.
     *  \exception ZException Parcel not open.
     */
    parcelerror storeFile(ZUID id, ZPath path);

    /*! Fetch bool from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    bool fetchBool(ZUID id);
    /*! Fetch unsigned int from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    zu64 fetchUint(ZUID id);
    /*! Fetch signed int from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    zs64 fetchSint(ZUID id);
    /*! Fetch float from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    double fetchFloat(ZUID id);
    /*! Fetch ZUID from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZUID fetchZUID(ZUID id);
    /*! Fetch string from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZString fetchString(ZUID id);
    /*! Fetch blob from parcel.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZBinary fetchBlob(ZUID id);
    /*! Fetch file object from parcel.
     *  If \a offset is not null, the offset of the file payload is written at \a offset.
     *  If \a size is not null, the size of the file payload is written at \a size.
     *  \return The name of the file.
     *  \exception ZException Parcel not open.
     *  \exception ZException Object does not exist.
     *  \exception ZException Object has wrong type.
     */
    ZString fetchFile(ZUID id, zu64 *offset = nullptr, zu64 *size = nullptr);

    //! Get the ZFile handle for the parcel.
    ZFile getHandle() { return _file; }

    //! Get string name of object type.
    static ZString typeName(objtype type);
    //! Get string for error.
    static ZString errorStr(parcelerror err);

protected:
    //! Compute the size of an object node payload.
    zu64 _objectSize(objtype type, zu64 size);
    /*! Store a new object with \a id and \a type.
     *  The contents of \a data are written into the payload of the new object.
     *  If \a trailsize > 0, indicates the number of bytes that should be reserved in the payload,
     *  beyond the size of \a data.
     */
    parcelerror _storeObject(ZUID id, objtype type, const ZBinary &data, zu64 trailsize = 0);
    //! Get object info struct.
    parcelerror _getObjectInfo(ZUID id, ObjectInfo &info);

private:
    //! Write a copy of the file header at \a offset.
    bool _writeHeader(zu64 offset);

private:
    parcelstate _state;
    ZFile _file;
    parceltype _version;
    zu64 _treehead;
    zu64 _freelist;
    zu64 _tail;
    ZMap<ZUID, ObjectInfo> _cache;
};

} // namespace LibChaos

#endif // ZPARCEL_H
