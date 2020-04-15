.section .boot, "ax", @progbits

.global _start
_start:
  make $r0=0x1050000000204 // KVX_SFR_PS_ET_WFXL_CLEAR | KVX_SFR_PS_USE_WFXL_CLEAR | KVX_SFR_PS_V64_WFXL_SET | KVX_SFR_PS_DCE_WFXL_SET | KVX_SFR_PS_ICE_WFXL_SET
  ;;
  wfxl $ps, $r0
  ;;
	make $r12 = _rm_stack_start
	;;
  call reserve_resources_pl1
  ;;
  call add_pl2
  ;;
  goto start_test
  ;;


reserve_resources_pl1:
  // delegate rights
  make $r0 = 3 /* reserve opcode trap */
  ;;
  wfxl $htow, $r0
  ;;
  make $r0 = 0x5555555555555555 /* all miscs assigned to PL+1*/
  ;;
  set $mow = $r0
  ;;
  make $r0 = 0x55555555 << 32 /* delegate all ps fields excepting for sme and smr*/
  ;;
  wfxl $psow = $r0
  ;;
  make $r0 = 0x55555505 << 32
  ;;
  wfxm $psow = $r0
  ;;
  // take scall Q2
  make $r0 = 5 << 32
  ;;
  wfxl $syow, $r0
  ;;
  // set ev
  make $r0 = ev_debug
  ;;
  set $ev = $r0
  ;;
  ret
  ;;

add_pl2:
  // prepare rfe
  make $r0 = add_pl2_first_instruction
  ;;
  set $spc = $r0
  ;;
  get $r0 = $ps
  make $r1 = 1
  ;;
  insf $r0 = $r1, 1, 0 /* sps.pl = ps.pl + 1 */
  make $r1 = 0xfff3ffff // sps = ps & ~(KVX_SFR_PS_SME_MASK | KVX_SFR_PS_SMR_MASK)
  ;;
  andd $r0 = $r0, $r1 // compute the sps wfxl set part
  ;;
  slld $r0 = $r0, 32 // move the sps wfxl set part to its location
  ;;
  ord $r0 = $r0, $r1 // add the sps wfxl clear part
  ;;
  wfxl $sps, $r0
  ;;
  rfe
  ;;
add_pl2_first_instruction:
  ret
  ;;

.align 4096
.global ev_debug
ev_debug:
  rfe
  ;;

.align 0x40
.global ev_trap
ev_trap:
  set $spc = $r0
  ;;
  rfe;
  ;;

.align 0x40
.global ev_irq
ev_irq:
  rfe;
  ;;

.align 0x40
.global ev_scall
ev_scall:
  rfe;
  ;;

.section .data
_debug_start:
  .word 0xabababab, 0xabababab, 0xabababab, 0xabababab, 0xabababab, 0xabababab, 0xabababab, 0xabababab, 0xabababab, 0xabababab, 0xabababab


