#ifndef MLBRIDGE_DEBUG_H
#define MLBRIDGE_DEBUG_H

namespace MLBridge {

#ifdef DEBUG_MODE
#define MLBRIDGE_DEBUG(X)                                                      \
  do {                                                                         \
    X;                                                                         \
  } while (false)
#else
#define MLBRIDGE_DEBUG(X)
#endif

} // namespace MLBridge

#endif
