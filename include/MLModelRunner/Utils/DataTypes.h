#ifndef MLBRIDGE_DATATYPES_H
#define MLBRIDGE_DATATYPES_H

namespace MLBridge {

#ifdef MLBRIDGE_EXTENDED_TYPES
    using RealType = double;
    using IntegerType = long;
#else
    using RealType = float;
    using IntegerType = int;
#endif

}

#endif