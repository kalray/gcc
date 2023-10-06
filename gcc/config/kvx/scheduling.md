(automata_option "ndfa")
(automata_option "v")

(define_automaton "kv3_exu")
(define_automaton "kv3_issue")

(define_cpu_unit
  "kv3_issue0_u,
   kv3_issue1_u,
   kv3_issue2_u,
   kv3_issue3_u,
   kv3_issue4_u,
   kv3_issue5_u,
   kv3_issue6_u,
   kv3_issue7_u"
  "kv3_issue")

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
  "kv3_exu")

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
(define_reservation "kv3_alu_nop_r" "kv3_issue_r")
;;
(define_reservation "kv3_alu_tiny_r" "kv3_tiny_r + kv3_issue_r")
(define_reservation "kv3_alu_tiny_crrp_r" "kv3_tiny_r + kv3_crrp_u + kv3_issue_r")
(define_reservation "kv3_alu_tiny_crwl_crwh_r" "kv3_tiny_r + kv3_crwl_u + kv3_crwh_u + kv3_issue_r")
(define_reservation "kv3_alu_tiny_crrp_crwl_crwh_r" "kv3_tiny_r + kv3_crrp_u + kv3_crwl_u + kv3_crwh_u + kv3_issue_r")
(define_reservation "kv3_alu_tiny_x_r" "kv3_tiny_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_tiny_y_r" "kv3_tiny_r + kv3_issue_x3_r")
(define_reservation "kv3_alu_tiny_x2_r" "kv3_tiny_x2_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_tiny_x2_x_r" "kv3_tiny_x2_r + kv3_issue_x4_r")
(define_reservation "kv3_alu_tiny_x2_y_r" "kv3_tiny_x2_r + kv3_issue_x6_r")
(define_reservation "kv3_alu_tiny_x2_crwl_crwh_r" "kv3_tiny_x2_r + kv3_crwl_u + kv3_crwh_u + kv3_issue_x4_r")
(define_reservation "kv3_alu_tiny_x4_r" "kv3_tiny_x4_r + kv3_issue_x4_r")
(define_reservation "kv3_alu_tiny_x4_x_r" "kv3_tiny_x4_r + kv3_issue_x8_r")
;;
(define_reservation "kv3_alu_lite_r" "kv3_lite_r + kv3_issue_r")
(define_reservation "kv3_alu_lite_x_r" "kv3_lite_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_lite_y_r" "kv3_lite_r + kv3_issue_x3_r")
(define_reservation "kv3_alu_lite_x2_r" "kv3_lite_x2_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_lite_x2_x_r" "kv3_lite_x2_r + kv3_issue_x4_r")
(define_reservation "kv3_alu_lite_x2_crwl_crwh_r" "kv3_lite_x2_r + kv3_crwl_u + kv3_crwh_u + kv3_issue_x4_r")
;;
(define_reservation "kv3_alu_full_r" "kv3_full_r + kv3_issue_r")
(define_reservation "kv3_alu_full_x_r" "kv3_full_r + kv3_issue_x2_r")
(define_reservation "kv3_alu_full_y_r" "kv3_full_r + kv3_issue_x3_r")
;;
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
;;
(define_reservation "kv3_mau_r" "kv3_mau_u + kv3_issue_r")
(define_reservation "kv3_mau_x_r" "kv3_mau_u + kv3_issue_x2_r")
(define_reservation "kv3_mau_y_r" "kv3_mau_u + kv3_issue_x3_r")
(define_reservation "kv3_mau_auxr_r" "kv3_mau_u + kv3_auxr_u + kv3_issue_r")
(define_reservation "kv3_mau_mul2_auxr_x_r" "kv3_mau_u + kv3_auxr_u + kv3_issue_x2_r")
(define_reservation "kv3_mau_mul2_auxr_y_r" "kv3_mau_u + kv3_auxr_u + kv3_issue_x3_r")
;;
(define_reservation "kv3_bcu_r" "kv3_bcu_u + kv3_issue_r")
(define_reservation "kv3_bcu_tiny_tiny_mau_r" "kv3_bcu_u + kv3_tiny_x2_r + kv3_mau_u + kv3_issue_r")
(define_reservation "kv3_bcu_tiny_auxw_crrp_r" "kv3_bcu_u + kv3_tiny_r + kv3_auxw_u + kv3_crrp_u + kv3_issue_r")
(define_reservation "kv3_bcu_crrp_crwl_crwh_r" "kv3_bcu_u + kv3_crrp_u + kv3_crwl_u + kv3_crwh_u + kv3_issue_r")
;;
(define_reservation "kv3_tca_r" "kv3_tca_u + kv3_issue_r")

