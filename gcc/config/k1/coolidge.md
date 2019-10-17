(automata_option "ndfa")
(automata_option "v")

(define_automaton "coolidge_exu")
(define_automaton "coolidge_issue")

(define_cpu_unit
  "k1c_issue0_u,
   k1c_issue1_u,
   k1c_issue2_u,
   k1c_issue3_u,
   k1c_issue4_u,
   k1c_issue5_u,
   k1c_issue6_u,
   k1c_issue7_u"
  "coolidge_issue")

(define_cpu_unit
  "k1c_tiny0_u,
   k1c_tiny1_u,
   k1c_tiny2_u,
   k1c_tiny3_u,
   k1c_lite0_u,
   k1c_lite1_u,
   k1c_full_u,
   k1c_lsu_u,
   k1c_mau_u,
   k1c_bcu_u,
   k1c_tca_u,
   k1c_auxr_u,
   k1c_auxw_u,
   k1c_crrp_u,
   k1c_crwl_u,
   k1c_crwh_u"
  "coolidge_exu")

(absence_set "k1c_tiny0_u" "k1c_lite0_u,k1c_full_u")
(absence_set "k1c_tiny1_u" "k1c_lite1_u")
(absence_set "k1c_tiny2_u" "k1c_mau_u")
(absence_set "k1c_tiny3_u" "k1c_lsu_u")
(absence_set "k1c_lite0_u" "k1c_tiny0_u, k1c_full_u")
(absence_set "k1c_lite1_u" "k1c_tiny1_u")
(absence_set "k1c_full_u"  "k1c_tiny0_u, k1c_lite0_u")
(absence_set "k1c_mau_u"   "k1c_tiny2_u")
(absence_set "k1c_lsu_u"   "k1c_tiny3_u")

(define_reservation "k1c_issue_r"
  "(k1c_issue0_u|k1c_issue1_u|k1c_issue2_u|k1c_issue3_u|k1c_issue4_u|k1c_issue5_u|k1c_issue6_u|k1c_issue7_u)")
(define_reservation "k1c_issue_x2_r"
  "(k1c_issue0_u+k1c_issue1_u)|(k1c_issue1_u+k1c_issue2_u)|(k1c_issue2_u+k1c_issue3_u)|(k1c_issue3_u+k1c_issue4_u)|(k1c_issue4_u+k1c_issue5_u)|(k1c_issue5_u+k1c_issue6_u)|(k1c_issue6_u+k1c_issue7_u)")
(define_reservation "k1c_issue_x3_r"
  "(k1c_issue0_u+k1c_issue1_u+k1c_issue2_u)|(k1c_issue1_u+k1c_issue2_u+k1c_issue3_u)|(k1c_issue2_u+k1c_issue3_u+k1c_issue4_u)|(k1c_issue3_u+k1c_issue4_u+k1c_issue5_u)|(k1c_issue4_u+k1c_issue5_u+k1c_issue6_u)|(k1c_issue5_u+k1c_issue6_u+k1c_issue7_u)")
(define_reservation "k1c_issue_x4_r"
  "(k1c_issue0_u+k1c_issue1_u+k1c_issue2_u+k1c_issue3_u)|(k1c_issue1_u+k1c_issue2_u+k1c_issue3_u+k1c_issue4_u)|(k1c_issue2_u+k1c_issue3_u+k1c_issue4_u+k1c_issue5_u)|(k1c_issue3_u+k1c_issue4_u+k1c_issue5_u+k1c_issue6_u)|(k1c_issue4_u+k1c_issue5_u+k1c_issue6_u+k1c_issue7_u)")
(define_reservation "k1c_issue_x6_r"
  "(k1c_issue0_u+k1c_issue1_u+k1c_issue2_u+k1c_issue3_u+k1c_issue4_u+k1c_issue5_u)|(k1c_issue1_u+k1c_issue2_u+k1c_issue3_u+k1c_issue4_u+k1c_issue5_u+k1c_issue6_u)|(k1c_issue2_u+k1c_issue3_u+k1c_issue4_u+k1c_issue5_u+k1c_issue6_u+k1c_issue7_u)")
