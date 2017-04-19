(automata_option "ndfa")
(automata_option "v")

(define_automaton "coolidge")
(define_automaton "coolidge_issue")

(define_cpu_unit
  "k1c_alu0_u, k1c_tiny_alu0_u, k1c_lite_alu0_u,
   k1c_alu1_u, k1c_tiny_alu1_u, k1c_lite_alu1_u,
   k1c_bcu_u,
   k1c_mau_u, k1c_tiny_mau_u, k1c_lite_mau_u,
   k1c_lsu_u, k1c_tiny_lsu_u, k1c_lsu_store_u,
   k1c_odd_u"
   "coolidge")

(define_cpu_unit
  "k1c_issue0, k1c_issue1, k1c_issue2, k1c_issue3, k1c_issue4, k1c_issue5, k1c_issue6, k1c_issue7"
  "coolidge_issue")

(absence_set "k1c_alu0_u"      "k1c_tiny_alu0_u, k1c_lite_alu0_u")
(absence_set "k1c_tiny_alu0_u" "k1c_alu0_u, k1c_lite_alu0_u")
(absence_set "k1c_lite_alu0_u" "k1c_tiny_alu0_u, k1c_alu0_u")

(absence_set "k1c_alu1_u"      "k1c_tiny_alu1_u, k1c_lite_alu1_u")
(absence_set "k1c_tiny_alu1_u" "k1c_alu1_u, k1c_lite_alu1_u")
(absence_set "k1c_lite_alu1_u" "k1c_tiny_alu1_u, k1c_alu1_u")

(absence_set "k1c_mau_u"      "k1c_tiny_mau_u, k1c_lite_mau_u")
(absence_set "k1c_tiny_mau_u" "k1c_mau_u, k1c_lite_mau_u")
(absence_set "k1c_lite_mau_u" "k1c_tiny_mau_u, k1c_mau_u")

(absence_set "k1c_lite_mau_u" "k1c_tiny_mau_u, k1c_mau_u")


;; k1c_lsu_store_u, k1c_tiny_lsu_u

;; ALU > LITE > TINY

;;(define_reservation "k1c_lite_alu_r" "(k1c_alu0_u + k1c_alu1_u)")

;; (define_reservation "k1c_lite_mau_r" "(k1c_mau_u)")
;; (define_reservation "k1c_lite_lsu_r" "(k1c_lsu_u)")

;; (define_reservation "k1c_tiny_alu0_r" "(k1c_alu0_u)")
;; (define_reservation "k1c_tiny_alu1_r" "(k1c_alu1_u)")
;; (define_reservation "k1c_tiny_mau_r" "(k1c_mau_u)")
;; (define_reservation "k1c_tiny_lsu_r" "(k1c_lsu_u)")

;; (define_reservation "k1c_lsu_store_r" "k1c_acc_u")

(define_reservation "k1c_alu_r" "(k1c_alu0_u | k1c_alu1_u)")

;;(define_reservation "k1c_lite_r" "( k1c_lite_mau_r | k1c_lite_lsu_r | k1c_lite_alu_r)")

(define_reservation "k1c_lite_simple_r" "( k1c_lite_mau_u | k1c_lite_alu0_u | k1c_lite_alu1_u)")
(define_reservation "k1c_lite_double_r" "( k1c_lite_mau_u | (k1c_lite_alu0_u + k1c_lite_alu1_u))")

;;(define_reservation "k1c_tiny_r" "(k1c_tiny_alu0_r | k1c_tiny_alu1_r | k1c_tiny_lsu_r | k1c_tiny_mau_r)")

(define_reservation "k1c_tiny_simple_r" "(k1c_tiny_alu0_u | k1c_tiny_alu1_u | k1c_tiny_lsu_u | k1c_tiny_mau_u)")
(define_reservation "k1c_tiny_double_r" "((k1c_tiny_alu0_u + k1c_tiny_alu1_u) | k1c_tiny_lsu_u | k1c_tiny_mau_u)")

