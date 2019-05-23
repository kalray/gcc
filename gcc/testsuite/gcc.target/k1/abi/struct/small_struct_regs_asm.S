    .text
    .align 8
    .global check_args_are_correct
    .type   check_args_are_correct, @function
check_args_are_correct:
    andd $r16 = $sp, 7
    ;;
    cb.dnez $r16, 1f
    ;;
    compd.eq $r15 = $r0, 0xdeadbeefffeeddcc
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r1, 1
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r2, 2
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r3, 3
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r4, 4
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r5, 5
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r6, 6
    ;;
    cb.deqz $r15, 1f
    ;;
    make $r0 = 0xdeadbeefffeeddcc
    ;;
    make $r1 = 1
    ;;
    make $r2 = 2
    ;;
    make $r3 = 3
    ;;
    make $r4 = 4
    ;;
    make $r5 = 5
    ;;
    make $r6 = 6
    ;;
    goto check_args_are_correct_user /* tail call */
    ;;
1:
    make $r0 = 0
    ret
    ;;
