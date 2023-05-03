(automata_option "ndfa")
(automata_option "v")

(define_automaton "kvx_exu")
(define_automaton "kvx_issue")

(define_cpu_unit
  "kvx_issue0_u,
   kvx_issue1_u,
   kvx_issue2_u,
   kvx_issue3_u,
   kvx_issue4_u,
   kvx_issue5_u,
   kvx_issue6_u,
   kvx_issue7_u"
  "kvx_issue")

(define_cpu_unit
  "kvx_tiny0_u,
   kvx_tiny1_u,
   kvx_tiny2_u,
   kvx_tiny3_u,
   kvx_lite0_u,
   kvx_lite1_u,
   kvx_full_u,
   kvx_lsu0_u,
   kvx_lsu1_u,
   kvx_memw_u,
   kvx_mau0_u,
   kvx_mau1_u,
   kvx_bcu0_u,
   kvx_bcu1_u,
   kvx_xfer_u,
   kvx_tca_u,
   kvx_auxr_u,
   kvx_auxw_u,
   kvx_crrp_u,
   kvx_crwl_u,
   kvx_crwh_u"
  "kvx_exu")

(absence_set "kvx_tiny0_u" "kvx_lite0_u,kvx_full_u")
(absence_set "kvx_tiny1_u" "kvx_lite1_u")
(absence_set "kvx_lite0_u" "kvx_tiny0_u, kvx_full_u")
(absence_set "kvx_lite1_u" "kvx_tiny1_u")
(absence_set "kvx_full_u"  "kvx_tiny0_u, kvx_lite0_u")

(define_reservation "kvx_issue_r"
  "(kvx_issue0_u|kvx_issue1_u|kvx_issue2_u|kvx_issue3_u|kvx_issue4_u|kvx_issue5_u|kvx_issue6_u|kvx_issue7_u)")
(define_reservation "kvx_issue_x2_r"
  "(kvx_issue0_u+kvx_issue1_u)|(kvx_issue1_u+kvx_issue2_u)|(kvx_issue2_u+kvx_issue3_u)|(kvx_issue3_u+kvx_issue4_u)|(kvx_issue4_u+kvx_issue5_u)|(kvx_issue5_u+kvx_issue6_u)|(kvx_issue6_u+kvx_issue7_u)")
(define_reservation "kvx_issue_x3_r"
  "(kvx_issue0_u+kvx_issue1_u+kvx_issue2_u)|(kvx_issue1_u+kvx_issue2_u+kvx_issue3_u)|(kvx_issue2_u+kvx_issue3_u+kvx_issue4_u)|(kvx_issue3_u+kvx_issue4_u+kvx_issue5_u)|(kvx_issue4_u+kvx_issue5_u+kvx_issue6_u)|(kvx_issue5_u+kvx_issue6_u+kvx_issue7_u)")
(define_reservation "kvx_issue_x4_r"
  "(kvx_issue0_u+kvx_issue1_u+kvx_issue2_u+kvx_issue3_u)|(kvx_issue1_u+kvx_issue2_u+kvx_issue3_u+kvx_issue4_u)|(kvx_issue2_u+kvx_issue3_u+kvx_issue4_u+kvx_issue5_u)|(kvx_issue3_u+kvx_issue4_u+kvx_issue5_u+kvx_issue6_u)|(kvx_issue4_u+kvx_issue5_u+kvx_issue6_u+kvx_issue7_u)")
(define_reservation "kvx_issue_x6_r"
  "(kvx_issue0_u+kvx_issue1_u+kvx_issue2_u+kvx_issue3_u+kvx_issue4_u+kvx_issue5_u)|(kvx_issue1_u+kvx_issue2_u+kvx_issue3_u+kvx_issue4_u+kvx_issue5_u+kvx_issue6_u)|(kvx_issue2_u+kvx_issue3_u+kvx_issue4_u+kvx_issue5_u+kvx_issue6_u+kvx_issue7_u)")
(define_reservation "kvx_issue_x8_r"
  "(kvx_issue0_u+kvx_issue1_u+kvx_issue2_u+kvx_issue3_u+kvx_issue4_u+kvx_issue5_u+kvx_issue6_u+kvx_issue7_u)")

(define_reservation "kvx_tiny_r" "(kvx_tiny0_u | kvx_tiny1_u | kvx_tiny2_u | kvx_tiny3_u )")
(define_reservation "kvx_tiny_x2_r" "((kvx_tiny0_u+kvx_tiny1_u) | (kvx_tiny1_u+kvx_tiny2_u) | (kvx_tiny2_u+kvx_tiny3_u))")
(define_reservation "kvx_tiny_x4_r" "(kvx_tiny0_u+kvx_tiny1_u+kvx_tiny2_u+kvx_tiny3_u)")
(define_reservation "kvx_lite_r" "(kvx_lite0_u | kvx_lite1_u)")
(define_reservation "kvx_lite_x2_r" "(kvx_lite0_u+kvx_lite1_u)")

(define_reservation "kv3_mau0_r" "(kvx_mau0_u + kvx_tiny2_u)")
(define_reservation "kv3_lsu0_r" "(kvx_lsu0_u + kvx_tiny3_u)")

(define_reservation "kv4_mau0_r" "kvx_mau0_u")
(define_reservation "kv4_mau1_r" "kvx_mau1_u")
(define_reservation "kv4_lsu0_r" "(kvx_lsu0_u + kvx_tiny2_u)")
(define_reservation "kv4_lsu1_r" "(kvx_lsu1_u + kvx_tiny3_u)")

