(automata_option "ndfa")
(automata_option "v")

(define_automaton "coolidge_exu")
(define_automaton "coolidge_issue")

(define_cpu_unit
  "k1c_bcu_u,
   k1c_alu_u,
   k1c_lite0_u,
   k1c_lite1_u,
   k1c_tiny0_u,
   k1c_tiny1_u,
   k1c_tiny2_u,
   k1c_tiny3_u,
   k1c_mau_u,
   k1c_lsu_u,
   k1c_auxw_u,
   k1c_auxr_u"
  "coolidge_exu")

(define_cpu_unit
  "k1c_issue0, k1c_issue1, k1c_issue2, k1c_issue3, k1c_issue4, k1c_issue5, k1c_issue6, k1c_issue7"
  "coolidge_issue")

(absence_set "k1c_alu_u"      "k1c_tiny0_u, k1c_lite0_u")
(absence_set "k1c_tiny0_u" "k1c_alu_u, k1c_lite0_u")
(absence_set "k1c_lite0_u" "k1c_tiny0_u, k1c_alu_u")

(absence_set "k1c_tiny1_u" "k1c_lite1_u")
(absence_set "k1c_lite1_u" "k1c_tiny1_u")

(absence_set "k1c_mau_u"      "k1c_tiny2_u")
(absence_set "k1c_tiny2_u" "k1c_mau_u")

(define_reservation "k1c_alu_r"  "(k1c_alu_u                                                          )")
(define_reservation "k1c_lite_r" "(k1c_lite0_u | k1c_lite1_u                                   )")
(define_reservation "k1c_tiny_r" "(k1c_tiny0_u | k1c_tiny1_u | k1c_tiny3_u | k1c_tiny2_u )")

(define_reservation "k1c_issue_r" "(k1c_issue0|k1c_issue1|k1c_issue2|k1c_issue3|k1c_issue4|k1c_issue5|k1c_issue6|k1c_issue7)")
(define_reservation "k1c_issue_x2_r" "(k1c_issue0+k1c_issue1)|(k1c_issue1+k1c_issue2)|(k1c_issue2+k1c_issue3)|(k1c_issue3+k1c_issue4)|(k1c_issue4+k1c_issue5)|(k1c_issue5+k1c_issue6)|(k1c_issue6+k1c_issue7)")

(define_reservation "k1c_issue_x3_r" "(k1c_issue0+k1c_issue1+k1c_issue2)|(k1c_issue1+k1c_issue2+k1c_issue3)|(k1c_issue2+k1c_issue3+k1c_issue4)|(k1c_issue3+k1c_issue4+k1c_issue5)|(k1c_issue4+k1c_issue5+k1c_issue6)|(k1c_issue5+k1c_issue6+k1c_issue7)")

(define_reservation "k1c_all_r"
 "(k1c_issue0 + k1c_issue1 + k1c_issue2 + k1c_issue3 + k1c_issue4 + k1c_issue5 + k1c_issue6 + k1c_issue7)")

(define_reservation "k1c_alu_full_r"
  "k1c_alu_r + k1c_issue_r")

(define_reservation "k1c_alu_full.x_r"
  "k1c_alu_r + k1c_issue_x2_r")

(define_reservation "k1c_alu_full.y_r"
  "k1c_alu_r + k1c_issue_x3_r")

(define_reservation "k1c_alu_lite_r"
  "k1c_lite_r + k1c_issue_r")

(define_reservation "k1c_alu_lite.x_r"
  "k1c_lite_r + k1c_issue_x2_r")

(define_reservation "k1c_alu_lite.y_r"
  "k1c_lite_r + k1c_issue_x3_r")

(define_reservation "k1c_alu_tiny_r"
  "k1c_tiny_r + k1c_issue_r")

(define_reservation "k1c_alu_tiny.x_r"
  "k1c_tiny_r + k1c_issue_x2_r")

(define_reservation "k1c_alu_tiny.y_r"
  "k1c_tiny_r + k1c_issue_x3_r")

(define_reservation "k1c_bcu_r"
  "k1c_bcu_u + k1c_issue_r")

;; fake reservation for tiny_lsu : EXU is not really used but adding 3 tinys would make the
;; the dispatcher try to use MAU tiny as lsu's not used.
(define_reservation "k1c_bcu_tiny_tiny_mau_r"
  "k1c_mau_u + k1c_bcu_u + k1c_tiny3_u + k1c_issue_r")

(define_reservation "k1c_lsu_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_issue_r")

(define_reservation "k1c_lsu_auxr_auxw_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_auxr_u + k1c_auxw_u + k1c_issue_r")

(define_reservation "k1c_lsu_auxr_auxw.x_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_auxr_u + k1c_auxw_u + k1c_issue_x2_r")