(define_reservation "k1c_issue_x8_r"
  "(k1c_issue0_u+k1c_issue1_u+k1c_issue2_u+k1c_issue3_u+k1c_issue4_u+k1c_issue5_u+k1c_issue6_u+k1c_issue7_u)")

(define_reservation "k1c_tiny_r" "(k1c_tiny0_u | k1c_tiny1_u | k1c_tiny2_u | k1c_tiny3_u )")
(define_reservation "k1c_tiny_x2_r" "((k1c_tiny0_u+k1c_tiny1_u) | (k1c_tiny1_u+k1c_tiny2_u) | (k1c_tiny2_u+k1c_tiny3_u))")
(define_reservation "k1c_tiny_x4_r" "(k1c_tiny0_u+k1c_tiny1_u+k1c_tiny2_u+k1c_tiny3_u)")
(define_reservation "k1c_lite_r" "(k1c_lite0_u | k1c_lite1_u)")
(define_reservation "k1c_lite_x2_r" "(k1c_lite0_u+k1c_lite1_u)")
(define_reservation "k1c_full_r"  "(k1c_full_u)")

(define_reservation "k1c_all_r" "k1c_tiny_x4_r + k1c_bcu_u + k1c_tca_u + k1c_issue_x8_r")
(define_reservation "k1c_alu_tiny_r" "k1c_tiny_r + k1c_issue_r")
(define_reservation "k1c_alu_tiny_x_r" "k1c_tiny_r + k1c_issue_x2_r")
(define_reservation "k1c_alu_tiny_y_r" "k1c_tiny_r + k1c_issue_x3_r")
(define_reservation "k1c_alu_tiny_x2_r" "k1c_tiny_x2_r + k1c_issue_x2_r")
(define_reservation "k1c_alu_tiny_x2_x_r" "k1c_tiny_x2_r + k1c_issue_x4_r")
(define_reservation "k1c_alu_tiny_x2_y_r" "k1c_tiny_x2_r + k1c_issue_x6_r")
(define_reservation "k1c_alu_tiny_x4_r" "k1c_tiny_x4_r + k1c_issue_x4_r")
(define_reservation "k1c_alu_tiny_x4_x_r" "k1c_tiny_x4_r + k1c_issue_x8_r")
(define_reservation "k1c_alu_lite_r" "k1c_lite_r + k1c_issue_r")
(define_reservation "k1c_alu_lite_x_r" "k1c_lite_r + k1c_issue_x2_r")
(define_reservation "k1c_alu_lite_y_r" "k1c_lite_r + k1c_issue_x3_r")
(define_reservation "k1c_alu_lite_x2_r" "k1c_lite_x2_r + k1c_issue_x2_r")
(define_reservation "k1c_alu_lite_x2_x_r" "k1c_lite_x2_r + k1c_issue_x4_r")
(define_reservation "k1c_alu_full_r" "k1c_full_r + k1c_issue_r")
(define_reservation "k1c_alu_full_x_r" "k1c_full_r + k1c_issue_x2_r")
(define_reservation "k1c_alu_full_y_r" "k1c_full_r + k1c_issue_x3_r")
(define_reservation "k1c_lsu_r" "k1c_lsu_u + k1c_tiny_r + k1c_issue_r")
(define_reservation "k1c_lsu_x_r" "k1c_lsu_u + k1c_tiny_r + k1c_issue_x2_r")
(define_reservation "k1c_lsu_y_r" "k1c_lsu_u + k1c_tiny_r + k1c_issue_x3_r")
(define_reservation "k1c_lsu_auxr_auxw_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxr_u + k1c_auxw_u + k1c_issue_r")
(define_reservation "k1c_lsu_auxr_auxw_x_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxr_u + k1c_auxw_u + k1c_issue_x2_r")
(define_reservation "k1c_lsu_auxr_auxw_y_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxr_u + k1c_auxw_u + k1c_issue_x3_r")
(define_reservation "k1c_lsu_auxw_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxw_u + k1c_issue_r")
(define_reservation "k1c_lsu_auxw_x_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxw_u + k1c_issue_x2_r")
(define_reservation "k1c_lsu_auxw_y_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxw_u + k1c_issue_x3_r")
(define_reservation "k1c_lsu_auxr_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxr_u + k1c_issue_r")
(define_reservation "k1c_lsu_auxr_x_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxr_u + k1c_issue_x2_r")
(define_reservation "k1c_lsu_auxr_y_r" "k1c_lsu_u + k1c_tiny_r + k1c_auxr_u + k1c_issue_x3_r")
(define_reservation "k1c_lsu_crrp_r" "k1c_lsu_u + k1c_tiny_r + k1c_crrp_u + k1c_issue_r")
(define_reservation "k1c_lsu_crrp_x_r" "k1c_lsu_u + k1c_tiny_r + k1c_crrp_u + k1c_issue_x2_r")
(define_reservation "k1c_lsu_crrp_y_r" "k1c_lsu_u + k1c_tiny_r + k1c_crrp_u + k1c_issue_x3_r")
(define_reservation "k1c_mau_r" "k1c_mau_u + k1c_issue_r")
(define_reservation "k1c_mau_x_r" "k1c_mau_u + k1c_issue_x2_r")
(define_reservation "k1c_mau_y_r" "k1c_mau_u + k1c_issue_x3_r")
(define_reservation "k1c_mau_auxr_r" "k1c_mau_u + k1c_auxr_u + k1c_issue_r")
(define_reservation "k1c_mau_auxr_x_r" "k1c_mau_u + k1c_auxr_u + k1c_issue_x2_r")
(define_reservation "k1c_mau_auxr_y_r" "k1c_mau_u + k1c_auxr_u + k1c_issue_x3_r")
(define_reservation "k1c_bcu_r" "k1c_bcu_u + k1c_issue_r")
(define_reservation "k1c_bcu_tiny_tiny_mau_r" "k1c_bcu_u + k1c_tiny_x2_r + k1c_mau_u + k1c_issue_r")
(define_reservation "k1c_bcu_tiny_auxw_crrp_r" "k1c_bcu_u + k1c_tiny_r + k1c_auxw_u + k1c_crrp_u + k1c_issue_r")
(define_reservation "k1c_bcu_crrp_crwl_crwh_r" "k1c_bcu_u + k1c_crrp_u + k1c_crwl_u + k1c_crwh_u + k1c_issue_r")
(define_reservation "k1c_tca_r" "k1c_tca_u + k1c_issue_r")

