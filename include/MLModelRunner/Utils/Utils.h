#ifndef MLBRIDGE_UTILS_H
#define MLBRIDGE_UTILS_H

namespace MLBridge {

// #ifdef DEBUG
#define MLBridge_DEBUG(X)                                                      \
  do {                                                                         \
    X;                                                                         \
  } while (false)

// namespace MLBridge
// #define MLBridge_DEBUG(X)
// #endif

// #define MLBridge_DEBUG(X)                                                      \
//   ({                                                                           \
//     if (MLBridge::debug) {                                                     \
//       X;                                                                       \
//     }                                                                          \
//   })

} // namespace MLBridge

#endif