(define_reservation "kv3_alu_tiny_r" "kvx_tiny_r + kvx_issue_r")
(define_reservation "kv3_alu_tiny_send_r" "kvx_tiny_r + kvx_crrp_u + kvx_issue_r")
(define_reservation "kv3_alu_tiny_recv_r" "kvx_tiny_r + kvx_crwl_u + kvx_crwh_u + kvx_issue_r")
(define_reservation "kv3_alu_tiny_sendrecv_r" "kvx_tiny_r + kvx_crrp_u + kvx_crwl_u + kvx_crwh_u + kvx_issue_r")
(define_reservation "kv3_alu_tiny_x_r" "kvx_tiny_r + kvx_issue_x2_r")
(define_reservation "kv3_alu_tiny_y_r" "kvx_tiny_r + kvx_issue_x3_r")
(define_reservation "kv3_alu_tiny_x2_r" "kvx_tiny_x2_r + kvx_issue_x2_r")
(define_reservation "kv3_alu_tiny_x2_x_r" "kvx_tiny_x2_r + kvx_issue_x4_r")
(define_reservation "kv3_alu_tiny_x2_y_r" "kvx_tiny_x2_r + kvx_issue_x6_r")
(define_reservation "kv3_alu_tiny_x2_crwl_crwh_r" "kvx_tiny_x2_r + kvx_crwl_u + kvx_crwh_u + kvx_issue_x4_r")
(define_reservation "kv3_alu_tiny_x4_r" "kvx_tiny_x4_r + kvx_issue_x4_r")
(define_reservation "kv3_alu_tiny_x4_x_r" "kvx_tiny_x4_r + kvx_issue_x8_r")
(define_reservation "kv3_alu_lite_r" "kvx_lite_r + kvx_issue_r")
(define_reservation "kv3_alu_lite_x_r" "kvx_lite_r + kvx_issue_x2_r")
(define_reservation "kv3_alu_lite_y_r" "kvx_lite_r + kvx_issue_x3_r")
(define_reservation "kv3_alu_lite_x2_r" "kvx_lite_x2_r + kvx_issue_x2_r")
(define_reservation "kv3_alu_lite_x2_x_r" "kvx_lite_x2_r + kvx_issue_x4_r")
(define_reservation "kv3_alu_lite_x2_crwl_crwh_r" "kvx_lite_x2_r + kvx_crwl_u + kvx_crwh_u + kvx_issue_x4_r")
(define_reservation "kv3_alu_full_r" "kvx_full_u + kvx_issue_r")
(define_reservation "kv3_alu_full_x_r" "kvx_full_u + kvx_issue_x2_r")
(define_reservation "kv3_alu_full_y_r" "kvx_full_u + kvx_issue_x3_r")
;;
(define_reservation "kv3_lsu_r" "kv3_lsu0_r + kvx_tiny_r + kvx_issue_r")
(define_reservation "kv3_lsu_x_r" "kv3_lsu0_r + kvx_tiny_r + kvx_issue_x2_r")
(define_reservation "kv3_lsu_y_r" "kv3_lsu0_r + kvx_tiny_r + kvx_issue_x3_r")
(define_reservation "kv3_lsu_auxr_auxw_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxr_u + kvx_auxw_u + kvx_issue_r")
(define_reservation "kv3_lsu_auxr_auxw_x_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxr_u + kvx_auxw_u + kvx_issue_x2_r")
(define_reservation "kv3_lsu_auxr_auxw_y_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxr_u + kvx_auxw_u + kvx_issue_x3_r")
(define_reservation "kv3_lsu_auxw_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxw_u + kvx_issue_r")
(define_reservation "kv3_lsu_auxw_x_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxw_u + kvx_issue_x2_r")
(define_reservation "kv3_lsu_auxw_y_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxw_u + kvx_issue_x3_r")
(define_reservation "kv3_lsu_auxr_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxr_u + kvx_issue_r")
(define_reservation "kv3_lsu_auxr_x_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxr_u + kvx_issue_x2_r")
(define_reservation "kv3_lsu_auxr_y_r" "kv3_lsu0_r + kvx_tiny_r + kvx_auxr_u + kvx_issue_x3_r")
(define_reservation "kv3_lsu_crrp_r" "kv3_lsu0_r + kvx_tiny_r + kvx_crrp_u + kvx_issue_r")
(define_reservation "kv3_lsu_crrp_x_r" "kv3_lsu0_r + kvx_tiny_r + kvx_crrp_u + kvx_issue_x2_r")
(define_reservation "kv3_lsu_crrp_y_r" "kv3_lsu0_r + kvx_tiny_r + kvx_crrp_u + kvx_issue_x3_r")

(define_reservation "kv3_mau_r" "kv3_mau0_r + kvx_issue_r")
(define_reservation "kv3_mau_x_r" "kv3_mau0_r + kvx_issue_x2_r")
(define_reservation "kv3_mau_y_r" "kv3_mau0_r + kvx_issue_x3_r")
(define_reservation "kv3_mau_auxr_r" "kv3_mau0_r + kvx_auxr_u + kvx_issue_r")
(define_reservation "kv3_mau_auxr_x_r" "kv3_mau0_r + kvx_auxr_u + kvx_issue_x2_r")
(define_reservation "kv3_mau_auxr_y_r" "kv3_mau0_r + kvx_auxr_u + kvx_issue_x3_r")
(define_reservation "kv3_bcu_r" "kvx_bcu0_u + kvx_issue_r")
(define_reservation "kv3_bcu_xfer_r" "kvx_bcu0_u + kvx_issue_r")
(define_reservation "kv3_bcu_tiny_tiny_mau_r" "kvx_bcu0_u + kvx_tiny_x2_r + kv3_mau0_r + kvx_issue_r")
(define_reservation "kv3_bcu_tiny_auxw_crrp_r" "kvx_bcu0_u + kvx_tiny_r + kvx_auxw_u + kvx_crrp_u + kvx_issue_r")
(define_reservation "kv3_bcu_crrp_crwl_crwh_r" "kvx_bcu0_u + kvx_crrp_u + kvx_crwl_u + kvx_crwh_u + kvx_issue_r")
(define_reservation "kv3_tca_r" "kvx_tca_u + kvx_issue_r")

