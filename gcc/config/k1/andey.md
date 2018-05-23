(automata_option "ndfa")
(automata_option "v")
(define_automaton "andey")

(define_cpu_unit
  "ALU0_U, TINY_ALU0_U, ALU1_U, TINY_ALU1_U, BCU_U, MAU_U, TINY_MAU_U, LSU_U, LSU_STORE_U, ODD_U, TINY_LSU_U, ISSUE0, ISSUE1, ISSUE2, ISSUE3, ISSUE4, ISSUE5, ISSUE6, ISSUE7"
  "andey")

(define_reservation "TINY_ALU0" "(ALU0_U+TINY_ALU0_U)")
(define_reservation "TINY_ALU1" "(ALU1_U+TINY_ALU1_U)")
(define_reservation "TINY_MAU" "(MAU_U+TINY_MAU_U)")
(define_reservation "TINY_LSU" "(LSU_U+TINY_LSU_U)")

(define_reservation "ISSUE" "(ISSUE0|ISSUE1|ISSUE2|ISSUE3|ISSUE4|ISSUE5|ISSUE6|ISSUE7)")
(define_reservation "ISSUE_x2" "(ISSUE0+ISSUE1)|(ISSUE1+ISSUE2)|(ISSUE2+ISSUE3)|(ISSUE3+ISSUE4)|(ISSUE4+ISSUE5)|(ISSUE5+ISSUE6)|(ISSUE6+ISSUE7)")
(define_reservation "ISSUE_x3" "(ISSUE0+ISSUE1+ISSUE2)|(ISSUE1+ISSUE2+ISSUE3)|(ISSUE2+ISSUE3+ISSUE4)|(ISSUE3+ISSUE4+ISSUE5)|(ISSUE4+ISSUE5+ISSUE6)|(ISSUE5+ISSUE6+ISSUE7)")
(define_reservation "ISSUE_x4" "(ISSUE0+ISSUE1+ISSUE2+ISSUE3)|(ISSUE1+ISSUE2+ISSUE3+ISSUE4)|(ISSUE2+ISSUE3+ISSUE4+ISSUE5)|(ISSUE3+ISSUE4+ISSUE5+ISSUE6)|(ISSUE4+ISSUE5+ISSUE6+ISSUE7)")

(define_reservation "K1A_TINY" "(TINY_ALU0|TINY_ALU1|TINY_MAU|TINY_LSU)")


(define_insn_reservation "all" 1 (and (eq_attr "arch" "andey")
                                      (eq_attr "type" "all"))
                         "ALU0_U+ALU1_U+MAU_U+LSU_U+BCU_U+ISSUE")

(define_insn_reservation "tiny" 1  (and (eq_attr "arch" "andey")
                                        (eq_attr "type" "tiny"))
                         "K1A_TINY+ISSUE")

(define_insn_reservation "tiny.x" 1  (and (eq_attr "arch" "andey")
                                          (eq_attr "type" "tiny_x"))
                         "K1A_TINY+ISSUE_x2")

(define_insn_reservation "alu0" 1  (and (eq_attr "arch" "andey")
                                        (eq_attr "type" "alu0"))
                         "ALU0_U+ISSUE")

(define_insn_reservation "alu" 1  (and (eq_attr "arch" "andey")
                                       (eq_attr "type" "alu,extfz,extfs,shift32,\
                                           clz,abdhp,abd,sbfhp,addhp,sat,\
                                           adds,maxuhp,minuhp,maxhp,minhp,comphp,\
                                           stsu,cmovehp,addshp,landhp,cmove,cbs,\
                                           insf,ctz,abs,and_Ilh"))
                         "(ALU0_U|ALU1_U)+ISSUE")

(define_insn_reservation "alu.x" 1  (and (eq_attr "arch" "andey")
                                         (eq_attr "type" "abd_x,cmove_x,bwluhp,alu_x"))
                         "(ALU0_U|ALU1_U)+ISSUE_x2")

