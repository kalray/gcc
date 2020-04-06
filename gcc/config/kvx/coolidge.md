(automata_option "ndfa")
(automata_option "v")

(define_automaton "coolidge_exu")
(define_automaton "coolidge_issue")

(define_cpu_unit
  "kv3_issue0_u,
   kv3_issue1_u,
   kv3_issue2_u,
   kv3_issue3_u,
   kv3_issue4_u,
   kv3_issue5_u,
   kv3_issue6_u,
   kv3_issue7_u"
  "coolidge_issue")

(define_cpu_unit
  "kv3_tiny0_u,
   kv3_tiny1_u,
   kv3_tiny2_u,
   kv3_tiny3_u,
   kv3_lite0_u,
   kv3_lite1_u,
   kv3_full_u,
   kv3_lsu_u,
   kv3_mau_u,
   kv3_bcu_u,
   kv3_tca_u,
   kv3_auxr_u,
   kv3_auxw_u,
   kv3_crrp_u,
   kv3_crwl_u,
   kv3_crwh_u"
  "coolidge_exu")

(absence_set "kv3_tiny0_u" "kv3_lite0_u,kv3_full_u")
(absence_set "kv3_tiny1_u" "kv3_lite1_u")
(absence_set "kv3_tiny2_u" "kv3_mau_u")
(absence_set "kv3_tiny3_u" "kv3_lsu_u")
(absence_set "kv3_lite0_u" "kv3_tiny0_u, kv3_full_u")
(absence_set "kv3_lite1_u" "kv3_tiny1_u")
(absence_set "kv3_full_u"  "kv3_tiny0_u, kv3_lite0_u")
(absence_set "kv3_mau_u"   "kv3_tiny2_u")
(absence_set "kv3_lsu_u"   "kv3_tiny3_u")

(define_reservation "kv3_issue_r"
  "(kv3_issue0_u|kv3_issue1_u|kv3_issue2_u|kv3_issue3_u|kv3_issue4_u|kv3_issue5_u|kv3_issue6_u|kv3_issue7_u)")
(define_reservation "kv3_issue_x2_r"
  "(kv3_issue0_u+kv3_issue1_u)|(kv3_issue1_u+kv3_issue2_u)|(kv3_issue2_u+kv3_issue3_u)|(kv3_issue3_u+kv3_issue4_u)|(kv3_issue4_u+kv3_issue5_u)|(kv3_issue5_u+kv3_issue6_u)|(kv3_issue6_u+kv3_issue7_u)")
(define_reservation "kv3_issue_x3_r"
  "(kv3_issue0_u+kv3_issue1_u+kv3_issue2_u)|(kv3_issue1_u+kv3_issue2_u+kv3_issue3_u)|(kv3_issue2_u+kv3_issue3_u+kv3_issue4_u)|(kv3_issue3_u+kv3_issue4_u+kv3_issue5_u)|(kv3_issue4_u+kv3_issue5_u+kv3_issue6_u)|(kv3_issue5_u+kv3_issue6_u+kv3_issue7_u)")
(define_reservation "kv3_issue_x4_r"
  "(kv3_issue0_u+kv3_issue1_u+kv3_issue2_u+kv3_issue3_u)|(kv3_issue1_u+kv3_issue2_u+kv3_issue3_u+kv3_issue4_u)|(kv3_issue2_u+kv3_issue3_u+kv3_issue4_u+kv3_issue5_u)|(kv3_issue3_u+kv3_issue4_u+kv3_issue5_u+kv3_issue6_u)|(kv3_issue4_u+kv3_issue5_u+kv3_issue6_u+kv3_issue7_u)")
(define_reservation "kv3_issue_x6_r"
  "(kv3_issue0_u+kv3_issue1_u+kv3_issue2_u+kv3_issue3_u+kv3_issue4_u+kv3_issue5_u)|(kv3_issue1_u+kv3_issue2_u+kv3_issue3_u+kv3_issue4_u+kv3_issue5_u+kv3_issue6_u)|(kv3_issue2_u+kv3_issue3_u+kv3_issue4_u+kv3_issue5_u+kv3_issue6_u+kv3_issue7_u)")
