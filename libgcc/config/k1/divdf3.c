#ifdef __k1b__
/** generated using metalibm 0.1
 * sha1 git: 5af92bbd5c9d0a9e5e15e8d3fb87abde406e85f7 **/
#include <support_lib/ml_utils.h>
#include <support_lib/k1/k1a_utils.h>
#include <support_lib/ml_multi_prec_lib.h>
#include <support_lib/ml_special_values.h>

double
__divdf3 (double x, double y)
{
  double tmp;
  double new_div0;
  ml_int64_t tmp6;
  double result;
  double yerr_last;
  ml_int32_t b0_specific_case_bit;
  double final_approx;
  double fast_init_approx;
  double iter_1;
  double iter_0;
  ml_int32_t specific_case;
  double iter_2;
  ml_int64_t tmp5;
  double tmp4;
  double yerr0;
  double tmp1;
  double tmp3;
  double tmp2;

  fast_init_approx = __builtin_k1_fsdivd (x, y);
  tmp = __builtin_k1_ffmsrnd (1.0, y, fast_init_approx);
  iter_0 = __builtin_k1_ffmarnd (fast_init_approx, tmp, fast_init_approx);
  tmp1 = __builtin_k1_ffmsrnd (1.0, y, iter_0);
  iter_1 = __builtin_k1_ffmarnd (iter_0, tmp1, iter_0);
  tmp2 = __builtin_k1_ffmsrnd (1.0, y, iter_1);
  iter_2 = __builtin_k1_ffmarnd (iter_1, tmp2, iter_1);
  tmp3 = __builtin_k1_ffmsrnd (1.0, y, iter_2);
  final_approx = __builtin_k1_ffmarnd (iter_2, tmp3, iter_2);
  tmp4 = __builtin_k1_fmulrnd (x, final_approx);
  yerr0 = __builtin_k1_ffmsrnd (x, tmp4, y);
  new_div0 = __builtin_k1_ffmarnd (tmp4, yerr0, final_approx);
  yerr_last = __builtin_k1_ffmsrnd (x, new_div0, y);
  result = __builtin_k1_ffmad (new_div0, yerr_last, final_approx);
  tmp5 = double_to_64b_encoding (fast_init_approx);
  tmp6 = tmp5 & 1;
  b0_specific_case_bit = tmp6 != 0;
  specific_case = b0_specific_case_bit != 0;

  if (__builtin_expect (specific_case, 1))
    {
      return result;
    }
  else
    {
      ml_int32_t b9_direct_result_flag;
      ml_int64_t tmp7;
      ml_int32_t direct_result_case;
      ml_int64_t tmp0;

      tmp0 = double_to_64b_encoding (fast_init_approx);
      tmp7 = tmp0 & 512;
      b9_direct_result_flag = tmp7 != 0;
      direct_result_case = b9_direct_result_flag == 0;

      if (direct_result_case)
	{
	  return fast_init_approx;
	}
      else
	{
	  ml_int32_t b7_mult_dividend_underflow;
	  ml_int64_t tmp9;
	  ml_int64_t tmp8;
	  ml_int32_t tmp24;
	  ml_int32_t tmp20;
	  ml_int32_t tmp21;
	  ml_int32_t tmp22;
	  ml_int32_t tmp23;
	  ml_int32_t b1_y_sub_or_zero;
	  ml_int32_t b4_inv_underflow;
	  ml_int64_t tmp16;
	  ml_int32_t b8_mult_dividend_overflow;
	  ml_int64_t tmp19;
	  ml_int64_t tmp18;
	  ml_int32_t b6_mult_error_underflow;
	  ml_int64_t tmp15;
	  ml_int64_t tmp14;
	  ml_int64_t tmp17;
	  ml_int32_t b2_x_sub_or_zero;
	  ml_int64_t tmp11;
	  ml_int64_t tmp10;
	  ml_int64_t tmp13;
	  ml_int64_t tmp12;

	  tmp8 = double_to_64b_encoding (fast_init_approx);
	  tmp9 = tmp8 & 128;
	  b7_mult_dividend_underflow = tmp9 != 0;
	  tmp10 = double_to_64b_encoding (fast_init_approx);
	  tmp11 = tmp10 & 256;
	  b8_mult_dividend_overflow = tmp11 != 0;
	  tmp12 = double_to_64b_encoding (fast_init_approx);
	  tmp13 = tmp12 & 64;
	  b6_mult_error_underflow = tmp13 != 0;
	  tmp14 = double_to_64b_encoding (fast_init_approx);
	  tmp15 = tmp14 & 16;
	  b4_inv_underflow = tmp15 != 0;
	  tmp16 = double_to_64b_encoding (fast_init_approx);
	  tmp17 = tmp16 & 2;
	  b1_y_sub_or_zero = tmp17 != 0;
	  tmp18 = double_to_64b_encoding (fast_init_approx);
	  tmp19 = tmp18 & 4;
	  b2_x_sub_or_zero = tmp19 != 0;
	  tmp20 = b2_x_sub_or_zero || b1_y_sub_or_zero;
	  tmp21 = tmp20 || b4_inv_underflow;
	  tmp22 = tmp21 || b6_mult_error_underflow;
	  tmp23 = tmp22 || b8_mult_dividend_overflow;
	  tmp24 = tmp23 || b7_mult_dividend_underflow;

	  if (__builtin_expect (tmp24, 0))
	    {
	      ml_int32_t tmp25;
	      ml_int32_t tmp26;
	      ml_int32_t tmp27;

	      tmp25 = ml_is_zero (x);
	      tmp26 = ml_is_zero (y);
	      tmp27 = tmp25 || tmp26;

	      if (__builtin_expect (tmp27, 0))
		{
		  return fast_init_approx;
		}
	      else
		{
		  ml_int32_t tmp49;
		  double iter_20;
		  double iter_00;
		  double sfx_ei;
		  double scaled_vx;
		  double scaled_vy;
		  ml_int32_t ey;
		  ml_int32_t ex;
		  double tmp42;
		  ml_int32_t tmp43;
		  double tmp40;
		  double tmp41;
		  ml_int32_t tmp46;
		  double tmp47;
		  ml_int32_t tmp44;
		  ml_int32_t tmp45;
		  double final_approx0;
		  double tmp48;
		  double new_div00;
		  ml_int32_t tmp28;
		  ml_int32_t tmp29;
		  double slow_init_approx;
		  double sfy_ei;
		  double iter_10;
		  double result0;
		  double unscaled_div_result;
		  double yerr_last0;
		  ml_int32_t tmp51;
		  ml_int32_t tmp50;
		  double tmp53;
		  ml_int32_t tmp52;
		  ml_int32_t tmp55;
		  double tmp54;
		  ml_int32_t tmp57;
		  double tmp56;
		  ml_int32_t scale_factor_2;
		  ml_int32_t scale_factor_1;
		  ml_int32_t scale_factor_0;
		  double yerr00;
		  double tmp39;
		  double tmp38;
		  ml_int32_t tmp33;
		  ml_int32_t tmp32;
		  ml_int32_t tmp31;
		  ml_int32_t tmp30;
		  ml_int32_t tmp37;
		  ml_int32_t tmp36;
		  ml_int32_t tmp35;
		  ml_int32_t tmp34;

		  tmp28 = ml_exp_extraction_dirty_fp64 (x);
		  tmp29 = tmp28 < 1020;
		  tmp30 = tmp29 ? tmp28 : 1020;
		  tmp31 = tmp30 > -1020;
		  ex = tmp31 ? tmp30 : -1020;
		  tmp32 = -ex;
		  sfx_ei = ml_exp_insertion_fp64 (tmp32);
		  scaled_vx = __builtin_k1_fmulrnd (x, sfx_ei);
		  tmp33 = ml_exp_extraction_dirty_fp64 (y);
		  tmp34 = tmp33 < 1020;
		  tmp35 = tmp34 ? tmp33 : 1020;
		  tmp36 = tmp35 > -1020;
		  ey = tmp36 ? tmp35 : -1020;
		  tmp37 = -ey;
		  sfy_ei = ml_exp_insertion_fp64 (tmp37);
		  scaled_vy = __builtin_k1_fmulrnd (y, sfy_ei);
		  slow_init_approx = __builtin_k1_fsdivd (scaled_vx, scaled_vy);
		  tmp38
		    = __builtin_k1_ffmsrnd (1.0, scaled_vy, slow_init_approx);
		  iter_00 = __builtin_k1_ffmarnd (slow_init_approx, tmp38,
						  slow_init_approx);
		  tmp39 = __builtin_k1_ffmsrnd (1.0, scaled_vy, iter_00);
		  iter_10 = __builtin_k1_ffmarnd (iter_00, tmp39, iter_00);
		  tmp40 = __builtin_k1_ffmsrnd (1.0, scaled_vy, iter_10);
		  iter_20 = __builtin_k1_ffmarnd (iter_10, tmp40, iter_10);
		  tmp41 = __builtin_k1_ffmsrnd (1.0, scaled_vy, iter_20);
		  final_approx0
		    = __builtin_k1_ffmarnd (iter_20, tmp41, iter_20);
		  tmp42 = __builtin_k1_fmulrnd (scaled_vx, final_approx0);
		  yerr00 = __builtin_k1_ffmsrnd (scaled_vx, tmp42, scaled_vy);
		  new_div00
		    = __builtin_k1_ffmarnd (tmp42, yerr00, final_approx0);
		  yerr_last0
		    = __builtin_k1_ffmsrnd (scaled_vx, new_div00, scaled_vy);
		  unscaled_div_result
		    = __builtin_k1_ffmad (new_div00, yerr_last0, final_approx0);
		  tmp43 = ex - ey;
		  tmp44 = tmp43 < 950;
		  tmp45 = tmp44 ? tmp43 : 950;
		  tmp46 = tmp45 > -950;
		  scale_factor_0 = tmp46 ? tmp45 : -950;
		  tmp47 = ml_exp_insertion_fp64 (scale_factor_0);
		  tmp48 = unscaled_div_result * tmp47;
		  tmp49 = tmp43 - scale_factor_0;
		  tmp50 = tmp49 < 950;
		  tmp51 = tmp50 ? tmp49 : 950;
		  tmp52 = tmp51 > -950;
		  scale_factor_1 = tmp52 ? tmp51 : -950;
		  tmp53 = ml_exp_insertion_fp64 (scale_factor_1);
		  tmp54 = tmp48 * tmp53;
		  tmp55 = scale_factor_1 + scale_factor_0;
		  scale_factor_2 = tmp43 - tmp55;
		  tmp56 = ml_exp_insertion_fp64 (scale_factor_2);
		  result0 = tmp54 * tmp56;
		  tmp57 = ml_is_subnormal (result0);

		  if (tmp57)
		    {
		      ml_int32_t tmp68;
		      double tmp69;
		      double subnormal_pre_result;
		      double tmp71;
		      double subnormal_result;
		      ml_int32_t tmp59;
		      ml_int32_t tmp58;
		      ml_int32_t tmp62;
		      double tmp63;
		      double tmp64;
		      ml_int32_t tmp65;
		      ml_int32_t tmp66;
		      ml_int32_t tmp67;
		      ml_int32_t tmp60;
		      ml_int32_t tmp70;
		      double sr_ey_ei;
		      ml_int32_t tmp61;
		      ml_dd_t ext_pre_result;
		      ml_int32_t sub_scale_factor_2;
		      ml_int32_t sub_scale_factor_0;
		      ml_int32_t sub_scale_factor_1;

		      ext_pre_result
			= ml_fma_dd_d3 (yerr_last0, final_approx0, new_div00);
		      tmp58 = ex - ey;
		      subnormal_pre_result
			= k1_subnormalize_d_dd_i (ext_pre_result, tmp58);
		      tmp59 = ex - ey;
		      tmp60 = tmp59 < 950;
		      tmp61 = tmp60 ? tmp59 : 950;
		      tmp62 = tmp61 > -950;
		      sub_scale_factor_0 = tmp62 ? tmp61 : -950;
		      tmp63 = ml_exp_insertion_fp64 (sub_scale_factor_0);
		      tmp64 = subnormal_pre_result * tmp63;
		      tmp65 = tmp59 - sub_scale_factor_0;
		      tmp66 = tmp65 < 950;
		      tmp67 = tmp66 ? tmp65 : 950;
		      tmp68 = tmp67 > -950;
		      sub_scale_factor_1 = tmp68 ? tmp67 : -950;
		      sr_ey_ei = ml_exp_insertion_fp64 (sub_scale_factor_1);
		      tmp69 = tmp64 * sr_ey_ei;
		      tmp70 = sub_scale_factor_1 + sub_scale_factor_0;
		      sub_scale_factor_2 = tmp59 - tmp70;
		      tmp71 = ml_exp_insertion_fp64 (sub_scale_factor_2);
		      subnormal_result = tmp69 * tmp71;
		      return subnormal_result;
		    }
		  else
		    {
		      return result0;
		    }
		}
	    }
	  else
	    {
	      ml_int64_t tmp25;
	      ml_int64_t tmp26;
	      ml_int32_t b5_x_inf_or_nan;

	      tmp25 = double_to_64b_encoding (fast_init_approx);
	      tmp26 = tmp25 & 32;
	      b5_x_inf_or_nan = tmp26 != 0;

	      if (__builtin_expect (b5_x_inf_or_nan, 0))
		{
		  return fast_init_approx;
		}
	      else
		{
		  ml_int32_t b3_y_inf_or_nan;
		  ml_int64_t tmp27;
		  ml_int64_t tmp28;

		  tmp27 = double_to_64b_encoding (fast_init_approx);
		  tmp28 = tmp27 & 8;
		  b3_y_inf_or_nan = tmp28 != 0;

		  if (__builtin_expect (b3_y_inf_or_nan, 0))
		    {
		      return fast_init_approx;
		    }
		  else
		    {
		      ml_int32_t div_overflow_case;
		      ml_int64_t tmp30;
		      ml_int64_t tmp29;
		      ml_int32_t b10_div_overflow;

		      tmp29 = double_to_64b_encoding (fast_init_approx);
		      tmp30 = tmp29 & 1024;
		      b10_div_overflow = tmp30 != 0;
		      div_overflow_case = b10_div_overflow != 0;

		      if (div_overflow_case)
			{
			  double signed_inf;

			  signed_inf
			    = k1_round_signed_overflow_fp64 (fast_init_approx);
			  return signed_inf;
			}
		      else
			{
			  return fp64_sv_SNaN.d;
			}
		    }
		}
	    }
	}
    }
}

#endif /* ifdef __k1b__ */