(define_insn_reservation "alud" 1  (and (eq_attr "arch" "andey")
                                        (eq_attr "type" "alud_x"))
                         "(ALU0_U+ALU1_U)+ISSUE_x2")

(define_insn_reservation "alud.y" 1  (and (eq_attr "arch" "andey")
                                          (eq_attr "type" "alud_y"))
                         "ALU0_U+ALU1_U+ISSUE_x3")

(define_insn_reservation "alud.z" 1  (and (eq_attr "arch" "andey")
                                          (eq_attr "type" "alud_z"))
                         "ALU0_U+ALU1_U+ISSUE_x4")

(define_insn_reservation "k1a_alu_full_odd" 1  (and (eq_attr "arch" "andey")
                                               (eq_attr "type" "alu_full_odd"))
                         "ALU0_U+ODD_U+ISSUE")

(define_insn_reservation "k1a_alud_full_odd" 1  (and (eq_attr "arch" "andey")
                                                (eq_attr "type" "alud_full_odd"))
                         "ALU0_U+ALU1_U+ODD_U+ISSUE")

(define_insn_reservation "k1a_alud_full_odd.x" 1  (and (eq_attr "arch" "andey")
                                                       (eq_attr "type" "alud_full_odd_x"))
                         "ALU0_U+ALU1_U+ODD_U+ISSUE+ISSUE")


(define_insn_reservation "bcu" 1  (and (eq_attr "arch" "andey")
                                       (eq_attr "type" "bcu"))
                         "BCU_U+ISSUE")

(define_insn_reservation "bcu_get" 1  (and (eq_attr "arch" "andey")
                                           (eq_attr "type" "bcu_get"))
                         "BCU_U+MAU_U+TINY_MAU+TINY_LSU+ISSUE")

(define_insn_reservation "mau" 2  (and (eq_attr "arch" "andey")
                                       (eq_attr "type" "mau"))
                         "MAU_U+ISSUE,nothing")

(define_insn_reservation "mau.x" 2  (and (eq_attr "arch" "andey")
                                         (eq_attr "type" "mau_x"))
                         "MAU_U+ISSUE_x2,nothing")

(define_insn_reservation "mau_lsu" 2  (and (eq_attr "arch" "andey")
                                           (eq_attr "type" "mau_lsu"))
                         "LSU_STORE_U+MAU_U+ISSUE,nothing")

(define_insn_reservation "mau_lsu.x" 2  (and (eq_attr "arch" "andey")
                                             (eq_attr "type" "mau_lsu_x"))
                         "LSU_STORE_U+MAU_U+ISSUE_x2,nothing")

;; duplicate rule mau_lsu[.x] for mau_acc[.x]
(define_insn_reservation "mau_acc" 1  (and (eq_attr "arch" "andey")
                                           (eq_attr "type" "faddd,fsbfd,fmuld,mau_acc"))
                         "LSU_STORE_U+MAU_U+ISSUE")
(define_insn_reservation "mau_acc.x" 2  (and (eq_attr "arch" "andey")
                                             (eq_attr "type" "mau_acc_x"))
                         "LSU_STORE_U+MAU_U+ISSUE_x2,nothing")

(define_insn_reservation "mau_acc_odd" 1  (and (eq_attr "arch" "andey")
                                               (eq_attr "type" "mau_acc_odd"))
                         "LSU_STORE_U+ODD_U+MAU_U+ISSUE")

(define_insn_reservation "mau_fpu" 4  (and (eq_attr "arch" "andey")
                                           (eq_attr "type" "mau_fpu"))
                         "MAU_U+ISSUE,nothing,nothing,nothing")

(define_insn_reservation "mau_lsu_fpu" 4  (and (eq_attr "arch" "andey")
                                               (eq_attr "type" "mau_lsu_fpu"))
                         "LSU_STORE_U+MAU_U+ISSUE,nothing,nothing,nothing")

