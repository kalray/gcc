    .text
    .align 8
    .global check_args_are_correct
    .type   check_args_are_correct, @function
check_args_are_correct:
    andd $r16 = $sp, 0x1f
    ;;
    cb.dnez $r16, 1f
    ;;
    compd.eq $r15 = $r1, 1
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r2, 2
    ;;
    cb.deqz $r15, 1f
    ;;
    ld $r16 = 0[$r0]
    ;;
    compd.eq $r16 = $r16, 0x0000000100000000
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 8[$r0]
    ;;
    compd.eq $r16 = $r16, 0x0000000000000001
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 16[$r0]
    ;;
    compd.eq $r16 = $r16, 0x0000000000000002
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 24[$r0]
    ;;
    compd.eq $r16 = $r16, 0xcdefaa9955555555
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 32[$r0]
    ;;
    compd.eq $r16 = $r16, 0xdeadbeef12345678
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 40[$r0]
    ;;
    compd.eq $r16 = $r16, 0xbcdefbb888888888
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 48[$r0]
    ;;
    compd.eq $r16 = $r16, 0xdeaddead12345678
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 56[$r0]
    ;;
    compd.eq $r16 = $r16, 0xbcdefcc777777777
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 64[$r0]
    ;;
    compd.eq $r16 = $r16, 0xbeefbeef12345678
    ;;
    cb.deqz $r16, 1f
    ;;
    make $r1 = 1
    ;;
    make $r2 = 2
    ;;
    goto check_args_are_correct_user /* tail call */
    ;;
1:
    make $r0 = 0
    ret
    ;;