(define_reservation "kv4_all_r" "kvx_issue_x8_r")
(define_reservation "kv4_alu_full_r" "kvx_full_u + kvx_issue_r")
(define_reservation "kv4_alu_full_x_r" "kvx_full_u + kvx_issue_x2_r")
(define_reservation "kv4_alu_full_y_r" "kvx_full_u + kvx_issue_x3_r")
(define_reservation "kv4_alu_lite_r" "kvx_lite_r + kvx_issue_r")
(define_reservation "kv4_alu_lite_x_r" "kvx_lite_r + kvx_issue_x2_r")
(define_reservation "kv4_alu_lite_y_r" "kvx_lite_r + kvx_issue_x3_r")
(define_reservation "kv4_alu_lite_x2_r" "kvx_lite_x2_r + kvx_issue_x2_r")
(define_reservation "kv4_alu_lite_x2_x_r" "kvx_lite_x2_r + kvx_issue_x4_r")
(define_reservation "kv4_alu_tiny_r" "kvx_tiny_r + kvx_issue_r")
(define_reservation "kv4_alu_tiny_x_r" "kvx_tiny_r + kvx_issue_x2_r")
(define_reservation "kv4_alu_tiny_y_r" "kvx_tiny_r + kvx_issue_x3_r")
(define_reservation "kv4_alu_tiny_x2_r" "kvx_tiny_x2_r + kvx_issue_x2_r")
(define_reservation "kv4_alu_tiny_x2_x_r" "kvx_tiny_x2_r + kvx_issue_x4_r")
(define_reservation "kv4_alu_tiny_x2_y_r" "kvx_tiny_x2_r + kvx_issue_x6_r")
(define_reservation "kv4_alu_tiny_x4_r" "kvx_tiny_x4_r + kvx_issue_x4_r")
(define_reservation "kv4_alu_tiny_x4_x_r" "kvx_tiny_x4_r + kvx_issue_x8_r")
(define_reservation "kv4_bcu_r" "(kvx_bcu0_u | kvx_bcu1_u)+kvx_xfer_u + kvx_issue_r")
(define_reservation "kv4_bcu_xfer_r" "(kvx_bcu0_u | kvx_bcu1_u) + kvx_xfer_u + kvx_issue_r")
(define_reservation "kv4_lsu_r" "(kv4_lsu0_r | kv4_lsu1_r)+kvx_memw_u + kvx_tiny_r + kvx_issue_r")
(define_reservation "kv4_lsu_x_r" "(kv4_lsu0_r | kv4_lsu1_r)+kvx_memw_u + kvx_tiny_r + kvx_issue_x2_r")
(define_reservation "kv4_lsu_y_r" "(kv4_lsu0_r | kv4_lsu1_r)+kvx_memw_u + kvx_tiny_r + kvx_issue_x3_r")
(define_reservation "kv4_lsu_auxr_auxw_r" "(kv4_lsu0_r | kv4_lsu1_r)+kvx_memw_u + kvx_tiny_r + kvx_auxr_u + kvx_auxw_u + kvx_issue_r")
(define_reservation "kv4_lsu_auxr_auxw_memw_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_auxr_u + kvx_auxw_u + kvx_memw_u + kvx_issue_r")
(define_reservation "kv4_lsu_auxr_auxw_memw_x_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_auxr_u + kvx_auxw_u + kvx_memw_u + kvx_issue_x2_r")
(define_reservation "kv4_lsu_auxr_auxw_memw_y_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_auxr_u + kvx_auxw_u + kvx_memw_u + kvx_issue_x3_r")
(define_reservation "kv4_lsu_auxr_memw_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_auxr_u + kvx_memw_u + kvx_issue_r")
(define_reservation "kv4_lsu_auxr_memw_x_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_auxr_u + kvx_memw_u + kvx_issue_x2_r")
(define_reservation "kv4_lsu_auxr_memw_y_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_auxr_u + kvx_memw_u + kvx_issue_x3_r")
(define_reservation "kv4_lsu_auxw_r" "(kv4_lsu0_r | kv4_lsu1_r)+kvx_memw_u + kvx_tiny_r + kvx_auxr_u + kvx_issue_r")
(define_reservation "kv4_lsu_auxw_x_r" "(kv4_lsu0_r | kv4_lsu1_r)+kvx_memw_u + kvx_tiny_r + kvx_auxr_u + kvx_issue_x2_r")
(define_reservation "kv4_lsu_auxw_y_r" "(kv4_lsu0_r | kv4_lsu1_r)+kvx_memw_u + kvx_tiny_r + kvx_auxr_u + kvx_issue_x3_r")
(define_reservation "kv4_lsu_auxw_memw_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_memw_u + kvx_tiny_r + kvx_issue_r")
(define_reservation "kv4_lsu_auxw_memw_x_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_memw_u + kvx_tiny_r + kvx_issue_x2_r")
(define_reservation "kv4_lsu_auxw_memw_y_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_memw_u + kvx_tiny_r + kvx_issue_x3_r")
(define_reservation "kv4_lsu_memw_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_memw_u + kvx_issue_r")
(define_reservation "kv4_lsu_memw_x_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_memw_u + kvx_issue_x2_r")
(define_reservation "kv4_lsu_memw_y_r" "(kv4_lsu0_r | kv4_lsu1_r) + kvx_tiny_r + kvx_memw_u + kvx_issue_x3_r")
(define_reservation "kv4_mau_r" "(kv4_mau0_r | kv4_mau1_r)+kvx_tca_u + kvx_issue_r")
(define_reservation "kv4_mau_auxr_r" "(kv4_mau0_r | kv4_mau1_r)+kvx_tca_u + kvx_auxr_u + kvx_issue_r")
(define_reservation "kv4_mau_auxw_r" "(kv4_mau0_r | kv4_mau1_r)+kvx_tca_u + kvx_auxw_u + kvx_issue_r")
(define_reservation "kv4_mau_tca_r" "(kv4_mau0_r | kv4_mau1_r) + kvx_tca_u + kvx_issue_r")
(define_reservation "kv4_nop_r" "kvx_issue_r")