(define_insn_reservation "k1c_all" 1 (eq_attr "type" "all") "k1c_all_r")
(define_insn_reservation "k1c_alu_tiny" 1 (eq_attr "type" "alu_tiny") "k1c_alu_tiny_r")
(define_insn_reservation "k1c_alu_tiny_x" 1 (eq_attr "type" "alu_tiny_x") "k1c_alu_tiny_x_r")
(define_insn_reservation "k1c_alu_tiny_y" 1 (eq_attr "type" "alu_tiny_y") "k1c_alu_tiny_y_r")
(define_insn_reservation "k1c_alu_tiny_x2" 1 (eq_attr "type" "alu_tiny_x2") "k1c_alu_tiny_x2_r")
(define_insn_reservation "k1c_alu_tiny_x2_x" 1 (eq_attr "type" "alu_tiny_x2_x") "k1c_alu_tiny_x2_x_r")
(define_insn_reservation "k1c_alu_tiny_x2_y" 1 (eq_attr "type" "alu_tiny_x2_y") "k1c_alu_tiny_x2_y_r")
(define_insn_reservation "k1c_alu_tiny_x4" 1 (eq_attr "type" "alu_tiny_x4") "k1c_alu_tiny_x4_r")
(define_insn_reservation "k1c_alu_tiny_x4_x" 1 (eq_attr "type" "alu_tiny_x4_x") "k1c_alu_tiny_x4_x_r")
(define_insn_reservation "k1c_alu_lite" 1 (eq_attr "type" "alu_lite") "k1c_alu_lite_r")
(define_insn_reservation "k1c_alu_lite_x" 1 (eq_attr "type" "alu_lite_x") "k1c_alu_lite_x_r")
(define_insn_reservation "k1c_alu_lite_y" 1 (eq_attr "type" "alu_lite_y") "k1c_alu_lite_y_r")
(define_insn_reservation "k1c_alu_lite_x2" 1 (eq_attr "type" "alu_lite_x2") "k1c_alu_lite_x2_r")
(define_insn_reservation "k1c_alu_lite_x2_x" 1 (eq_attr "type" "alu_lite_x2_x") "k1c_alu_lite_x2_x_r")
(define_insn_reservation "k1c_alu_full" 1 (eq_attr "type" "alu_full") "k1c_alu_full_r")
(define_insn_reservation "k1c_alu_full_x" 1 (eq_attr "type" "alu_full_x") "k1c_alu_full_x_r")
(define_insn_reservation "k1c_alu_full_y" 1 (eq_attr "type" "alu_full_y") "k1c_alu_full_y_r")
(define_insn_reservation "k1c_alu_full_copro" 15 (eq_attr "type" "alu_full_copro") "k1c_alu_full_r")
(define_insn_reservation "k1c_lsu" 1 (eq_attr "type" "lsu") "k1c_lsu_r")
(define_insn_reservation "k1c_lsu_x" 1 (eq_attr "type" "lsu_x") "k1c_lsu_x_r")
(define_insn_reservation "k1c_lsu_y" 1 (eq_attr "type" "lsu_y") "k1c_lsu_y_r")
(define_insn_reservation "k1c_lsu_auxw_load" 3 (eq_attr "type" "lsu_auxw_load") "k1c_lsu_auxw_r")
(define_insn_reservation "k1c_lsu_auxw_load_x" 3 (eq_attr "type" "lsu_auxw_load_x") "k1c_lsu_auxw_x_r")
(define_insn_reservation "k1c_lsu_auxw_load_y" 3 (eq_attr "type" "lsu_auxw_load_y") "k1c_lsu_auxw_y_r")
(define_insn_reservation "k1c_lsu_auxw_load_uncached" 24 (eq_attr "type" "lsu_auxw_load_uncached") "k1c_lsu_auxw_r")
(define_insn_reservation "k1c_lsu_auxw_load_uncached_x" 24 (eq_attr "type" "lsu_auxw_load_uncached_x") "k1c_lsu_auxw_x_r")
(define_insn_reservation "k1c_lsu_auxw_load_uncached_y" 24 (eq_attr "type" "lsu_auxw_load_uncached_y") "k1c_lsu_auxw_y_r")
(define_insn_reservation "k1c_lsu_load_uncached" 24 (eq_attr "type" "lsu_load_uncached") "k1c_lsu_r")
(define_insn_reservation "k1c_lsu_load_uncached_x" 24 (eq_attr "type" "lsu_load_uncached_x") "k1c_lsu_x_r")
(define_insn_reservation "k1c_lsu_load_uncached_y" 24 (eq_attr "type" "lsu_load_uncached_y") "k1c_lsu_y_r")
(define_insn_reservation "k1c_lsu_auxr_store" 1 (eq_attr "type" "lsu_auxr_store") "k1c_lsu_auxr_r")
(define_insn_reservation "k1c_lsu_auxr_store_x" 1 (eq_attr "type" "lsu_auxr_store_x") "k1c_lsu_auxr_x_r")
(define_insn_reservation "k1c_lsu_auxr_store_y" 1 (eq_attr "type" "lsu_auxr_store_y") "k1c_lsu_auxr_y_r")
(define_insn_reservation "k1c_lsu_crrp_store" 1 (eq_attr "type" "lsu_crrp_store") "k1c_lsu_crrp_r")
(define_insn_reservation "k1c_lsu_crrp_store_x" 1 (eq_attr "type" "lsu_crrp_store_x") "k1c_lsu_crrp_x_r")
(define_insn_reservation "k1c_lsu_crrp_store_y" 1 (eq_attr "type" "lsu_crrp_store_y") "k1c_lsu_crrp_y_r")
(define_insn_reservation "k1c_lsu_auxw_atomic" 24 (eq_attr "type" "lsu_auxw_atomic") "k1c_lsu_auxw_r")
(define_insn_reservation "k1c_lsu_auxw_atomic_x" 24 (eq_attr "type" "lsu_auxw_atomic_x") "k1c_lsu_auxw_x_r")
(define_insn_reservation "k1c_lsu_auxw_atomic_y" 24 (eq_attr "type" "lsu_auxw_atomic_y") "k1c_lsu_auxw_y_r")
(define_insn_reservation "k1c_lsu_auxr_auxw_atomic" 24 (eq_attr "type" "lsu_auxr_auxw_atomic") "k1c_lsu_auxr_auxw_r")
(define_insn_reservation "k1c_lsu_auxr_auxw_atomic_x" 24 (eq_attr "type" "lsu_auxr_auxw_atomic_x") "k1c_lsu_auxr_auxw_x_r")
(define_insn_reservation "k1c_lsu_auxr_auxw_atomic_y" 24 (eq_attr "type" "lsu_auxr_auxw_atomic_y") "k1c_lsu_auxr_auxw_y_r")
(define_insn_reservation "k1c_lsu_auxr_auxw" 3 (eq_attr "type" "lsu_auxr_auxw") "k1c_lsu_auxr_auxw_r")
(define_insn_reservation "k1c_mau" 2 (eq_attr "type" "mau") "k1c_mau_r, nothing")
(define_insn_reservation "k1c_mau_x" 2 (eq_attr "type" "mau_x") "k1c_mau_x_r")
(define_insn_reservation "k1c_mau_y" 2 (eq_attr "type" "mau_y") "k1c_mau_y_r")
(define_insn_reservation "k1c_mau_auxr" 2 (eq_attr "type" "mau_auxr") "k1c_mau_auxr_r")
(define_insn_reservation "k1c_mau_auxr_x" 2 (eq_attr "type" "mau_auxr_x") "k1c_mau_auxr_x_r")
(define_insn_reservation "k1c_mau_auxr_y" 2 (eq_attr "type" "mau_auxr_y") "k1c_mau_auxr_y_r")
(define_insn_reservation "k1c_mau_fpu" 4 (eq_attr "type" "mau_fpu") "k1c_mau_r")
(define_insn_reservation "k1c_mau_auxr_fpu" 4 (eq_attr "type" "mau_auxr_fpu") "k1c_auxr_u + k1c_mau_u + k1c_issue_r")
(define_insn_reservation "k1c_bcu" 1 (eq_attr "type" "bcu") "k1c_bcu_r")
(define_insn_reservation "k1c_bcu_get" 1 (eq_attr "type" "bcu_get") "k1c_bcu_tiny_tiny_mau_r")
(define_insn_reservation "k1c_bcu_tiny_auxw_crrp" 1 (eq_attr "type" "bcu_tiny_auxw_crrp") "k1c_bcu_tiny_auxw_crrp_r")
(define_insn_reservation "k1c_bcu_crrp_crwl_crwh" 1 (eq_attr "type" "bcu_crrp_crwl_crwh") "k1c_bcu_crrp_crwl_crwh_r")
(define_insn_reservation "k1c_tca" 1 (eq_attr "type" "tca") "k1c_tca_r")

