#pragma once

//We check this since this is the deepest file in the engine

#if defined(_MSC_VER) && !defined(__clang__)
# if !defined(USING)
#   define IN_USE &&
#   define NOT_IN_USE &&!
#   define USE_IF(x) &&((x)?1:0)&&
#   define USING(x) (1 x 1)
# endif // #if !defined(USING)
#else
# ifndef GE_USING_BOOL
#   define GE_USING_BOOL(x) ((x) ? 1 : 0)
# endif
# if !defined(USING)
#   define USING(x)      (GE_USING_BOOL(x) == 1)
#   define NOT_USING(x)  (GE_USING_BOOL(x) == 0)
#   define USE_IF(x) GE_USING_BOOL(x)
#   define IN_USE 1
#   define NOT_IN_USE 0
# endif // #if !defined(USING)
#endif
