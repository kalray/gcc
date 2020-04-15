.section .text

.macro get_got_addr reg var
  #ifdef __KVX_FDPIC__
  addd \reg = $r14, @got(\var)
  ;;
  ld \reg = 0[\reg]
  ;;
  #else
  make \reg = \var
  #endif
.endm

.global do_test
do_test:
  get $r8 = $ra
  addd $r12 = $r12, -8
  ;;
  sd 16[$r12] = $r8
  ;;

  // prepare_test_regs
  get_got_addr $r40 saved_regs
  ;;
  sd 0[$r40] = $r0
  get_got_addr $r1 ret_val
  ;;
  make $r43 = 0xFFFFFFFFFFFFFFFF
  ;;
  sd 0[$r1] = $r43
  ;;
  sd 8[$r1] = $r43
  ;;
  sd 8[$r40] = $r1
  ;;
  sd 16[$r40] = $r8 // ra
  ;;
  sd 24[$r40] = $r12
  ;;
  // execute test
  igoto $r0
  ;;
ret_from_test:

  goto verify_test_regs
  ;;
ret_verify_test_regs:

  ld $r8 = 16[$r12]
  addd $r12 = $r12, 8
  ;;
  set $ra = $r8
  ;;
  ret
  ;;

.global no_bcu_test
.global bpbcu_no_bcu_test
no_bcu_test:
bpbcu_no_bcu_test:
  make $r40 = 45
  ;;
  get_got_addr $r42 ret_val
  ;;
  get $r43 = $ra
  ;;
  sd 0[$r42] = $r43
  ;;
  goto ret_from_test
  ;;

.global call_test
.global ra_call_test
.global bpbcu_call_test
call_test:
bpbcu_call_test:
  call label_call_test
  ;;
ra_call_test:
  goto ret_from_test
  ;;
label_call_test:
  ret
  ;;

.global goto_test
.global bpbcu_goto_test
goto_test:
bpbcu_goto_test:
  goto label_goto_test
  copyd $r42 = $r43
  ;;
label_goto_test:
  goto ret_from_test
  ;;

.global cb_taken_test
.global bpbcu_cb_taken_test
cb_taken_test:
  make $r41 = 0
  ;;
bpbcu_cb_taken_test:
  cb.deqz $r41, label_cb_taken_test
  ;;
  copyd $r42 = $r43
  ;;
label_cb_taken_test:
  goto ret_from_test
  ;;

.global cb_skip_test
.global bpbcu_cb_skip_test
cb_skip_test:
  make $r41 = 1
  ;;
bpbcu_cb_skip_test:
  cb.deqz $r41, label_cb_skip_test
  ;;
  copyd $r42 = $r43
  ;;
label_cb_skip_test:
  goto ret_from_test
  ;;

.global icall_test
.global ra_icall_test
.global bpbcu_icall_test
icall_test:
  get_got_addr $r41 label_icall_test
  ;;
bpbcu_icall_test:
  icall $r41
  copyd $r42 = $r43
  make $r41 = 0
  ;;
ra_icall_test:
  copyd $r41 = $r42
  ;;
  goto ret_from_test
  ;;
label_icall_test:
  ret
  ;;

.global igoto_test
.global bpbcu_igoto_test
igoto_test:
  get_got_addr $r41 label_igoto_test
  ;;
bpbcu_igoto_test:
  igoto $r41
  ;;
  copyd $r42 = $r43
  ;;
label_igoto_test:
  goto ret_from_test
  ;;

.global ret_test
.global ra_ret_test
.global bpbcu_ret_test
ret_test:
  call label_ret_test
  ;;
ra_ret_test:
  goto ret_from_test
  ;;
bpbcu_ret_test:
label_ret_test:
  ret
  ;;

.global loopdo_test
.global bpbcu_loopdo_test_1
.global bpbcu_loopdo_test_2
loopdo_test:
  make $r40 = 10
  make $r41 = 20
  ;;
bpbcu_loopdo_test_1:
  loopdo $r40, label_loopdo_test
  ;;
  addd $r41 = $r41, 1
  ;;
label_loopdo_test:
bpbcu_loopdo_test_2:
  get_got_addr $r42 ret_val
  ;;
  sd 0[$r42], $r41
  ;;
  goto ret_from_test
  ;;

.global get_test
.global bpbcu_getra_test
.global bpbcu_getpc_test
get_test:
bpbcu_getra_test:
  get $r40 = $ra
  ;;
  get_got_addr $r42 ret_val
  ;;
bpbcu_getpc_test:
  get $r41 = $pc
  ;;
  sd 0[$r42] = $r40
  ;;
  sd 8[$r42] = $r41
  ;;
  goto ret_from_test
  ;;

.global iget_test
.global bpbcu_igetra_test
.global bpbcu_igetpc_test
iget_test:
  make $r40 = 3
  ;;
bpbcu_igetra_test:
  iget $r40
  ;;
  make $r41 = 0
  ;;
bpbcu_igetpc_test:
  iget $r41
  get_got_addr $r42 ret_val
  ;;
  sd 0[$r42] = $r40
  ;;
  sd 8[$r42] = $r41
  ;;
  goto ret_from_test
  ;;

.global pcrel_no_bcu_test
.global bpbcu_pcrel_no_bcu_test
pcrel_no_bcu_test:
bpbcu_pcrel_no_bcu_test:
  pcrel $r43 = 0x20
  ;;
  get_got_addr $r42 ret_val
  ;;
  sd 0[$r42] = $r43
  ;;
  goto ret_from_test
  ;;

.global pcrel_bcu_test
.global bpbcu_pcrel_bcu_test
pcrel_bcu_test:
bpbcu_pcrel_bcu_test:
  get $r41 = $pc
  pcrel $r43 = 0x20
  ;;
  get_got_addr $r42 ret_val
  ;;
  sd 0[$r42] = $r43
  ;;
  sd 8[$r42] = $r41
  ;;
  goto ret_from_test
  ;;

verify_test_regs:
  make $r42 = 0
  get_got_addr $r40 saved_regs
  ;;
  ld $r41 = 0[$r40] //test r0
  ;;
  compd.eq $r41 = $r41, $r0
  ;;
  cb.deqz $r41, reg_0_changed
  ;;
  ld $r41 = 8[$r40] //test r1
  ;;
  compd.eq $r41 = $r41, $r1
  ;;
  cb.deqz $r41, reg_1_changed
  ;;
  ld $r41 = 24[$r40] //test r12
  ;;
  compd.eq $r41 = $r41, $r12
  ;;
  cb.deqz $r41, reg_12_changed
  ;;
  ld $r41 = 16[$r40] //test ra
  get $r43 = $ra
  ;;
  compd.eq $r41 = $r41, $r43
  ;;
  cb.deqz $r41, reg_ra_changed
  ;;
  goto exit_test_regs // no reg changed
  ;;
reg_ra_changed:
  get_got_addr $r1 ret_val
  ;;
  sd 0[$r1] = $r43
  ;;
  addd $r42 = $r42, 1
  ;;
reg_12_changed:
  addd $r42 = $r42, 1
  ;;
reg_1_changed:
  addd $r42 = $r42, 1
  ;;
reg_0_changed:
  addd $r42 = $r42, 1
  ;;
exit_test_regs:
  copyd $r0 = $r42
  goto ret_verify_test_regs
  ;;