;; KV3 instruction reservations
(define_insn_reservation "kv3_nop" 1 (and (eq_attr "type" "nop") (match_test "KV3")) "kvx_issue_r")
(define_insn_reservation "kv3_all" 1 (and (eq_attr "type" "all") (match_test "KV3")) "kvx_issue_x8_r")
(define_insn_reservation "kv3_alu_full" 1 (and (eq_attr "type" "alu_full") (match_test "KV3")) "kv3_alu_full_r")
(define_insn_reservation "kv3_alu_full_x" 1 (and (eq_attr "type" "alu_full_x") (match_test "KV3")) "kv3_alu_full_x_r")
(define_insn_reservation "kv3_alu_full_y" 1 (and (eq_attr "type" "alu_full_y") (match_test "KV3")) "kv3_alu_full_y_r")
(define_insn_reservation "kv3_alu_full_sfu" 15 (and (eq_attr "type" "alu_full_sfu") (match_test "KV3")) "kv3_alu_full_r")
(define_insn_reservation "kv3_alu_lite" 1 (and (eq_attr "type" "alu_lite") (match_test "KV3")) "kv3_alu_lite_r")
(define_insn_reservation "kv3_alu_lite_x" 1 (and (eq_attr "type" "alu_lite_x") (match_test "KV3")) "kv3_alu_lite_x_r")
(define_insn_reservation "kv3_alu_lite_y" 1 (and (eq_attr "type" "alu_lite_y") (match_test "KV3")) "kv3_alu_lite_y_r")
(define_insn_reservation "kv3_alu_lite_x2" 1 (and (eq_attr "type" "alu_lite_x2") (match_test "KV3")) "kv3_alu_lite_x2_r")
(define_insn_reservation "kv3_alu_lite_x2_x" 1 (and (eq_attr "type" "alu_lite_x2_x") (match_test "KV3")) "kv3_alu_lite_x2_x_r")
(define_insn_reservation "kv3_alu_tiny" 1 (and (eq_attr "type" "alu_tiny") (match_test "KV3")) "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_tiny_send" 1 (and (eq_attr "type" "alu_tiny_send") (match_test "KV3")) "kv3_alu_tiny_send_r")
(define_insn_reservation "kv3_alu_tiny_recv" 1 (and (eq_attr "type" "alu_tiny_recv") (match_test "KV3")) "kv3_alu_tiny_recv_r")
(define_insn_reservation "kv3_alu_tiny_sendrecv" 1 (and (eq_attr "type" "alu_tiny_sendrecv") (match_test "KV3")) "kv3_alu_tiny_sendrecv_r")
(define_insn_reservation "kv3_alu_tiny_use" 0 (and (eq_attr "type" "alu_tiny_use") (match_test "KV3")) "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_tiny_x" 1 (and (eq_attr "type" "alu_tiny_x") (match_test "KV3")) "kv3_alu_tiny_x_r")
(define_insn_reservation "kv3_alu_tiny_y" 1 (and (eq_attr "type" "alu_tiny_y") (match_test "KV3")) "kv3_alu_tiny_y_r")
(define_insn_reservation "kv3_alu_tiny_x2" 1 (and (eq_attr "type" "alu_tiny_x2") (match_test "KV3")) "kv3_alu_tiny_x2_r")
(define_insn_reservation "kv3_alu_tiny_use_x2" 0 (and (eq_attr "type" "alu_tiny_use_x2") (match_test "KV3")) "kv3_alu_tiny_x2_r")
(define_insn_reservation "kv3_alu_tiny_x2_x" 1 (and (eq_attr "type" "alu_tiny_x2_x") (match_test "KV3")) "kv3_alu_tiny_x2_x_r")
(define_insn_reservation "kv3_alu_tiny_x2_y" 1 (and (eq_attr "type" "alu_tiny_x2_y") (match_test "KV3")) "kv3_alu_tiny_x2_y_r")
(define_insn_reservation "kv3_alu_tiny_x4" 1 (and (eq_attr "type" "alu_tiny_x4") (match_test "KV3")) "kv3_alu_tiny_x4_r")
(define_insn_reservation "kv3_alu_tiny_x4_x" 1 (and (eq_attr "type" "alu_tiny_x4_x") (match_test "KV3")) "kv3_alu_tiny_x4_x_r")
(define_insn_reservation "kv3_alu_tiny_w" 1 (and (eq_attr "type" "alu_tiny_w") (match_test "KV3")) "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_thin_v1" 1 (and (eq_attr "type" "alu_thin") (match_test "KV3_1")) "kv3_alu_lite_r")
(define_insn_reservation "kv3_alu_thin_v2" 1 (and (eq_attr "type" "alu_thin") (match_test "KV3_2")) "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_thin_w_v1" 1 (and (eq_attr "type" "alu_thin_w") (match_test "KV3_1")) "kv3_alu_lite_r")
(define_insn_reservation "kv3_alu_thin_w_v2" 1 (and (eq_attr "type" "alu_thin_w") (match_test "KV3_2")) "kv3_alu_tiny_r")
(define_insn_reservation "kv3_alu_thin_x_v1" 1 (and (eq_attr "type" "alu_thin_x") (match_test "KV3_1")) "kv3_alu_lite_x_r")
(define_insn_reservation "kv3_alu_thin_x_v2" 1 (and (eq_attr "type" "alu_thin_x") (match_test "KV3_2")) "kv3_alu_tiny_x_r")
(define_insn_reservation "kv3_alu_thin_y_v1" 1 (and (eq_attr "type" "alu_thin_y") (match_test "KV3_1")) "kv3_alu_lite_y_r")
(define_insn_reservation "kv3_alu_thin_y_v2" 1 (and (eq_attr "type" "alu_thin_y") (match_test "KV3_2")) "kv3_alu_tiny_y_r")
(define_insn_reservation "kv3_alu_thin_x2_v1" 1 (and (eq_attr "type" "alu_thin_x2") (match_test "KV3_1")) "kv3_alu_lite_x2_r")
(define_insn_reservation "kv3_alu_thin_x2_v2" 1 (and (eq_attr "type" "alu_thin_x2") (match_test "KV3_2")) "kv3_alu_tiny_x2_r")
(define_insn_reservation "kv3_alu_thin_x2_x_v1" 1 (and (eq_attr "type" "alu_thin_x2_x") (match_test "KV3_1")) "kv3_alu_lite_x2_x_r")
(define_insn_reservation "kv3_alu_thin_x2_x_v2" 1 (and (eq_attr "type" "alu_thin_x2_x") (match_test "KV3_2")) "kv3_alu_tiny_x2_x_r")
(define_insn_reservation "kv3_movet_copro_v1" 2 (and (eq_attr "type" "movet_copro") (match_test "KV3_1")) "kv3_alu_lite_x2_crwl_crwh_r")
(define_insn_reservation "kv3_movet_copro_v2" 2 (and (eq_attr "type" "movet_copro") (match_test "KV3_2")) "kv3_alu_tiny_x2_crwl_crwh_r")
(define_insn_reservation "kv3_cache" 1 (and (eq_attr "type" "cache") (match_test "KV3")) "kv3_lsu_r")
(define_insn_reservation "kv3_cache_x" 1 (and (eq_attr "type" "cache_x") (match_test "KV3")) "kv3_lsu_x_r")
(define_insn_reservation "kv3_cache_y" 1 (and (eq_attr "type" "cache_y") (match_test "KV3")) "kv3_lsu_y_r")
(define_insn_reservation "kv3_preload" 1 (and (eq_attr "type" "preload") (match_test "KV3")) "kv3_lsu_r")
(define_insn_reservation "kv3_preload_x" 1 (and (eq_attr "type" "preload_x") (match_test "KV3")) "kv3_lsu_x_r")
(define_insn_reservation "kv3_preload_y" 1 (and (eq_attr "type" "preload_y") (match_test "KV3")) "kv3_lsu_y_r")
(define_insn_reservation "kv3_load_copro" 3 (and (eq_attr "type" "load_copro") (match_test "KV3")) "kv3_lsu_r")
(define_insn_reservation "kv3_load_copro_x" 3 (and (eq_attr "type" "load_copro_x") (match_test "KV3")) "kv3_lsu_x_r")
(define_insn_reservation "kv3_load_copro_y" 3 (and (eq_attr "type" "load_copro_y") (match_test "KV3")) "kv3_lsu_y_r")
(define_insn_reservation "kv3_load_core" 3 (and (eq_attr "type" "load_core") (match_test "KV3")) "kv3_lsu_auxw_r")
(define_insn_reservation "kv3_load_core_x" 3 (and (eq_attr "type" "load_core_x") (match_test "KV3")) "kv3_lsu_auxw_x_r")
(define_insn_reservation "kv3_load_core_y" 3 (and (eq_attr "type" "load_core_y") (match_test "KV3")) "kv3_lsu_auxw_y_r")
(define_insn_reservation "kv3_load_core_uncached" 24 (and (eq_attr "type" "load_core_uncached") (match_test "KV3")) "kv3_lsu_auxw_r")
(define_insn_reservation "kv3_load_core_uncached_x" 24 (and (eq_attr "type" "load_core_uncached_x") (match_test "KV3")) "kv3_lsu_auxw_x_r")
(define_insn_reservation "kv3_load_core_uncached_y" 24 (and (eq_attr "type" "load_core_uncached_y") (match_test "KV3")) "kv3_lsu_auxw_y_r")
(define_insn_reservation "kv3_load_copro_uncached" 24 (and (eq_attr "type" "load_copro_uncached") (match_test "KV3")) "kv3_lsu_r")
(define_insn_reservation "kv3_load_copro_uncached_x" 24 (and (eq_attr "type" "load_copro_uncached_x") (match_test "KV3")) "kv3_lsu_x_r")
(define_insn_reservation "kv3_load_copro_uncached_y" 24 (and (eq_attr "type" "load_copro_uncached_y") (match_test "KV3")) "kv3_lsu_y_r")
(define_insn_reservation "kv3_store_core" 1 (and (eq_attr "type" "store_core") (match_test "KV3")) "kv3_lsu_auxr_r")
(define_insn_reservation "kv3_store_core_x" 1 (and (eq_attr "type" "store_core_x") (match_test "KV3")) "kv3_lsu_auxr_x_r")
(define_insn_reservation "kv3_store_core_y" 1 (and (eq_attr "type" "store_core_y") (match_test "KV3")) "kv3_lsu_auxr_y_r")
(define_insn_reservation "kv3_store_copro" 1 (and (eq_attr "type" "store_copro") (match_test "KV3")) "kv3_lsu_crrp_r")
(define_insn_reservation "kv3_store_copro_x" 1 (and (eq_attr "type" "store_copro_x") (match_test "KV3")) "kv3_lsu_crrp_x_r")
(define_insn_reservation "kv3_store_copro_y" 1 (and (eq_attr "type" "store_copro_y") (match_test "KV3")) "kv3_lsu_crrp_y_r")
(define_insn_reservation "kv3_aload_core" 24 (and (eq_attr "type" "aload_core") (match_test "KV3")) "kv3_lsu_auxw_r")
(define_insn_reservation "kv3_aload_core_x" 24 (and (eq_attr "type" "aload_core_x") (match_test "KV3")) "kv3_lsu_auxw_x_r")
(define_insn_reservation "kv3_aload_core_y" 24 (and (eq_attr "type" "aload_core_y") (match_test "KV3")) "kv3_lsu_auxw_y_r")
(define_insn_reservation "kv3_atomic_core" 24 (and (eq_attr "type" "atomic_core") (match_test "KV3")) "kv3_lsu_auxr_auxw_r")
(define_insn_reservation "kv3_atomic_core_x" 24 (and (eq_attr "type" "atomic_core_x") (match_test "KV3")) "kv3_lsu_auxr_auxw_x_r")
(define_insn_reservation "kv3_atomic_core_y" 24 (and (eq_attr "type" "atomic_core_y") (match_test "KV3")) "kv3_lsu_auxr_auxw_y_r")
(define_insn_reservation "kv3_copy_core" 3 (and (eq_attr "type" "copy_core") (match_test "KV3")) "kv3_lsu_auxr_auxw_r")
(define_insn_reservation "kv3_mult_int" 2 (and (eq_attr "type" "mult_int") (match_test "KV3")) "kv3_mau_r")
(define_insn_reservation "kv3_mult_int_x" 2 (and (eq_attr "type" "mult_int_x") (match_test "KV3")) "kv3_mau_x_r")
(define_insn_reservation "kv3_mult_int_y" 2 (and (eq_attr "type" "mult_int_y") (match_test "KV3")) "kv3_mau_y_r")
(define_insn_reservation "kv3_mult_fp3" 3 (and (eq_attr "type" "mult_fp3") (match_test "KV3")) "kv3_mau_r")
(define_insn_reservation "kv3_mult_fp4" 4 (and (eq_attr "type" "mult_fp4") (match_test "KV3")) "kv3_mau_r")
(define_insn_reservation "kv3_conv_fp4" 4 (and (eq_attr "type" "conv_fp4") (match_test "KV3")) "kv3_mau_r")
(define_insn_reservation "kv3_madd_int" 2 (and (eq_attr "type" "madd_int") (match_test "KV3")) "kv3_mau_auxr_r")
(define_insn_reservation "kv3_madd_int_x" 2 (and (eq_attr "type" "madd_int_x") (match_test "KV3")) "kv3_mau_auxr_x_r")
(define_insn_reservation "kv3_madd_int_y" 2 (and (eq_attr "type" "madd_int_y") (match_test "KV3")) "kv3_mau_auxr_y_r")
(define_insn_reservation "kv3_madd_fp3" 3 (and (eq_attr "type" "madd_fp3") (match_test "KV3")) "kvx_auxr_u + kv3_mau0_r + kvx_issue_r")
(define_insn_reservation "kv3_madd_fp4" 4 (and (eq_attr "type" "madd_fp4") (match_test "KV3")) "kvx_auxr_u + kv3_mau0_r + kvx_issue_r")
(define_insn_reservation "kv3_bcu" 1 (and (eq_attr "type" "bcu") (match_test "KV3")) "kv3_bcu_r")
(define_insn_reservation "kv3_bcu_get" 1 (and (eq_attr "type" "bcu_get") (match_test "KV3")) "kv3_bcu_tiny_tiny_mau_r")
(define_insn_reservation "kv3_movef_copro" 3 (and (eq_attr "type" "movef_copro") (match_test "KV3")) "kv3_bcu_tiny_auxw_crrp_r")
(define_insn_reservation "kv3_copy_copro" 1 (and (eq_attr "type" "copy_copro") (match_test "KV3")) "kv3_bcu_crrp_crwl_crwh_r")
(define_insn_reservation "kv3_bcu_xfer" 1 (and (eq_attr "type" "bcu_xfer") (match_test "KV3")) "kv3_bcu_xfer_r")
(define_insn_reservation "kv3_tca" 1 (and (eq_attr "type" "tca") (match_test "KV3")) "kv3_tca_r")
(define_insn_reservation "kv3_tca_int" 3 (and (eq_attr "type" "tca_int") (match_test "KV3")) "kv3_tca_r")
(define_insn_reservation "kv3_tca_float" 4 (and (eq_attr "type" "tca_float") (match_test "KV3")) "kv3_tca_r")