;; Instruction reservations
(define_insn_reservation "kv3_all" 1 (eq_attr "type" "all") "kv3_all_r")
(define_insn_reservation "kv3_alu_nop" 1 (eq_attr "type" "alu_nop") "kv3_alu_nop_r")
;;
(define_insn_reservation "kv3_alu_tiny" 1 (eq_attr "type" "alu_tiny") "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_tiny_crrp" 1 (eq_attr "type" "alu_tiny_crrp") "kv3_alu_tiny_crrp_r")
(define_insn_reservation "kv3_alu_tiny_crwl_crwh" 1 (eq_attr "type" "alu_tiny_crwl_crwh") "kv3_alu_tiny_crwl_crwh_r")
(define_insn_reservation "kv3_alu_tiny_crrp_crwl_crwh" 1 (eq_attr "type" "alu_tiny_crrp_crwl_crwh") "kv3_alu_tiny_crrp_crwl_crwh_r")
(define_insn_reservation "kv3_alu_tiny_use" 0 (eq_attr "type" "alu_tiny_use") "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_tiny_w" 1 (and (eq_attr "type" "alu_tiny_w") (match_test "KV3")) "kv3_alu_tiny_r")
(define_insn_reservation "kv4_alu_tiny_w" 1 (and (eq_attr "type" "alu_tiny_w") (match_test "KV4")) "kv3_alu_tiny_x_r")
(define_insn_reservation "kv3_alu_tiny_x" 1 (eq_attr "type" "alu_tiny_x") "kv3_alu_tiny_x_r")
(define_insn_reservation "kv3_alu_tiny_y" 1 (eq_attr "type" "alu_tiny_y") "kv3_alu_tiny_y_r")
(define_insn_reservation "kv3_alu_tiny_x2" 1 (eq_attr "type" "alu_tiny_x2") "kv3_alu_tiny_x2_r")
(define_insn_reservation "kv3_alu_tiny_use_x2" 0 (eq_attr "type" "alu_tiny_use_x2") "kv3_alu_tiny_x2_r")
(define_insn_reservation "kv3_alu_tiny_x2_x" 1 (eq_attr "type" "alu_tiny_x2_x") "kv3_alu_tiny_x2_x_r")
(define_insn_reservation "kv3_alu_tiny_x2_y" 1 (eq_attr "type" "alu_tiny_x2_y") "kv3_alu_tiny_x2_y_r")
(define_insn_reservation "kv3_alu_tiny_x4" 1 (eq_attr "type" "alu_tiny_x4") "kv3_alu_tiny_x4_r")
(define_insn_reservation "kv3_alu_tiny_x4_x" 1 (eq_attr "type" "alu_tiny_x4_x") "kv3_alu_tiny_x4_x_r")
;;
(define_insn_reservation "kv3_alu_thin_1" 1 (and (eq_attr "type" "alu_thin") (match_test "KV3_1")) "kv3_alu_lite_r")
(define_insn_reservation "kv3_alu_thin_w_1" 1 (and (eq_attr "type" "alu_thin_w") (match_test "KV3_1")) "kv3_alu_lite_r")
(define_insn_reservation "kv3_alu_thin_x_1" 1 (and (eq_attr "type" "alu_thin_x") (match_test "KV3_1")) "kv3_alu_lite_x_r")
(define_insn_reservation "kv3_alu_thin_y_1" 1 (and (eq_attr "type" "alu_thin_y") (match_test "KV3_1")) "kv3_alu_lite_y_r")
(define_insn_reservation "kv3_alu_thin_x2_1" 1 (and (eq_attr "type" "alu_thin_x2") (match_test "KV3_1")) "kv3_alu_lite_x2_r")
(define_insn_reservation "kv3_alu_thin_x2_x_1" 1 (and (eq_attr "type" "alu_thin_x2_x") (match_test "KV3_1")) "kv3_alu_lite_x2_x_r")
(define_insn_reservation "kv3_alu_thin_x2_crwl_crwh_1" 1 (and (eq_attr "type" "alu_thin_x2_crwl_crwh") (match_test "KV3_1")) "kv3_alu_lite_x2_crwl_crwh_r")
(define_insn_reservation "kv3_alu_thin_2" 1 (and (eq_attr "type" "alu_thin") (match_test "(KV3_2||KV4)")) "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_thin_w_2" 1 (and (eq_attr "type" "alu_thin_w") (match_test "KV3_2")) "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_thin_x_2" 1 (and (eq_attr "type" "alu_thin_x") (match_test "(KV3_2||KV4)")) "kv3_alu_tiny_x_r")
(define_insn_reservation "kv3_alu_thin_y_2" 1 (and (eq_attr "type" "alu_thin_y") (match_test "(KV3_2||KV4)")) "kv3_alu_tiny_y_r")
(define_insn_reservation "kv3_alu_thin_x2_2" 1 (and (eq_attr "type" "alu_thin_x2") (match_test "(KV3_2||KV4)")) "kv3_alu_tiny_x2_r")
(define_insn_reservation "kv3_alu_thin_x2_x_2" 1 (and (eq_attr "type" "alu_thin_x2_x") (match_test "(KV3_2||KV4)")) "kv3_alu_tiny_x2_x_r")
(define_insn_reservation "kv3_alu_thin_x2_crwl_crwh_2" 1 (and (eq_attr "type" "alu_thin_x2_crwl_crwh") (match_test "(KV3_2||KV4)")) "kv3_alu_tiny_x2_crwl_crwh_r")
(define_insn_reservation "kv4_alu_thin_w_1" 1 (and (eq_attr "type" "alu_thin_w") (match_test "KV4")) "kv3_alu_tiny_x_r")
;;
(define_insn_reservation "kv3_alu_lite" 1 (eq_attr "type" "alu_lite") "kv3_alu_lite_r")
(define_insn_reservation "kv3_alu_lite_x" 1 (eq_attr "type" "alu_lite_x") "kv3_alu_lite_x_r")
(define_insn_reservation "kv3_alu_lite_y" 1 (eq_attr "type" "alu_lite_y") "kv3_alu_lite_y_r")
(define_insn_reservation "kv3_alu_lite_x2" 1 (eq_attr "type" "alu_lite_x2") "kv3_alu_lite_x2_r")
(define_insn_reservation "kv3_alu_lite_x2_x" 1 (eq_attr "type" "alu_lite_x2_x") "kv3_alu_lite_x2_x_r")
(define_insn_reservation "kv3_alu_lite_x2_crwl_crwh" 1 (eq_attr "type" "alu_lite_x2_crwl_crwh") "kv3_alu_lite_x2_crwl_crwh_r")
;;
(define_insn_reservation "kv3_alu_full" 1 (eq_attr "type" "alu_full") "kv3_alu_full_r")
(define_insn_reservation "kv3_alu_full_x" 1 (eq_attr "type" "alu_full_x") "kv3_alu_full_x_r")
(define_insn_reservation "kv3_alu_full_y" 1 (eq_attr "type" "alu_full_y") "kv3_alu_full_y_r")
(define_insn_reservation "kv3_alu_full_sfu" 15 (eq_attr "type" "alu_full_sfu") "kv3_alu_full_r")
;;
(define_insn_reservation "kv3_lsu" 1 (eq_attr "type" "lsu") "kv3_lsu_r")
(define_insn_reservation "kv3_lsu_x" 1 (eq_attr "type" "lsu_x") "kv3_lsu_x_r")
(define_insn_reservation "kv3_lsu_y" 1 (eq_attr "type" "lsu_y") "kv3_lsu_y_r")
(define_insn_reservation "kv3_lsu_load" 3 (eq_attr "type" "lsu_load") "kv3_lsu_r")
(define_insn_reservation "kv3_lsu_load_x" 3 (eq_attr "type" "lsu_load_x") "kv3_lsu_x_r")
(define_insn_reservation "kv3_lsu_load_y" 3 (eq_attr "type" "lsu_load_y") "kv3_lsu_y_r")
(define_insn_reservation "kv3_lsu_auxw_load" 3 (eq_attr "type" "lsu_auxw_load") "kv3_lsu_auxw_r")
(define_insn_reservation "kv3_lsu_auxw_load_x" 3 (eq_attr "type" "lsu_auxw_load_x") "kv3_lsu_auxw_x_r")
(define_insn_reservation "kv3_lsu_auxw_load_y" 3 (eq_attr "type" "lsu_auxw_load_y") "kv3_lsu_auxw_y_r")
(define_insn_reservation "kv3_lsu_auxw_uncached_load" 24 (eq_attr "type" "lsu_auxw_uncached_load") "kv3_lsu_auxw_r")
(define_insn_reservation "kv3_lsu_auxw_uncached_load_x" 24 (eq_attr "type" "lsu_auxw_uncached_load_x") "kv3_lsu_auxw_x_r")
(define_insn_reservation "kv3_lsu_auxw_uncached_load_y" 24 (eq_attr "type" "lsu_auxw_uncached_load_y") "kv3_lsu_auxw_y_r")
(define_insn_reservation "kv3_lsu_uncached_load" 24 (eq_attr "type" "lsu_uncached_load") "kv3_lsu_r")
(define_insn_reservation "kv3_lsu_uncached_load_x" 24 (eq_attr "type" "lsu_uncached_load_x") "kv3_lsu_x_r")
(define_insn_reservation "kv3_lsu_uncached_load_y" 24 (eq_attr "type" "lsu_uncached_load_y") "kv3_lsu_y_r")
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
;;
(define_insn_reservation "kv3_mau_mul2" 2 (and (eq_attr "type" "alu_mul2") (match_test "KV3")) "kv3_mau_r")
(define_insn_reservation "kv3_mau_mul2_x" 2 (and (eq_attr "type" "alu_mul2_x") (match_test "KV3")) "kv3_mau_x_r")
(define_insn_reservation "kv3_mau_mul2_y" 2 (and (eq_attr "type" "alu_mul2_y") (match_test "KV3")) "kv3_mau_y_r")
(define_insn_reservation "kv3_mau_mul2_auxr" 2 (and (eq_attr "type" "alu_mac2") (match_test "KV3")) "kv3_mau_auxr_r")
(define_insn_reservation "kv3_mau_mul2_auxr_x" 2 (and (eq_attr "type" "alu_mac2_x") (match_test "KV3")) "kv3_mau_mul2_auxr_x_r")
(define_insn_reservation "kv3_mau_mul2_auxr_y" 2 (and (eq_attr "type" "alu_mac2_y") (match_test "KV3")) "kv3_mau_mul2_auxr_y_r")
(define_insn_reservation "kv3_mau_mul3" 3 (eq_attr "type" "fpu_mul3") "kv3_mau_r")
(define_insn_reservation "kv3_mau_mul3_auxr" 3 (eq_attr "type" "fpu_fma3") "kv3_auxr_u + kv3_mau_u + kv3_issue_r")
(define_insn_reservation "kv3_mau_mul4" 4 (eq_attr "type" "fpu_mul4") "kv3_mau_r")
(define_insn_reservation "kv3_mau_mul4_auxr" 4 (eq_attr "type" "fpu_fma4") "kv3_auxr_u + kv3_mau_u + kv3_issue_r")
(define_insn_reservation "kv4_alu_mul2" 2 (and (eq_attr "type" "alu_mul2") (match_test "KV4")) "kv3_alu_lite_r")
(define_insn_reservation "kv4_alu_mul2_x" 2 (and (eq_attr "type" "alu_mul2_x") (match_test "KV4")) "kv3_alu_lite_x_r")
(define_insn_reservation "kv4_alu_mul2_auxr" 2 (and (eq_attr "type" "alu_mac2") (match_test "KV4")) "kv3_alu_lite_r")
(define_insn_reservation "kv4_alu_mul2_auxr_x" 2 (and (eq_attr "type" "alu_mac2_x") (match_test "KV4")) "kv3_alu_lite_x_r")
;;
(define_insn_reservation "kv3_bcu" 1 (eq_attr "type" "bcu") "kv3_bcu_r")
(define_insn_reservation "kv3_bcu_get" 1 (eq_attr "type" "bcu_get") "kv3_bcu_tiny_tiny_mau_r")
(define_insn_reservation "kv3_bcu_tiny_auxw_crrp" 1 (eq_attr "type" "bcu_tiny_auxw_crrp") "kv3_bcu_tiny_auxw_crrp_r")
(define_insn_reservation "kv3_bcu_crrp_crwl_crwh" 1 (eq_attr "type" "bcu_crrp_crwl_crwh") "kv3_bcu_crrp_crwl_crwh_r")
;;
(define_insn_reservation "kv3_tca" 1 (eq_attr "type" "tca") "kv3_tca_r")
(define_insn_reservation "kv3_tca_int" 3 (eq_attr "type" "tca_int") "kv3_tca_r")
(define_insn_reservation "kv3_tca_float" 4 (eq_attr "type" "tca_float") "kv3_tca_r")

