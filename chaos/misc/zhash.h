#ifndef ZHASH
#define ZHASH

#include "ztypes.h"
#include "xxhash.h"

namespace LibChaos {

template <typename T> class has_mapHash {
    typedef char one;
    typedef long two;
    template <typename C> static one test(typeof(&C::mapHash)) ;
    template <typename C> static two test(...);
public:
    enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <typename T> zu64 mapHash(T &data){
    if(has_mapHash<T>::value){
        return ((ZHashable*)data)->mapHash();
    } else {
        return 0;
    }
}

template <> zu64 mapHash<zu64>(zu64 num){
    return num;
}
template <> zu64 mapHash<zs64>(zs64 num){
    return (zu64)num;
}

}

#endif // ZHASH