(define_reservation "k1c_issue_r" "(k1c_issue0|k1c_issue1|k1c_issue2|k1c_issue3|k1c_issue4|k1c_issue5|k1c_issue6|k1c_issue7)")
(define_reservation "k1c_issue_x2_r" "(k1c_issue0+k1c_issue1)|(k1c_issue1+k1c_issue2)|(k1c_issue2+k1c_issue3)|(k1c_issue3+k1c_issue4)|(k1c_issue4+k1c_issue5)|(k1c_issue5+k1c_issue6)|(k1c_issue6+k1c_issue7)")
(define_reservation "k1c_issue_x3_r" "(k1c_issue0+k1c_issue1+k1c_issue2)|(k1c_issue1+k1c_issue2+k1c_issue3)|(k1c_issue2+k1c_issue3+k1c_issue4)|(k1c_issue3+k1c_issue4+k1c_issue5)|(k1c_issue4+k1c_issue5+k1c_issue6)|(k1c_issue5+k1c_issue6+k1c_issue7)")
(define_reservation "k1c_issue_x4_r" "(k1c_issue0+k1c_issue1+k1c_issue2+k1c_issue3)|(k1c_issue1+k1c_issue2+k1c_issue3+k1c_issue4)|(k1c_issue2+k1c_issue3+k1c_issue4+k1c_issue5)|(k1c_issue3+k1c_issue4+k1c_issue5+k1c_issue6)|(k1c_issue4+k1c_issue5+k1c_issue6+k1c_issue7)")

;; (define_reservation "k1c_tiny" "(k1c_tiny_alu0|k1c_tiny_alu1|k1c_tiny_mau|k1c_tiny_lsu)")
;; (define_reservation "k1c_lite" "(k1c_lite_alu|k1c_lite_mau|k1c_lite_lsu)")

(define_reservation "k1c_all_r"
 "(k1c_issue0 + k1c_issue1 + k1c_issue2 + k1c_issue3 + k1c_issue4 + k1c_issue5 + k1c_issue6 + k1c_issue7)")

;; "(k1c_issue_r + k1c_issue_r + k1c_issue_r + k1c_issue_r + k1c_issue_r + k1c_issue_r + k1c_issue_r + k1c_issue_r)")

(define_reservation "k1c_alud_full_r"
 "(k1c_alu0_u + k1c_alu1_u) + k1c_issue_r")

(define_reservation "k1c_alud_full.x_r"
 "(k1c_alu0_u + k1c_alu1_u) + k1c_issue_x2_r")

(define_reservation "k1c_alud_full.y_r"
 "k1c_alu0_u + k1c_alu1_u + k1c_issue_x3_r")

(define_reservation "k1c_alud_full.z_r"
 "k1c_alu0_u + k1c_alu1_u + k1c_issue_x4_r")

;; Should be used when addcd/sbfcd/addcid/sbfcid are introduced with monodouble fmts
(define_reservation "k1c_alud_full_odd_r"
    "k1c_alu0_u + k1c_alu1_u + k1c_odd_u + k1c_issue_r")

(define_reservation "k1c_alud_full_odd.x_r"
    "k1c_alu0_u + k1c_alu1_u + k1c_odd_u + k1c_issue_x2_r")

(define_reservation "k1c_alud_lite_r"
   "(k1c_lite_double_r + k1c_issue_r)")

(define_reservation "k1c_alud_lite.x_r"
   "(k1c_lite_double_r | k1c_mau_u) + k1c_issue_x2_r")

(define_reservation "k1c_alud_tiny_r"
   "k1c_tiny_double_r + k1c_issue_r")

(define_reservation "k1c_alud_tiny.x_r"
   "k1c_tiny_double_r + k1c_issue_x2_r")

(define_reservation "k1c_alu_full_r"
  "k1c_alu_r + k1c_issue_r")

(define_reservation "k1c_alu_full.x_r"
  "k1c_alu_r + k1c_issue_x2_r")