;; The BCU reads GPRs one cycle earlier than arithmetic instructions.
(define_bypass 2 "kv3_alu_full*,kv3_alu_lite*,kv3_alu_thin*,kv3_alu_tiny*"
                 "kv3_bcu*")
(define_bypass 3 "kv3_mau_mul2*"
                 "kv3_bcu*")
(define_bypass 4 "kv3_lsu_auxw_load*"
                 "kv3_bcu*")
(define_bypass 4 "kv3_mau_mul3*"
                 "kv3_bcu*")
(define_bypass 5 "kv3_mau_mul4*"
                 "kv3_bcu*")
;; The stores read their input one cycle later than arithmetic instructions.
(define_bypass 1 "kv3_mau_mul2*"
                 "kv3_lsu_auxr_store*"
                 "kvx_stored_value_bypass_p")
(define_bypass 2 "kv3_mau_mul3*"
                 "kv3_lsu_auxr_store*"
                 "kvx_stored_value_bypass_p")
(define_bypass 3 "kv3_mau_mul4*"
                 "kv3_lsu_auxr_store*"
                 "kvx_stored_value_bypass_p")
(define_bypass 2 "kv3_lsu_auxw_load*"
                 "kv3_lsu_auxr_store*"
                 "kvx_stored_value_bypass_p")