;; KV4 instruction reservations
(define_insn_reservation "kv4_nop" 1 (and (eq_attr "type" "nop") (match_test "KV4")) "kv4_nop_r")
(define_insn_reservation "kv4_all" 1 (and (eq_attr "type" "all") (match_test "KV4")) "kv4_all_r")
(define_insn_reservation "kv4_alu_full" 1 (and (eq_attr "type" "alu_full") (match_test "KV4")) "kv4_alu_full_r")
(define_insn_reservation "kv4_alu_full_x" 1 (and (eq_attr "type" "alu_full_x") (match_test "KV4")) "kv4_alu_full_x_r")
(define_insn_reservation "kv4_alu_full_y" 1 (and (eq_attr "type" "alu_full_y") (match_test "KV4")) "kv4_alu_full_y_r")
(define_insn_reservation "kv4_alu_full_sfu" 15 (and (eq_attr "type" "alu_full_sfu") (match_test "KV4")) "kv4_alu_full_r")
(define_insn_reservation "kv4_alu_lite" 1 (and (eq_attr "type" "alu_lite") (match_test "KV4")) "kv4_alu_lite_r")
(define_insn_reservation "kv4_alu_lite_x" 1 (and (eq_attr "type" "alu_lite_x") (match_test "KV4")) "kv4_alu_lite_x_r")
(define_insn_reservation "kv4_alu_lite_y" 1 (and (eq_attr "type" "alu_lite_y") (match_test "KV4")) "kv4_alu_lite_y_r")
(define_insn_reservation "kv4_alu_lite_x2" 1 (and (eq_attr "type" "alu_lite_x2") (match_test "KV4")) "kv4_alu_lite_x2_r")
(define_insn_reservation "kv4_alu_lite_x2_x" 1 (and (eq_attr "type" "alu_lite_x2_x") (match_test "KV4")) "kv4_alu_lite_x2_x_r")
(define_insn_reservation "kv4_alu_tiny" 1 (and (eq_attr "type" "alu_tiny") (match_test "KV4")) "kv4_alu_tiny_r")
(define_insn_reservation "kv4_alu_tiny_send" 1 (and (eq_attr "type" "alu_tiny_send") (match_test "KV4")) "kv4_alu_tiny_r")
(define_insn_reservation "kv4_alu_tiny_recv" 1 (and (eq_attr "type" "alu_tiny_recv") (match_test "KV4")) "kv4_alu_tiny_r")
(define_insn_reservation "kv4_alu_tiny_sendrecv" 1 (and (eq_attr "type" "alu_tiny_sendrecv") (match_test "KV4")) "kv4_alu_tiny_r")
(define_insn_reservation "kv4_alu_tiny_use" 0 (and (eq_attr "type" "alu_tiny_use") (match_test "KV4")) "kv4_alu_tiny_r")
(define_insn_reservation "kv4_alu_tiny_x" 1 (and (eq_attr "type" "alu_tiny_x") (match_test "KV4")) "kv4_alu_tiny_x_r")
(define_insn_reservation "kv4_alu_tiny_y" 1 (and (eq_attr "type" "alu_tiny_y") (match_test "KV4")) "kv4_alu_tiny_y_r")
(define_insn_reservation "kv4_alu_tiny_x2" 1 (and (eq_attr "type" "alu_tiny_x2") (match_test "KV4")) "kv4_alu_tiny_x2_r")
(define_insn_reservation "kv4_alu_tiny_use_x2" 0 (and (eq_attr "type" "alu_tiny_use_x2") (match_test "KV4")) "kv4_alu_tiny_x2_r")
(define_insn_reservation "kv4_alu_tiny_x2_x" 1 (and (eq_attr "type" "alu_tiny_x2_x") (match_test "KV4")) "kv4_alu_tiny_x2_x_r")
(define_insn_reservation "kv4_alu_tiny_x2_y" 1 (and (eq_attr "type" "alu_tiny_x2_y") (match_test "KV4")) "kv4_alu_tiny_x2_y_r")
(define_insn_reservation "kv4_alu_tiny_x4" 1 (and (eq_attr "type" "alu_tiny_x4") (match_test "KV4")) "kv4_alu_tiny_x4_r")
(define_insn_reservation "kv4_alu_tiny_x4_x" 1 (and (eq_attr "type" "alu_tiny_x4_x") (match_test "KV4")) "kv4_alu_tiny_x4_x_r")
(define_insn_reservation "kv4_alu_tiny_w" 1 (and (eq_attr "type" "alu_tiny_w") (match_test "KV4")) "kv4_alu_tiny_x_r")
(define_insn_reservation "kv4_alu_thin" 1 (and (eq_attr "type" "alu_thin") (match_test "KV4")) "kv4_alu_tiny_r")
;;
(define_insn_reservation "kv4_alu_thin_w" 1 (and (eq_attr "type" "alu_thin_w") (match_test "KV4")) "kv4_alu_tiny_x_r")
;;
(define_insn_reservation "kv4_alu_thin_x" 1 (and (eq_attr "type" "alu_thin_x") (match_test "KV4")) "kv4_alu_tiny_x_r")
;;
(define_insn_reservation "kv4_alu_thin_y" 1 (and (eq_attr "type" "alu_thin_y") (match_test "KV4")) "kv4_alu_tiny_y_r")
;;
(define_insn_reservation "kv4_alu_thin_x2" 1 (and (eq_attr "type" "alu_thin_x2") (match_test "KV4")) "kv4_alu_tiny_x2_r")
;;
(define_insn_reservation "kv4_alu_thin_x2_x" 1 (and (eq_attr "type" "alu_thin_x2_x") (match_test "KV4")) "kv4_alu_tiny_x2_x_r")
;;
(define_insn_reservation "kv4_movet_copro_v2" 2 (and (eq_attr "type" "movet_copro") (match_test "KV4")) "kv4_mau_auxw_r")
;;
(define_insn_reservation "kv4_cache" 1 (and (eq_attr "type" "cache") (match_test "KV4")) "kv4_lsu_r")
(define_insn_reservation "kv4_cache_x" 1 (and (eq_attr "type" "cache_x") (match_test "KV4")) "kv4_lsu_x_r")
(define_insn_reservation "kv4_cache_y" 1 (and (eq_attr "type" "cache_y") (match_test "KV4")) "kv4_lsu_y_r")
(define_insn_reservation "kv4_preload" 1 (and (eq_attr "type" "preload") (match_test "KV4")) "kv4_lsu_r")
(define_insn_reservation "kv4_preload_x" 1 (and (eq_attr "type" "preload_x") (match_test "KV4")) "kv4_lsu_x_r")
(define_insn_reservation "kv4_preload_y" 1 (and (eq_attr "type" "preload_y") (match_test "KV4")) "kv4_lsu_y_r")
(define_insn_reservation "kv4_load_copro" 3 (and (eq_attr "type" "load_copro") (match_test "KV4")) "kv4_lsu_r")
(define_insn_reservation "kv4_load_copro_x" 3 (and (eq_attr "type" "load_copro_x") (match_test "KV4")) "kv4_lsu_x_r")
(define_insn_reservation "kv4_load_copro_y" 3 (and (eq_attr "type" "load_copro_y") (match_test "KV4")) "kv4_lsu_y_r")
(define_insn_reservation "kv4_load_core" 3 (and (eq_attr "type" "load_core") (match_test "KV4")) "kv4_lsu_auxw_r")
(define_insn_reservation "kv4_load_core_x" 3 (and (eq_attr "type" "load_core_x") (match_test "KV4")) "kv4_lsu_auxw_x_r")
(define_insn_reservation "kv4_load_core_y" 3 (and (eq_attr "type" "load_core_y") (match_test "KV4")) "kv4_lsu_auxw_y_r")
(define_insn_reservation "kv4_load_core_uncached" 24 (and (eq_attr "type" "load_core_uncached") (match_test "KV4")) "kv4_lsu_auxw_r")
(define_insn_reservation "kv4_load_core_uncached_x" 24 (and (eq_attr "type" "load_core_uncached_x") (match_test "KV4")) "kv4_lsu_auxw_x_r")
(define_insn_reservation "kv4_load_core_uncached_y" 24 (and (eq_attr "type" "load_core_uncached_y") (match_test "KV4")) "kv4_lsu_auxw_y_r")
(define_insn_reservation "kv4_load_copro_uncached" 24 (and (eq_attr "type" "load_copro_uncached") (match_test "KV4")) "kv4_lsu_r")
(define_insn_reservation "kv4_load_copro_uncached_x" 24 (and (eq_attr "type" "load_copro_uncached_x") (match_test "KV4")) "kv4_lsu_x_r")
(define_insn_reservation "kv4_load_copro_uncached_y" 24 (and (eq_attr "type" "load_copro_uncached_y") (match_test "KV4")) "kv4_lsu_y_r")
(define_insn_reservation "kv4_store_core" 1 (and (eq_attr "type" "store_core") (match_test "KV4")) "kv4_lsu_auxr_memw_r")
(define_insn_reservation "kv4_store_core_x" 1 (and (eq_attr "type" "store_core_x") (match_test "KV4")) "kv4_lsu_auxr_memw_x_r")
(define_insn_reservation "kv4_store_core_y" 1 (and (eq_attr "type" "store_core_y") (match_test "KV4")) "kv4_lsu_auxr_memw_y_r")
(define_insn_reservation "kv4_store_copro" 1 (and (eq_attr "type" "store_copro") (match_test "KV4")) "kv4_lsu_memw_r")
(define_insn_reservation "kv4_store_copro_x" 1 (and (eq_attr "type" "store_copro_x") (match_test "KV4")) "kv4_lsu_memw_x_r")
(define_insn_reservation "kv4_store_copro_y" 1 (and (eq_attr "type" "store_copro_y") (match_test "KV4")) "kv4_lsu_memw_y_r")
(define_insn_reservation "kv4_aload_core" 24 (and (eq_attr "type" "aload_core") (match_test "KV4")) "kv4_lsu_auxw_memw_r")
(define_insn_reservation "kv4_aload_core_x" 24 (and (eq_attr "type" "aload_core_x") (match_test "KV4")) "kv4_lsu_auxw_memw_x_r")
(define_insn_reservation "kv4_aload_core_y" 24 (and (eq_attr "type" "aload_core_y") (match_test "KV4")) "kv4_lsu_auxw_memw_y_r")
(define_insn_reservation "kv4_atomic_core" 24 (and (eq_attr "type" "atomic_core") (match_test "KV4")) "kv4_lsu_auxr_auxw_memw_r")
(define_insn_reservation "kv4_atomic_core_x" 24 (and (eq_attr "type" "atomic_core_x") (match_test "KV4")) "kv4_lsu_auxr_auxw_memw_x_r")
(define_insn_reservation "kv4_atomic_core_y" 24 (and (eq_attr "type" "atomic_core_y") (match_test "KV4")) "kv4_lsu_auxr_auxw_memw_y_r")
(define_insn_reservation "kv4_copy_core" 3 (and (eq_attr "type" "copy_core") (match_test "KV4")) "kv4_lsu_auxr_auxw_r")
(define_insn_reservation "kv4_mult_int" 2 (and (eq_attr "type" "mult_int") (match_test "KV4")) "kv4_alu_lite_r")
(define_insn_reservation "kv4_mult_int_x" 2 (and (eq_attr "type" "mult_int_x") (match_test "KV4")) "kv4_alu_lite_x_r")
;;
(define_insn_reservation "kv4_mult_fp3" 3 (and (eq_attr "type" "mult_fp3") (match_test "KV4")) "kv4_mau_r")
(define_insn_reservation "kv4_mult_fp4" 4 (and (eq_attr "type" "mult_fp4") (match_test "KV4")) "kv4_mau_r")
(define_insn_reservation "kv4_conv_fp4" 4 (and (eq_attr "type" "conv_fp4") (match_test "KV4")) "kv4_mau_r")
(define_insn_reservation "kv4_madd_int" 2 (and (eq_attr "type" "madd_int") (match_test "KV4")) "kv4_alu_lite_r")
(define_insn_reservation "kv4_madd_int_x" 2 (and (eq_attr "type" "madd_int_x") (match_test "KV4")) "kv4_alu_lite_x_r")
(define_insn_reservation "kv4_madd_int_y" 2 (and (eq_attr "type" "madd_int_y") (match_test "KV4")) "kv4_alu_lite_y_r")
(define_insn_reservation "kv4_madd_fp3" 3 (and (eq_attr "type" "madd_fp3") (match_test "KV4")) "kv4_mau_auxr_r")
(define_insn_reservation "kv4_madd_fp4" 4 (and (eq_attr "type" "madd_fp4") (match_test "KV4")) "kv4_mau_auxr_r")
(define_insn_reservation "kv4_bcu" 1 (and (eq_attr "type" "bcu") (match_test "KV4")) "kv4_bcu_r")
(define_insn_reservation "kv4_bcu_get" 1 (and (eq_attr "type" "bcu_get") (match_test "KV4")) "kv4_bcu_r")
(define_insn_reservation "kv4_movef_copro" 3 (and (eq_attr "type" "movef_copro") (match_test "KV4")) "kv4_mau_auxw_r")
(define_insn_reservation "kv4_copy_copro" 1 (and (eq_attr "type" "copy_copro") (match_test "KV4")) "kv4_bcu_r")
(define_insn_reservation "kv4_bcu_xfer" 1 (and (eq_attr "type" "bcu_xfer") (match_test "KV4")) "kv4_bcu_xfer_r")
(define_insn_reservation "kv4_tca" 1 (and (eq_attr "type" "tca") (match_test "KV4")) "kv4_mau_tca_r")
(define_insn_reservation "kv4_tca_int" 3 (and (eq_attr "type" "tca_int") (match_test "KV4")) "kv4_mau_tca_r")
(define_insn_reservation "kv4_tca_float" 4 (and (eq_attr "type" "tca_float") (match_test "KV4")) "kv4_mau_tca_r")