(define_insn_reservation "lsu_atomic" 3  (and (eq_attr "arch" "andey")
                                              (eq_attr "type" "lsu_atomic"))
                         "LSU_U+LSU_STORE_U+ISSUE")

(define_insn_reservation "lsu_atomic.x" 3  (and (eq_attr "arch" "andey")
                                                (eq_attr "type" "lsu_atomic_x"))
                         "LSU_U+LSU_STORE_U+ISSUE_x2")

(define_insn_reservation "lsu_load" 2  (and (eq_attr "arch" "andey")
                                            (and (eq_attr "type" "lsu_load")
                                                 (match_test "!TARGET_K1IO")))
                         "LSU_U+ISSUE,nothing")

(define_insn_reservation "lsu_load.x" 2  (and (eq_attr "arch" "andey")
                                              (and (eq_attr "type" "lsu_load_x")
                                                   (match_test "!TARGET_K1IO")))
                         "LSU_U+ISSUE_x2,nothing,nothing")

(define_insn_reservation "io_lsu_load" 3  (and (eq_attr "arch" "andey")
                                               (and (eq_attr "type" "lsu_load")
                                                    (match_test "TARGET_K1IO")))
                         "LSU_U+ISSUE,nothing")

(define_insn_reservation "io_lsu_load.x" 3  (and (eq_attr "arch" "andey")
                                                 (and (eq_attr "type" "lsu_load_x")
                                                      (match_test "TARGET_K1IO")))
                         "LSU_U+ISSUE_x2,nothing,nothing")

(define_insn_reservation "lsu_store" 1  (and (eq_attr "arch" "andey")
                                             (eq_attr "type" "lsu_store"))
                         "LSU_U+LSU_STORE_U+ISSUE")

(define_insn_reservation "lsu_store.x" 1  (and (eq_attr "arch" "andey")
                                               (eq_attr "type" "lsu_store_x"))
                         "LSU_U+LSU_STORE_U+ISSUE_x2")

(define_insn_reservation "lsu" 1  (and (eq_attr "arch" "andey")
                                       (eq_attr "type" "lsu"))
                         "LSU_U+ISSUE")

(define_insn_reservation "lsu.x" 1  (and (eq_attr "arch" "andey")
                                         (eq_attr "type" "lsu_x"))
                         "LSU_U+ISSUE_x2")

/* The bcus read their input 1 cycle earlier */
(define_bypass 2 "alu,alu.x,alud,alud.y,alud.z,tiny,tiny.x" "bcu,bcu_get")
(define_bypass 5 "mau_fpu,mau_lsu_fpu" "bcu,bcu_get")
(define_bypass 3 "mau,mau.x,mau_lsu" "bcu,bcu_get")
(define_bypass 4 "lsu_atomic,lsu_atomic.x" "bcu,bcu_get")
(define_bypass 3 "lsu_load,lsu_load.x" "bcu,bcu_get")
(define_bypass 4 "io_lsu_load,io_lsu_load.x" "bcu,bcu_get")

/* The stores read their input 1 cycle later */
(define_bypass 3 "mau_fpu,mau_lsu_fpu" "lsu_store,lsu_store.x")
(define_bypass 1 "mau,mau.x,mau_lsu" "lsu_store,lsu_store.x")
(define_bypass 2 "lsu_atomic,lsu_atomic.x" "lsu_store,lsu_store.x")
(define_bypass 1 "lsu_load,lsu_load.x" "lsu_store,lsu_store.x")
(define_bypass 2 "io_lsu_load,io_lsu_load.x" "lsu_store,lsu_store.x")

/* The double port shared by MAU and LSU allows a bypass when it is 
   used as accumulator in a MAC operation. */
(define_bypass 1 "mau,mau.x" "mau_lsu" "k1_mau_lsu_double_port_bypass_p")