(define_reservation "kv3_issue_x8_r"
  "(kv3_issue0_u+kv3_issue1_u+kv3_issue2_u+kv3_issue3_u+kv3_issue4_u+kv3_issue5_u+kv3_issue6_u+kv3_issue7_u)")

(define_reservation "kv3_tiny_r" "(kv3_tiny0_u | kv3_tiny1_u | kv3_tiny2_u | kv3_tiny3_u )")
(define_reservation "kv3_tiny_x2_r" "((kv3_tiny0_u+kv3_tiny1_u) | (kv3_tiny1_u+kv3_tiny2_u) | (kv3_tiny2_u+kv3_tiny3_u))")
(define_reservation "kv3_tiny_x4_r" "(kv3_tiny0_u+kv3_tiny1_u+kv3_tiny2_u+kv3_tiny3_u)")
(define_reservation "kv3_lite_r" "(kv3_lite0_u | kv3_lite1_u)")
(define_reservation "kv3_lite_x2_r" "(kv3_lite0_u+kv3_lite1_u)")
(define_reservation "kv3_full_r"  "(kv3_full_u)")

(define_reservation "kv3_all_r" "kv3_tiny_x4_r + kv3_bcu_u + kv3_tca_u + kv3_issue_x8_r")
(define_reservation "kv3_alu_tiny_r" "kv3_tiny_r + kv3_issue_r")
(define_reservation "kv3_alu_tiny_x_r" "kv3_tiny_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_tiny_y_r" "kv3_tiny_r + kv3_issue_x3_r")
(define_reservation "kv3_alu_tiny_x2_r" "kv3_tiny_x2_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_tiny_x2_x_r" "kv3_tiny_x2_r + kv3_issue_x4_r")
(define_reservation "kv3_alu_tiny_x2_y_r" "kv3_tiny_x2_r + kv3_issue_x6_r")
(define_reservation "kv3_alu_tiny_x4_r" "kv3_tiny_x4_r + kv3_issue_x4_r")
(define_reservation "kv3_alu_tiny_x4_x_r" "kv3_tiny_x4_r + kv3_issue_x8_r")
(define_reservation "kv3_alu_lite_r" "kv3_lite_r + kv3_issue_r")
(define_reservation "kv3_alu_lite_x_r" "kv3_lite_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_lite_y_r" "kv3_lite_r + kv3_issue_x3_r")
(define_reservation "kv3_alu_lite_x2_r" "kv3_lite_x2_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_lite_x2_x_r" "kv3_lite_x2_r + kv3_issue_x4_r")
(define_reservation "kv3_alu_full_r" "kv3_full_r + kv3_issue_r")
(define_reservation "kv3_alu_full_x_r" "kv3_full_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_full_y_r" "kv3_full_r + kv3_issue_x3_r")
(define_reservation "kv3_lsu_r" "kv3_lsu_u + kv3_tiny_r + kv3_issue_r")
(define_reservation "kv3_lsu_x_r" "kv3_lsu_u + kv3_tiny_r + kv3_issue_x2_r")
(define_reservation "kv3_lsu_y_r" "kv3_lsu_u + kv3_tiny_r + kv3_issue_x3_r")
(define_reservation "kv3_lsu_auxr_auxw_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxr_u + kv3_auxw_u + kv3_issue_r")
(define_reservation "kv3_lsu_auxr_auxw_x_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxr_u + kv3_auxw_u + kv3_issue_x2_r")
(define_reservation "kv3_lsu_auxr_auxw_y_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxr_u + kv3_auxw_u + kv3_issue_x3_r")
(define_reservation "kv3_lsu_auxw_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxw_u + kv3_issue_r")
(define_reservation "kv3_lsu_auxw_x_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxw_u + kv3_issue_x2_r")
(define_reservation "kv3_lsu_auxw_y_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxw_u + kv3_issue_x3_r")
(define_reservation "kv3_lsu_auxr_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxr_u + kv3_issue_r")
(define_reservation "kv3_lsu_auxr_x_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxr_u + kv3_issue_x2_r")
(define_reservation "kv3_lsu_auxr_y_r" "kv3_lsu_u + kv3_tiny_r + kv3_auxr_u + kv3_issue_x3_r")
(define_reservation "kv3_lsu_crrp_r" "kv3_lsu_u + kv3_tiny_r + kv3_crrp_u + kv3_issue_r")
(define_reservation "kv3_lsu_crrp_x_r" "kv3_lsu_u + kv3_tiny_r + kv3_crrp_u + kv3_issue_x2_r")
(define_reservation "kv3_lsu_crrp_y_r" "kv3_lsu_u + kv3_tiny_r + kv3_crrp_u + kv3_issue_x3_r")
(define_reservation "kv3_mau_r" "kv3_mau_u + kv3_issue_r")
(define_reservation "kv3_mau_x_r" "kv3_mau_u + kv3_issue_x2_r")
(define_reservation "kv3_mau_y_r" "kv3_mau_u + kv3_issue_x3_r")
(define_reservation "kv3_mau_auxr_r" "kv3_mau_u + kv3_auxr_u + kv3_issue_r")
(define_reservation "kv3_mau_auxr_x_r" "kv3_mau_u + kv3_auxr_u + kv3_issue_x2_r")
(define_reservation "kv3_mau_auxr_y_r" "kv3_mau_u + kv3_auxr_u + kv3_issue_x3_r")
(define_reservation "kv3_bcu_r" "kv3_bcu_u + kv3_issue_r")
(define_reservation "kv3_bcu_tiny_tiny_mau_r" "kv3_bcu_u + kv3_tiny_x2_r + kv3_mau_u + kv3_issue_r")
(define_reservation "kv3_bcu_tiny_auxw_crrp_r" "kv3_bcu_u + kv3_tiny_r + kv3_auxw_u + kv3_crrp_u + kv3_issue_r")
(define_reservation "kv3_bcu_crrp_crwl_crwh_r" "kv3_bcu_u + kv3_crrp_u + kv3_crwl_u + kv3_crwh_u + kv3_issue_r")
(define_reservation "kv3_tca_r" "kv3_tca_u + kv3_issue_r")

