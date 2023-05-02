#ifndef __H__KVX_BUILTINS__
#define __H__KVX_BUILTINS__

#if defined(__llvm__) && defined(__kv3_1__)
/** LLVM KV3-1 emulation of KV3-2 builtins */

#define __get_v4i16_low(a) __builtin_shufflevector((a), (a), 0, 1, 2, 3)
#define __get_v4i16_high(a) __builtin_shufflevector((a), (a), 4, 5, 6, 7)

#define __builtin_kvx_fmulho(a, b, mod)                                        \
  (__kvx_v8hi) __builtin_shufflevector(                                        \
      __builtin_kvx_fmulhq(__get_v4i16_low(a), __get_v4i16_low(b), mod),       \
      __builtin_kvx_fmulhq(__get_v4i16_high(a), __get_v4i16_high(b), mod), 0,  \
      1, 2, 3, 4, 5, 6, 7)

#define __get_v8i16_low(a)                                                     \
  __builtin_shufflevector((a), (a), 0, 1, 2, 3, 4, 5, 6, 7)
#define __get_v8i16_high(a)                                                    \
  __builtin_shufflevector((a), (a), 8, 9, 10, 11, 12, 13, 14, 15)

#define __builtin_kvx_fmulhx(a, b, mod)                                        \
  (__kvx_v16hi) __builtin_shufflevector(                                       \
      __builtin_kvx_fmulho(__get_v8i16_low(a), __get_v8i16_low(b), mod),       \
      __builtin_kvx_fmulho(__get_v8i16_high(a), __get_v8i16_high(b), mod), 0,  \
      1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15)

#endif // defined(__llvm__) && defined(__kv3_1__)

#endif /* __H__KVX_BUILTINS__ */