(define_reservation "k1c_alu_full_odd_r"
  "k1c_alu0_u + k1c_odd_u + k1c_issue_r")

(define_reservation "k1c_alu_lite_r"
  "k1c_lite_simple_r + k1c_issue_r")

(define_reservation "k1c_alu_lite.x_r"
  "k1c_lite_simple_r + k1c_issue_x2_r")

(define_reservation "k1c_alu_tiny_r"
  "k1c_tiny_simple_r + k1c_issue_r")

(define_reservation "k1c_alu_tiny.x_r"
  "k1c_tiny_simple_r + k1c_issue_x2_r")

(define_reservation "k1c_bcu_r"
  "k1c_bcu_u + k1c_issue_r")

;; fake reservation for tiny_lsu : EXU is not really used but adding 3 tinys would make the
;; the dispatcher try to use MAU tiny as lsu's not used.
(define_reservation "k1c_bcu_tiny_tiny_mau_r"
  "k1c_mau_u + k1c_bcu_u + k1c_tiny_lsu_u + k1c_issue_r")

(define_reservation "k1c_lsu_r"
  "k1c_lsu_u + k1c_tiny_lsu_u + k1c_issue_r")

(define_reservation "k1c_lsu.x_r"
  "k1c_lsu_u + k1c_tiny_lsu_u + k1c_issue_x2_r")

(define_reservation "k1c_lsu_acc_r"
  "k1c_lsu_u + k1c_tiny_lsu_u + k1c_lsu_store_u + k1c_issue_r")

(define_reservation "k1c_lsu_acc.x_r"
  "k1c_lsu_u + k1c_tiny_lsu_u + k1c_lsu_store_u + k1c_issue_x2_r")

(define_reservation "k1c_mau_r"
  "k1c_mau_u + k1c_issue_r")

(define_reservation "k1c_mau.x_r"
  "k1c_mau_u + k1c_issue_x2_r")

(define_reservation "k1c_mau_acc_r"
  "k1c_mau_u + k1c_lsu_store_u + k1c_issue_r")

(define_reservation "k1c_mau_acc.x_r"
  "k1c_mau_u + k1c_lsu_store_u + k1c_issue_x2_r")

(define_reservation "k1c_mau_acc_odd_r"
   "k1c_mau_u + k1c_lsu_store_u + k1c_issue_r")

(define_insn_reservation "k1c_all" 1 (and (eq_attr "arch" "coolidge")
                                          (eq_attr "type" "all"))
                         "k1c_all_r")


(define_insn_reservation "k1c_tiny_double" 1 (and (eq_attr "arch" "coolidge")
                                                  (eq_attr "type" "alud_tiny"))
                         "k1c_alud_tiny_r")

(define_insn_reservation "k1c_tiny_double.x" 1 (and (eq_attr "arch" "coolidge")
                                                    (eq_attr "type" "alud_tiny_x"))
                         "k1c_alud_tiny.x_r")


(define_insn_reservation "k1c_tiny" 1 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "tiny,sbfhp,maxuhp,\
                                                   minuhp,maxhp,minhp,comphp,addhp,\
                                                   and_Ilh"))
                         "k1c_alu_tiny_r")

(define_insn_reservation "k1c_tiny.x" 1 (and (eq_attr "arch" "coolidge")
                                             (eq_attr "type" "tiny_x"))
                         "k1c_alu_tiny.x_r")


(define_insn_reservation "k1c_lite_double" 1 (and (eq_attr "arch" "coolidge")
                                                  (eq_attr "type" "alud_lite,addshp"))
                         "k1c_alud_lite_r")

(define_insn_reservation "k1c_lite_double.x" 1 (and (eq_attr "arch" "coolidge")
                                                    (eq_attr "type" "alud_lite_x"))
                         "k1c_alud_lite.x_r")