(define_insn_reservation "kv3_all" 1 (eq_attr "type" "all") "kv3_all_r")
(define_insn_reservation "kv3_alu_tiny" 1 (eq_attr "type" "alu_tiny") "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_tiny_x" 1 (eq_attr "type" "alu_tiny_x") "kv3_alu_tiny_x_r")
(define_insn_reservation "kv3_alu_tiny_y" 1 (eq_attr "type" "alu_tiny_y") "kv3_alu_tiny_y_r")
(define_insn_reservation "kv3_alu_tiny_x2" 1 (eq_attr "type" "alu_tiny_x2") "kv3_alu_tiny_x2_r")
(define_insn_reservation "kv3_alu_tiny_x2_x" 1 (eq_attr "type" "alu_tiny_x2_x") "kv3_alu_tiny_x2_x_r")
(define_insn_reservation "kv3_alu_tiny_x2_y" 1 (eq_attr "type" "alu_tiny_x2_y") "kv3_alu_tiny_x2_y_r")
(define_insn_reservation "kv3_alu_tiny_x4" 1 (eq_attr "type" "alu_tiny_x4") "kv3_alu_tiny_x4_r")
(define_insn_reservation "kv3_alu_tiny_x4_x" 1 (eq_attr "type" "alu_tiny_x4_x") "kv3_alu_tiny_x4_x_r")
(define_insn_reservation "kv3_alu_lite" 1 (eq_attr "type" "alu_lite") "kv3_alu_lite_r")
(define_insn_reservation "kv3_alu_lite_x" 1 (eq_attr "type" "alu_lite_x") "kv3_alu_lite_x_r")
(define_insn_reservation "kv3_alu_lite_y" 1 (eq_attr "type" "alu_lite_y") "kv3_alu_lite_y_r")
(define_insn_reservation "kv3_alu_lite_x2" 1 (eq_attr "type" "alu_lite_x2") "kv3_alu_lite_x2_r")
(define_insn_reservation "kv3_alu_lite_x2_x" 1 (eq_attr "type" "alu_lite_x2_x") "kv3_alu_lite_x2_x_r")
(define_insn_reservation "kv3_alu_full" 1 (eq_attr "type" "alu_full") "kv3_alu_full_r")
(define_insn_reservation "kv3_alu_full_x" 1 (eq_attr "type" "alu_full_x") "kv3_alu_full_x_r")
(define_insn_reservation "kv3_alu_full_y" 1 (eq_attr "type" "alu_full_y") "kv3_alu_full_y_r")
(define_insn_reservation "kv3_alu_full_copro" 15 (eq_attr "type" "alu_full_copro") "kv3_alu_full_r")
(define_insn_reservation "kv3_lsu" 1 (eq_attr "type" "lsu") "kv3_lsu_r")
(define_insn_reservation "kv3_lsu_x" 1 (eq_attr "type" "lsu_x") "kv3_lsu_x_r")
(define_insn_reservation "kv3_lsu_y" 1 (eq_attr "type" "lsu_y") "kv3_lsu_y_r")
(define_insn_reservation "kv3_lsu_auxw_load" 3 (eq_attr "type" "lsu_auxw_load") "kv3_lsu_auxw_r")
(define_insn_reservation "kv3_lsu_auxw_load_x" 3 (eq_attr "type" "lsu_auxw_load_x") "kv3_lsu_auxw_x_r")
(define_insn_reservation "kv3_lsu_auxw_load_y" 3 (eq_attr "type" "lsu_auxw_load_y") "kv3_lsu_auxw_y_r")
(define_insn_reservation "kv3_lsu_auxw_load_uncached" 24 (eq_attr "type" "lsu_auxw_load_uncached") "kv3_lsu_auxw_r")
(define_insn_reservation "kv3_lsu_auxw_load_uncached_x" 24 (eq_attr "type" "lsu_auxw_load_uncached_x") "kv3_lsu_auxw_x_r")
(define_insn_reservation "kv3_lsu_auxw_load_uncached_y" 24 (eq_attr "type" "lsu_auxw_load_uncached_y") "kv3_lsu_auxw_y_r")
(define_insn_reservation "kv3_lsu_load_uncached" 24 (eq_attr "type" "lsu_load_uncached") "kv3_lsu_r")
(define_insn_reservation "kv3_lsu_load_uncached_x" 24 (eq_attr "type" "lsu_load_uncached_x") "kv3_lsu_x_r")
(define_insn_reservation "kv3_lsu_load_uncached_y" 24 (eq_attr "type" "lsu_load_uncached_y") "kv3_lsu_y_r")
(define_insn_reservation "kv3_lsu_auxr_store" 1 (eq_attr "type" "lsu_auxr_store") "kv3_lsu_auxr_r")
(define_insn_reservation "kv3_lsu_auxr_store_x" 1 (eq_attr "type" "lsu_auxr_store_x") "kv3_lsu_auxr_x_r")
(define_insn_reservation "kv3_lsu_auxr_store_y" 1 (eq_attr "type" "lsu_auxr_store_y") "kv3_lsu_auxr_y_r")
(define_insn_reservation "kv3_lsu_crrp_store" 1 (eq_attr "type" "lsu_crrp_store") "kv3_lsu_crrp_r")
(define_insn_reservation "kv3_lsu_crrp_store_x" 1 (eq_attr "type" "lsu_crrp_store_x") "kv3_lsu_crrp_x_r")
(define_insn_reservation "kv3_lsu_crrp_store_y" 1 (eq_attr "type" "lsu_crrp_store_y") "kv3_lsu_crrp_y_r")
(define_insn_reservation "kv3_lsu_auxw_atomic" 24 (eq_attr "type" "lsu_auxw_atomic") "kv3_lsu_auxw_r")
(define_insn_reservation "kv3_lsu_auxw_atomic_x" 24 (eq_attr "type" "lsu_auxw_atomic_x") "kv3_lsu_auxw_x_r")
(define_insn_reservation "kv3_lsu_auxw_atomic_y" 24 (eq_attr "type" "lsu_auxw_atomic_y") "kv3_lsu_auxw_y_r")
(define_insn_reservation "kv3_lsu_auxr_auxw_atomic" 24 (eq_attr "type" "lsu_auxr_auxw_atomic") "kv3_lsu_auxr_auxw_r")
(define_insn_reservation "kv3_lsu_auxr_auxw_atomic_x" 24 (eq_attr "type" "lsu_auxr_auxw_atomic_x") "kv3_lsu_auxr_auxw_x_r")
(define_insn_reservation "kv3_lsu_auxr_auxw_atomic_y" 24 (eq_attr "type" "lsu_auxr_auxw_atomic_y") "kv3_lsu_auxr_auxw_y_r")
(define_insn_reservation "kv3_lsu_auxr_auxw" 3 (eq_attr "type" "lsu_auxr_auxw") "kv3_lsu_auxr_auxw_r")
(define_insn_reservation "kv3_mau" 2 (eq_attr "type" "mau") "kv3_mau_r, nothing")
(define_insn_reservation "kv3_mau_x" 2 (eq_attr "type" "mau_x") "kv3_mau_x_r")
(define_insn_reservation "kv3_mau_y" 2 (eq_attr "type" "mau_y") "kv3_mau_y_r")
(define_insn_reservation "kv3_mau_auxr" 2 (eq_attr "type" "mau_auxr") "kv3_mau_auxr_r")
(define_insn_reservation "kv3_mau_auxr_x" 2 (eq_attr "type" "mau_auxr_x") "kv3_mau_auxr_x_r")
(define_insn_reservation "kv3_mau_auxr_y" 2 (eq_attr "type" "mau_auxr_y") "kv3_mau_auxr_y_r")
(define_insn_reservation "kv3_mau_fpu" 4 (eq_attr "type" "mau_fpu") "kv3_mau_r")
(define_insn_reservation "kv3_mau_auxr_fpu" 4 (eq_attr "type" "mau_auxr_fpu") "kv3_auxr_u + kv3_mau_u + kv3_issue_r")
(define_insn_reservation "kv3_bcu" 1 (eq_attr "type" "bcu") "kv3_bcu_r")
(define_insn_reservation "kv3_bcu_get" 1 (eq_attr "type" "bcu_get") "kv3_bcu_tiny_tiny_mau_r")
(define_insn_reservation "kv3_bcu_tiny_auxw_crrp" 1 (eq_attr "type" "bcu_tiny_auxw_crrp") "kv3_bcu_tiny_auxw_crrp_r")
(define_insn_reservation "kv3_bcu_crrp_crwl_crwh" 1 (eq_attr "type" "bcu_crrp_crwl_crwh") "kv3_bcu_crrp_crwl_crwh_r")
(define_insn_reservation "kv3_tca" 1 (eq_attr "type" "tca") "kv3_tca_r")