/* The BCU reads GPRs 1 cycle earlier */
(define_bypass 2 "k1c_alu_full*,k1c_alu_lite*,k1c_alu_tiny*" "k1c_bcu,k1c_bcu_get")
(define_bypass 3 "k1c_mau,k1c_mau_x,k1c_mau_auxr" "k1c_bcu,k1c_bcu_get")
(define_bypass 5 "k1c_mau_fpu,k1c_mau_auxr_fpu" "k1c_bcu,k1c_bcu_get")
(define_bypass 4 "k1c_lsu_auxw_load,k1c_lsu_auxw_load_x" "k1c_bcu,k1c_bcu_get")
/* The stores read their input 1 cycle later */
(define_bypass 3 "k1c_mau_fpu,k1c_mau_auxr_fpu" "k1c_lsu_auxr_store,k1c_lsu_auxr_store_x,k1c_lsu_auxr_store_y")
(define_bypass 1 "k1c_mau,k1c_mau_x,k1c_mau_auxr" "k1c_lsu_auxr_store,k1c_lsu_auxr_store_x,k1c_lsu_auxr_store_y")
(define_bypass 2 "k1c_lsu_auxw_load,k1c_lsu_auxw_load_x" "k1c_lsu_auxr_store,k1c_lsu_auxr_store_x,k1c_lsu_auxr_store_y")
/* The auxr port shared by MAU and LSU allows a bypass when used as accumulator in a MAC operation. */
(define_bypass 1 "k1c_mau,k1c_mau_x" "k1c_mau_auxr" "k1_mau_lsu_double_port_bypass_p")