(define_reservation "k1c_lsu.x_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_issue_x2_r")

(define_reservation "k1c_lsu_auxw_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_auxw_u + k1c_issue_r")

(define_reservation "k1c_lsu_auxw.x_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_auxw_u + k1c_issue_x2_r")

(define_reservation "k1c_lsu_auxw.y_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_auxw_u + k1c_issue_x3_r")

(define_reservation "k1c_lsu_auxr_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_auxr_u + k1c_issue_r")

(define_reservation "k1c_lsu_auxr.x_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_auxr_u + k1c_issue_x2_r")

(define_reservation "k1c_lsu_auxr.y_r"
  "k1c_lsu_u + k1c_tiny3_u + k1c_auxr_u + k1c_issue_x3_r")

(define_reservation "k1c_mau_r"
  "k1c_mau_u + k1c_issue_r")

(define_reservation "k1c_mau.x_r"
  "k1c_mau_u + k1c_issue_x2_r")

(define_reservation "k1c_mau.y_r"
  "k1c_mau_u + k1c_issue_x3_r")

(define_reservation "k1c_mau_auxr_r"
  "k1c_mau_u + k1c_auxr_u + k1c_issue_r")

(define_reservation "k1c_mau_auxr.x_r"
  "k1c_mau_u + k1c_auxr_u + k1c_issue_x2_r")

(define_reservation "k1c_mau_auxr.y_r"
  "k1c_mau_u + k1c_auxr_u + k1c_issue_x3_r")


(define_insn_reservation "k1c_all" 1
  (eq_attr "type" "all")
  "k1c_all_r")

(define_insn_reservation "k1c_tiny" 1
  (eq_attr "type" "alu_tiny")
  "k1c_alu_tiny_r")

(define_insn_reservation "k1c_tiny.x" 1
  (eq_attr "type" "alu_tiny_x")
  "k1c_alu_tiny.x_r")

(define_insn_reservation "k1c_tiny.y" 1
  (eq_attr "type" "alu_tiny_y")
  "k1c_alu_tiny.y_r")

(define_insn_reservation "k1c_lite" 1
  (eq_attr "type" "alu_lite")
  "k1c_alu_lite_r")

(define_insn_reservation "k1c_lite.x" 1
  (eq_attr "type" "alu_lite_x")
  "k1c_alu_lite.x_r")

(define_insn_reservation "k1c_lite.y" 1
  (eq_attr "type" "alu_lite_y")
  "k1c_alu_lite.y_r")

(define_insn_reservation "k1c_alu" 1
  (eq_attr "type" "alu_full")
  "k1c_alu_full_r")

(define_insn_reservation "k1c_alu.x" 1
  (eq_attr "type" "alu_full_x")
  "k1c_alu_full.x_r")

(define_insn_reservation "k1c_alu.y" 1
  (eq_attr "type" "alu_full_y")
  "k1c_alu_full.y_r")

(define_insn_reservation "k1c_bcu" 1
  (eq_attr "type" "bcu")
  "k1c_bcu_r")

(define_insn_reservation "k1c_bcu_get" 1
  (eq_attr "type" "bcu_get")
  "k1c_bcu_tiny_tiny_mau_r")

(define_insn_reservation "k1c_mau" 2
  (eq_attr "type" "mau")
  "k1c_mau_r, nothing")

(define_insn_reservation "k1c_mau.x" 2
  (eq_attr "type" "mau_x")
  "k1c_mau.x_r,nothing")

(define_insn_reservation "k1c_mau.y" 2
  (eq_attr "type" "mau_y")
  "k1c_mau.y_r,nothing")

(define_insn_reservation "k1c_mau_auxr" 2
  (eq_attr "type" "mau_auxr")
  "k1c_mau_auxr_r,nothing")

(define_insn_reservation "k1c_mau_auxr.x" 2
  (eq_attr "type" "mau_auxr_x")
  "k1c_mau_auxr.x_r,nothing")

(define_insn_reservation "k1c_mau_auxr.y" 2
  (eq_attr "type" "mau_auxr_y")
  "k1c_mau_auxr.y_r,nothing")

(define_insn_reservation "k1c_mau_fpu" 4
  (eq_attr "type" "mau_fpu")
  "k1c_mau_r,nothing,nothing,nothing")

(define_insn_reservation "k1c_mau_auxr_fpu" 4
  (eq_attr "type" "mau_auxr_fpu")
  "k1c_auxr_u + k1c_mau_u + k1c_issue_r,nothing,nothing,nothing")

;; Uncached LSU

(define_insn_reservation "k1c_lsu_auxw_load_uncached" 24
  (eq_attr "type" "lsu_auxw_load_uncached")
  "k1c_lsu_auxw_r")

