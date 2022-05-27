define_normalize;
define_handle_special_cases;
define_is_inexact_p;
define_iterate_approx;
define_rescale_float;

#ifdef SPECIALIZE
typedef fp_type (*__divm3_rx_t) (fp_type, fp_type);

static __divm3_rx_t __divm3_rx[] = {
  __divm3_RN,
  __divm3_RU,
  __divm3_RD,
  __divm3_RZ
};

fp_type
__divm3 (fp_type a, fp_type b)
{
  return __div3m_rx[GET_ROUND] (a, b);
}

define_specialized_divm3 (RU, ru, rd);
define_specialized_divm3 (RD, rd, ru);
define_specialized_divm3 (RN, rn, rd);
define_specialized_divm3 (RZ, rz, rd);

#else //! SPECIALIZE
typedef fp_type (*__divm3_rx_tail_t) (fp_type, fp_type, fp_type,
                                      int, int, uint64_t *);

#define declare_tail_function(mode) \
  static inline fp_type CONCAT4(__divm3,_, mode, _tail) \
  (fp_type, fp_type, fp_type, int, int, uint64_t *)

declare_tail_function (RN);
declare_tail_function (RU);
declare_tail_function (RD);
declare_tail_function (RZ);

static __divm3_rx_tail_t __divm3_rx_tail[] = {
  __divm3_RN_tail,
  __divm3_RU_tail,
  __divm3_RD_tail,
  __divm3_RZ_tail
};


define_divm3_generic;
define_divm3_tail (RN, rn, rd);
define_divm3_tail (RZ, rz, rd);
define_divm3_tail (RU, ru, rd);
define_divm3_tail (RD, rd, ru);
#endif // SPECIALIZE
