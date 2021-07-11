#ifndef _STATIC_ASSERT_H_
#define _STATIC_ASSERT_H_

#define ASSERT_CONCAT_(a, b)    a##b
#define ASSERT_CONCAT(a, b)     ASSERT_CONCAT_(a, b)

#ifdef __COUNTER__
    #define STATIC_ASSERT(e, m) \
        enum { ASSERT_CONCAT(static_assert_, __COUNTER__) = 1 / (!!(e)) }
#else
    #define STATIC_ASSERT(e, m) \
        enum ( ASSERT_CONCAT(assert_line_, __LINE__) = 1 / (!!(e)) )
#endif

#endif /* _STATIC_ASSERT_H_ */