;; Bypasses
;; The KV3 BCU reads GPRs one cycle earlier than the other execution units.
(define_bypass 2 "kv3_alu*"
                 "kv3_bcu*"
                 "kvx_branch_tested_bypass_p")
(define_bypass 3 "kv3_mult_int*,kv3_madd_int*"
                 "kv3_bcu*"
                 "kvx_branch_tested_bypass_p")
(define_bypass 4 "kv3_load_core*"
                 "kv3_bcu*"
                 "kvx_branch_tested_bypass_p")
(define_bypass 4 "kv3_mult_fp3*,kv3_madd_fp3*"
                 "kv3_bcu*"
                 "kvx_branch_tested_bypass_p")
(define_bypass 5 "kv3_mult_fp4*,kv3_madd_fp4*"
                 "kv3_bcu*"
                 "kvx_branch_tested_bypass_p")
;; The stores read their input one cycle later than other execution units.
(define_bypass 1 "kv3_mult_int*,kv3_madd_int*,kv4_mult_int*,kv4_madd_int*"
                 "kv3_store_core*,kv4_store_core*"
                 "kvx_stored_value_bypass_p")
(define_bypass 2 "kv3_mult_fp3*,kv3_madd_fp3*,kv4_mult_fp3*,kv4_madd_fp3*"
                 "kv3_store_core*,kv4_store_core*"
                 "kvx_stored_value_bypass_p")