(define_insn_reservation "k1c_lite" 1 (and (eq_attr "arch" "coolidge")
                 (eq_attr "type" "lite, \
                      shift32,abdhp,abd,abs, \
                      cmovehp,landhp,extfs,cbs, \
                      sat,ctz,adds,insf,cmove, \
                      stsu,clz,extfz"))
                 "k1c_alu_lite_r")

(define_insn_reservation "k1c_lite.x" 1 (and (eq_attr "arch" "coolidge")
                                             (eq_attr "type" "abd_x,cmove_x,bwluhp,lite_x"))
                         "k1c_alu_lite.x_r")

;; (define_insn_reservation "k1c_alu0" 1 (and (eq_attr "arch" "coolidge")
;;                                            (eq_attr "type" "alu0"))
;;                          "k1c_alu0_u + k1c_issue_r")

(define_insn_reservation "k1c_alu" 1 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "alu"))
                         "k1c_alu_full_r")

(define_insn_reservation "k1c_alu_full_odd" 1 (and (eq_attr "arch" "coolidge")
                                                   (eq_attr "type" "alu_full_odd"))
                         "k1c_alu_full_odd_r")

(define_insn_reservation "k1c_alu.x" 1 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "alu_x"))
                         "k1c_alu_full.x_r")

(define_insn_reservation "k1c_alud" 1 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "alud"))
                         "k1c_alud_full_r")

(define_insn_reservation "k1c_alud_full_odd" 1 (and (eq_attr "arch" "coolidge")
                                                    (eq_attr "type" "alud_full_odd"))
                         "k1c_alud_full_odd_r")

(define_insn_reservation "k1c_alud_full_odd.x" 1 (and (eq_attr "arch" "coolidge")
                                                      (eq_attr "type" "alud_full_odd_x"))
                         "k1c_alud_full_odd.x_r")

(define_insn_reservation "k1c_alud.x" 1 (and (eq_attr "arch" "coolidge")
                                             (eq_attr "type" "alud_x"))
                         "k1c_alud_full.x_r")

(define_insn_reservation "k1c_alud.y" 1 (and (eq_attr "arch" "coolidge")
                                             (eq_attr "type" "alud_y"))
                         "k1c_alud_full.y_r")

(define_insn_reservation "k1c_alud.z" 1 (and (eq_attr "arch" "coolidge")
                                             (eq_attr "type" "alud_z"))
                         "k1c_alud_full.z_r")

(define_insn_reservation "k1c_bcu" 1 (and (eq_attr "arch" "coolidge")
                                          (eq_attr "type" "bcu"))
                         "k1c_bcu_r")

(define_insn_reservation "k1c_bcu_get" 1 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "bcu_get"))
                         "k1c_bcu_tiny_tiny_mau_r")

(define_insn_reservation "k1c_mau" 2 (and (eq_attr "arch" "coolidge")
                                          (eq_attr "type" "mau"))
                         "k1c_mau_r, nothing")

(define_insn_reservation "k1c_mau.x" 2 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "mau_x"))
                         "k1c_mau.x_r,nothing")

(define_insn_reservation "k1c_mau_lsu" 2 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "mau_lsu"))
                         "k1c_lsu_store_u + k1c_mau_u + k1c_issue_r,nothing")

(define_insn_reservation "k1c_mau_lsu.x" 2 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "mau_lsu_x"))
                         "k1c_lsu_store_u + k1c_mau_r + k1c_issue_x2_r,nothing")

(define_insn_reservation "k1c_mau_acc" 1 (and (eq_attr "arch" "coolidge")
                                              (eq_attr "type" "mau_acc"))
                         "k1c_mau_acc_r,nothing")

(define_insn_reservation "k1c_mau_acc_odd" 1 (and (eq_attr "arch" "coolidge")
                                                  (eq_attr "type" "mau_acc_odd"))
                         "k1c_mau_acc_odd_r,nothing")

(define_insn_reservation "k1c_acc.x" 2 (and (eq_attr "arch" "coolidge")
                                            (eq_attr "type" "mau_acc_x"))
                         "k1c_mau_acc.x_r,nothing")

