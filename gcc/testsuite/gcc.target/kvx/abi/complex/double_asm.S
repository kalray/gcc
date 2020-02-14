    .text
    .align 8
    .global check_args_are_correct
    .type   check_args_are_correct, @function
check_args_are_correct:
    andd $r16 = $sp, 0x1f
    ;;
    cb.dnez $r16, 1f
    ;;
    compd.eq $r15 = $r0, 0x3ff0000000000000
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r1, 0x3ff0000000000000
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r2, 0xbff0000000000000
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r3, 0x0000000000000000
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r4, 0x0000000000000000
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r5, 0xbff0000000000000
    ;;
    cb.deqz $r15, 1f
    ;;
    make $r0 = 0x3ff0000000000000
    ;;
    make $r1 = 0x3ff0000000000000
    ;;
    make $r2 = 0xbff0000000000000
    ;;
    make $r3 = 0x0000000000000000
    ;;
    make $r4 = 0x0000000000000000
    ;;
    make $r5 = 0xbff0000000000000
    ;;
    goto check_args_are_correct_user /* tail call */
    ;;
1:
    make $r0 = 0
    ret
    ;;
