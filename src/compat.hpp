#ifndef TRAFT_COMPAT_H
#define TRAFT_COMPAT_H

#ifndef TEMP_FAILURE_RETRY
#define TEMP_FAILURE_RETRY(expression) ({     \
  __typeof(expression) __result;              \
  do {                                        \
    __result = (expression);                  \
  } while (__result == -1 && errno == EINTR); \
  __result; })
#endif

#if defined(__APPLE__)
  #define up_pthread_setname(thread, name) ({ \
    int __result = 0;                         \
    if (thread == pthread_self())             \
      __result = pthread_setname_np(name);    \
    __result; })
#else
  #define up_pthread_setname pthread_setname_np
#endif

#endif
