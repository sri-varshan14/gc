#ifndef _GC_UTILS_H_
#define _GC_UTILS_H_

#define UNIMPLEMENTED                                                          \
  do {                                                                         \
    fprintf(stderr, "%s:%d: Need to Implement {{ %s }} \n", __FILE__,          \
            __LINE__, __func__);                                               \
  } while (false);

#endif // _GC_UTILS_H_