;; Bug workaround : insn issuing a FPU must not be bundled with store insn
(define_insn_reservation "k1c_mau_fpu" 4 (and (eq_attr "arch" "coolidge")
                                         (eq_attr "type" "mau_fpu,fmuld,fmulnd,fmulnrn,faddd,fsbfd"))
                         "k1c_mau_r + k1c_lsu_store_u,nothing,nothing,nothing")

(define_insn_reservation "k1c_mau_lsu_fpu" 4 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "mau_lsu_fpu"))
                         "k1c_lsu_store_u + k1c_mau_u + k1c_issue_r,nothing,nothing,nothing")

;; Uncached LSU

;; atomic uncached insn block all the pipeline : a*u insn
(define_insn_reservation "k1c_lsu_atomic_uncached" 10 (and (eq_attr "arch" "coolidge")
                                                          (eq_attr "type" "lsu_atomic_uncached"))
                         "k1c_lsu_acc_r")

(define_insn_reservation "k1c_lsu_atomic_uncached.x" 10 (and (eq_attr "arch" "coolidge")
                                                            (eq_attr "type" "lsu_atomic_uncached_x"))
                         "k1c_lsu_acc.x_r")


(define_insn_reservation "k1c_lsu_load_uncached" 10 (and (eq_attr "arch" "coolidge")
                                                         (and (eq_attr "type" "lsu_load_uncached")
                                                              (match_test "TARGET_K1PE")))
                         "k1c_lsu_r")

(define_insn_reservation "k1c_lsu_load_uncached.x" 10 (and (eq_attr "arch" "coolidge")
                                                          (and (eq_attr "type" "lsu_load_uncached_x")
                                                               (match_test "TARGET_K1PE")))
                         "k1c_lsu.x_r")

(define_insn_reservation "k1c_io_lsu_load_uncached" 10 (and (eq_attr "arch" "coolidge")
                                                           (and (eq_attr "type" "lsu_load_uncached")
                                                                (match_test "TARGET_K1RM")))
                         "k1c_lsu_r")

(define_insn_reservation "k1c_io_lsu_load_uncached.x" 10 (and (eq_attr "arch" "coolidge")
                                                             (and (eq_attr "type" "lsu_load_uncached_x")
                                                                  (match_test "TARGET_K1RM")))
                         "k1c_lsu.x_r")

(define_insn_reservation "k1c_lsu_store_uncached" 1 (and (eq_attr "arch" "coolidge")
                                                         (eq_attr "type" "lsu_store_uncached"))
                         "k1c_lsu_u + k1c_tiny_lsu_u + k1c_lsu_store_u + k1c_issue_r")

(define_insn_reservation "k1c_lsu_store_uncached.x" 1 (and (eq_attr "arch" "coolidge")
                                                           (eq_attr "type" "lsu_store_uncached_x"))
                         "k1c_lsu_u + k1c_tiny_lsu_u + k1c_lsu_store_u + k1c_issue_x2_r")


;; Cached LSU

(define_insn_reservation "k1c_lsu_atomic" 2 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "lsu_atomic"))
                         "k1c_lsu_acc_r")

(define_insn_reservation "k1c_lsu_atomic.x" 2 (and (eq_attr "arch" "coolidge")
                                           (eq_attr "type" "lsu_atomic_x"))
                         "k1c_lsu_acc.x_r")

(define_insn_reservation "k1c_lsu_load" 2 (and (eq_attr "arch" "coolidge")
                                               (and (eq_attr "type" "lsu_load")
                                                    (match_test "TARGET_K1PE")))
                         "k1c_lsu_r")

(define_insn_reservation "k1c_lsu_load.x" 2 (and (eq_attr "arch" "coolidge")
                                                 (and (eq_attr "type" "lsu_load_x")
                                                      (match_test "TARGET_K1PE")))
                         "k1c_lsu.x_r")

