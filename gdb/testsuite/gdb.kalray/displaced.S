.section .interp
.global _go_start
  goto _start
  ;;
.section .text
.global start_test
start_test:
  make $r0 = ev_debug
  ;;
  set $ev = $r0
  ;;
  make $r0 = 0x0000002000000004 /* Enable H/W loops, clear PS.ET */
  ;;
  wfxl $ps, $r0
  ;;

test_displaced1:
  make $r0 = 5
  make $r1 = 0
  ;;
test_displaced2:
  loopdo $r0, test_displaced4
  ;;
test_displaced3:
  addd $r1 = $r1, 1
  ;;Here $r1 = 5
test_displaced4:
  make $r0 = -1
  ;;
test_displaced5:
  scall 2048
  ;;
test_displaced6:
  make $r0 = test_displaced8
  ;;
test_displaced7:
  errop /* Invalid opcode */
  ;;
test_displaced8:
  get $r61 = $pc
  ;;
test_displaced9:
  compd.eq $r61 = $r61, test_displaced8
  addd $r2 = $r1, 1
  ;;
test_displaced10:
  cmoved.dnez $r61? $r1 = $r2
  make $r62 = 0
  ;;
test_displaced11:
  iget $r62
  ;;
test_displaced12:
  compd.eq $r62 = $r62, test_displaced11
  addd $r2 = $r1, 1
  ;;
test_displaced13:
  cmoved.dnez $r62? $r1 = $r2
  ;;
test_displaced:
  copyd $r0 = $r1
  ;;
  scall 4095
  ;;

