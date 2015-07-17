#include "zmath.h"

namespace LibChaos {

zu64 ZMath::abs(zs64 num){
    return (num < 0 ? (zu64)-num : (zu64)num);
}

}