(define_bypass 1 "kv4_alu_mul2*"
                 "kv3_lsu_auxr_store*"
                 "kvx_stored_value_bypass_p")
;; The auxr port shared by MAU and LSU have a bypass to the accumulator in a MAC operation.
(define_bypass 1 "kv3_mau_mul2*"
                 "kv3_mau_mul2_auxr*"
                 "kvx_accumulator_bypass_p")
(define_bypass 2 "kv3_lsu_auxw_load*"
                 "kv3_mau_mul2_auxr*"
                 "kvx_accumulator_bypass_p")
(define_bypass 1 "kv4_alu_mul2*"
                 "kv4_alu_mul2_auxr*"
                 "kvx_accumulator_bypass_p")
(define_bypass 2 "kv3_lsu_auxw_load*"
                 "kv4_alu_mul2_auxr*"
                 "kvx_accumulator_bypass_p")
;; The coprocessor has a bypass between the BILAU accumulations.
(define_bypass 1 "kv3_tca_int"
                 "kv3_tca_int"
                 "kvx_accumulator_bypass_p")
;; The coprocessor does not have a bypass from any E1 source to the BILAU accumulators.
(define_bypass 3 "kv3_bcu*"
                 "kv3_tca_int"
                 "kvx_accumulator_bypass_p")
(define_bypass 3 "kv3_alu*"
                 "kv3_tca_int"
                 "kvx_accumulator_bypass_p")
;; The coprocessor does not have a bypass from any E1 source to the BLAU accumulators.
(define_bypass 4 "kv3_tca"
                 "kv3_tca_float"
                 "kvx_accumulator_bypass_p")
(define_bypass 4 "kv3_alu*"
                 "kv3_tca_float"
                 "kvx_accumulator_bypass_p")