(define_insn_reservation "k1c_lsu_auxw_load_uncached.x" 24
  (eq_attr "type" "lsu_auxw_load_uncached_x")
  "k1c_lsu_auxw.x_r")

(define_insn_reservation "k1c_lsu_auxw_load_uncached.y" 24
  (eq_attr "type" "lsu_auxw_load_uncached_y")
  "k1c_lsu_auxw.y_r")

;; Atomic LSU

(define_insn_reservation "k1c_lsu_auxw_atomic" 24
  (eq_attr "type" "lsu_auxw_atomic")
  "k1c_lsu_auxw_r")

(define_insn_reservation "k1c_lsu_auxw_atomic.x" 24
  (eq_attr "type" "lsu_auxw_atomic_x")
  "k1c_lsu_auxw.x_r")

(define_insn_reservation "k1c_lsu_auxr_auxw_atomic" 24
  (eq_attr "type" "lsu_auxr_auxw_atomic")
  "k1c_lsu_auxr_auxw_r")

(define_insn_reservation "k1c_lsu_auxr_auxw_atomic.x" 24
  (eq_attr "type" "lsu_auxr_auxw_atomic_x")
  "k1c_lsu_auxr_auxw.x_r")

;; Cached LSU

(define_insn_reservation "k1c_lsu_auxw_load" 3
  (eq_attr "type" "lsu_auxw_load")
  "k1c_lsu_auxw_r")

(define_insn_reservation "k1c_lsu_auxw_load.x" 3
  (eq_attr "type" "lsu_auxw_load_x")
  "k1c_lsu_auxw.x_r")

(define_insn_reservation "k1c_lsu_auxw_load.y" 3
  (eq_attr "type" "lsu_auxw_load_y")
  "k1c_lsu_auxw.y_r")

;; Store LSU

(define_insn_reservation "k1c_lsu_auxr_store" 1
  (eq_attr "type" "lsu_auxr_store")
  "k1c_lsu_auxr_r")

(define_insn_reservation "k1c_lsu_auxr_store.x" 1
  (eq_attr "type" "lsu_auxr_store_x")
  "k1c_lsu_auxr.x_r")

(define_insn_reservation "k1c_lsu_auxr_store.y" 1
  (eq_attr "type" "lsu_auxr_store_y")
  "k1c_lsu_auxr.y_r")

(define_insn_reservation "k1c_lsu" 1
  (eq_attr "type" "lsu")
  "k1c_lsu_r")

(define_insn_reservation "k1c_lsu.x" 1
  (eq_attr "type" "lsu_x")
  "k1c_lsu.x_r")

(define_insn_reservation "k1c_lsu_auxr_auxw" 1
  (eq_attr "type" "lsu_auxr_auxw")
  "k1c_lsu_auxr_auxw_r")


/* The BCU reads GPRs 1 cycle earlier */
(define_bypass 2 "k1c_alu,k1c_alu.x" "k1c_bcu,k1c_bcu_get")
(define_bypass 2 "k1c_lite,k1c_lite.x,k1c_lite.y" "k1c_bcu,k1c_bcu_get")
(define_bypass 2 "k1c_tiny,k1c_tiny.x,k1c_tiny.y" "k1c_bcu,k1c_bcu_get")
(define_bypass 3 "k1c_mau,k1c_mau.x,k1c_mau_auxr" "k1c_bcu,k1c_bcu_get")
(define_bypass 5 "k1c_mau_fpu,k1c_mau_auxr_fpu" "k1c_bcu,k1c_bcu_get")
(define_bypass 4 "k1c_lsu_auxw_load,k1c_lsu_auxw_load.x" "k1c_bcu,k1c_bcu_get")

/* The stores read their input 1 cycle later */
(define_bypass 3 "k1c_mau_fpu,k1c_mau_auxr_fpu" "k1c_lsu_auxr_store,k1c_lsu_auxr_store.x,k1c_lsu_auxr_store.y")
(define_bypass 1 "k1c_mau,k1c_mau.x,k1c_mau_auxr" "k1c_lsu_auxr_store,k1c_lsu_auxr_store.x,k1c_lsu_auxr_store.y")
(define_bypass 2 "k1c_lsu_auxw_load,k1c_lsu_auxw_load.x" "k1c_lsu_auxr_store,k1c_lsu_auxr_store.x,k1c_lsu_auxr_store.y")

/* The auxr port shared by MAU and LSU allows a bypass when used as accumulator in a MAC operation. */
(define_bypass 1 "k1c_mau,k1c_mau.x" "k1c_mau_auxr" "k1_mau_lsu_double_port_bypass_p")