/* The BCU reads GPRs 1 cycle earlier */
(define_bypass 2 "kv3_alu_full*,kv3_alu_lite*,kv3_alu_tiny*" "kv3_bcu,kv3_bcu_get")
(define_bypass 3 "kv3_mau,kv3_mau_x,kv3_mau_auxr" "kv3_bcu,kv3_bcu_get")
(define_bypass 5 "kv3_mau_fpu,kv3_mau_auxr_fpu" "kv3_bcu,kv3_bcu_get")
(define_bypass 4 "kv3_lsu_auxw_load,kv3_lsu_auxw_load_x" "kv3_bcu,kv3_bcu_get")
/* The stores read their input 1 cycle later */
(define_bypass 3 "kv3_mau_fpu,kv3_mau_auxr_fpu" "kv3_lsu_auxr_store,kv3_lsu_auxr_store_x,kv3_lsu_auxr_store_y")
(define_bypass 1 "kv3_mau,kv3_mau_x,kv3_mau_auxr" "kv3_lsu_auxr_store,kv3_lsu_auxr_store_x,kv3_lsu_auxr_store_y")
(define_bypass 2 "kv3_lsu_auxw_load,kv3_lsu_auxw_load_x" "kv3_lsu_auxr_store,kv3_lsu_auxr_store_x,kv3_lsu_auxr_store_y")
/* The auxr port shared by MAU and LSU allows a bypass when used as accumulator in a MAC operation. */
(define_bypass 1 "kv3_mau,kv3_mau_x" "kv3_mau_auxr" "kv3_mau_lsu_double_port_bypass_p")