(define_bypass 3 "kv3_mult_fp4*,kv3_madd_fp4*,kv4_mult_fp4*,kv4_madd_fp4*"
                 "kv3_store_core*,kv4_store_core*"
                 "kvx_stored_value_bypass_p")
(define_bypass 2 "kv3_load_core*,kv4_load_core*"
                 "kv3_store_core*,kv4_store_core*"
                 "kvx_stored_value_bypass_p")
;; The integer MAC accumulator reads its input cycle later than other execution units..
(define_bypass 1 "kv3_mult_int*,kv3_madd_int*,kv4_mult_int*,kv4_madd_int*"
                 "kv3_madd_int*,kv4_madd_int*"
                 "kvx_accumulator_bypass_p")
(define_bypass 2 "kv3_load_core*,kv4_load_core*"
                 "kv3_madd_int*,kv4_madd_int*"
                 "kvx_accumulator_bypass_p")
;; The KV3 coprocessor has a bypass between the BILAU accumulations.
(define_bypass 1 "kv3_tca_int"
                 "kv3_tca_int"
                 "kvx_accumulator_bypass_p")
;; The KV3 coprocessor does not have a bypass from any E1 source to the BILAU accumulators.
(define_bypass 3 "kv3_bcu*"
                 "kv3_tca_int"
                 "kvx_accumulator_bypass_p")
(define_bypass 3 "kv3_alu*"
                 "kv3_tca_int"
                 "kvx_accumulator_bypass_p")
;; The KV3 coprocessor does not have a bypass from any E1 source to the BLAU accumulators.
(define_bypass 4 "kv3_tca"
                 "kv3_tca_float"
                 "kvx_accumulator_bypass_p")
(define_bypass 4 "kv3_alu*"
                 "kv3_tca_float"
                 "kvx_accumulator_bypass_p")