(define_insn_reservation "k1c_io_lsu_load" 3 (and (eq_attr "arch" "coolidge")
                                                  (and (eq_attr "type" "lsu_load")
                                                       (match_test "TARGET_K1RM")))
                         "k1c_lsu_r")

(define_insn_reservation "k1c_io_lsu_load.x" 3 (and (eq_attr "arch" "coolidge")
                                                    (and (eq_attr "type" "lsu_load_x")
                                                         (match_test "TARGET_K1RM")))
                         "k1c_lsu.x_r")

;; Cached and Uncached LSU

(define_insn_reservation "k1c_lsu_store" 1 (and (eq_attr "arch" "coolidge")
                                                (eq_attr "type" "lsu_store"))
                         "k1c_lsu_u + k1c_tiny_lsu_u + k1c_lsu_store_u + k1c_issue_r")

(define_insn_reservation "k1c_lsu_store.x" 1 (and (eq_attr "arch" "coolidge")
                                                  (eq_attr "type" "lsu_store_x"))
                         "k1c_lsu_u + k1c_tiny_lsu_u + k1c_lsu_store_u + k1c_issue_x2_r")

(define_insn_reservation "k1c_lsu" 1 (and (eq_attr "arch" "coolidge")
                                          (eq_attr "type" "lsu"))
                         "k1c_lsu_r")

(define_insn_reservation "k1c_lsu.x" 1 (and (eq_attr "arch" "coolidge")
                                            (eq_attr "type" "lsu_x"))
                         "k1c_lsu.x_r")

/* The bcus read their input 1 cycle earlier */
(define_bypass 2 "k1c_alu,k1c_alu.x" "k1c_bcu,k1c_bcu_get")
(define_bypass 2 "k1c_alud,k1c_alud.x,k1c_alud.y,k1c_alud.z" "k1c_bcu,k1c_bcu_get")
(define_bypass 2 "k1c_lite,k1c_lite.x" "k1c_bcu,k1c_bcu_get")
(define_bypass 2 "k1c_tiny,k1c_tiny.x,k1c_tiny_double,k1c_tiny_double.x" "k1c_bcu,k1c_bcu_get")
(define_bypass 2 "k1c_lite_double,k1c_lite_double.x" "k1c_bcu,k1c_bcu_get")

(define_bypass 5 "k1c_mau_fpu,k1c_mau_lsu_fpu" "k1c_bcu,k1c_bcu_get")
(define_bypass 3 "k1c_mau,k1c_mau.x,k1c_mau_lsu" "k1c_bcu,k1c_bcu_get")
(define_bypass 4 "k1c_lsu_atomic,k1c_lsu_atomic.x" "k1c_bcu,k1c_bcu_get")
(define_bypass 3 "k1c_lsu_load,k1c_lsu_load.x" "k1c_bcu,k1c_bcu_get")
(define_bypass 4 "k1c_io_lsu_load,k1c_io_lsu_load.x" "k1c_bcu,k1c_bcu_get")

/* The stores read their input 1 cycle later */
(define_bypass 3 "k1c_mau_fpu,k1c_mau_lsu_fpu" "k1c_lsu_store,k1c_lsu_store.x")
(define_bypass 1 "k1c_mau,k1c_mau.x,k1c_mau_lsu" "k1c_lsu_store,k1c_lsu_store.x")
(define_bypass 2 "k1c_lsu_atomic,k1c_lsu_atomic.x" "k1c_lsu_store,k1c_lsu_store.x")
(define_bypass 1 "k1c_lsu_load,k1c_lsu_load.x" "k1c_lsu_store,k1c_lsu_store.x")
(define_bypass 2 "k1c_io_lsu_load,k1c_io_lsu_load.x" "k1c_lsu_store,k1c_lsu_store.x")

/* The double port shared by MAU and LSU allows a bypass when it is 
   used as accumulator in a MAC operation. */
(define_bypass 1 "k1c_mau,k1c_mau.x" "k1c_mau_lsu" "k1_mau_lsu_double_port_bypass_p")
