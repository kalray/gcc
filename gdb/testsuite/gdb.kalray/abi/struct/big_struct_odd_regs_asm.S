    .text
    .align 8
    .global check_args_are_correct
    .type   check_args_are_correct, @function
check_args_are_correct:
    andd $r16 = $sp, 7
    ;;
    cb.dnez $r16, 1f
    ;;
    compd.eq $r15 = $r0, 0
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
    ld $r16 = 0[$r1]
    ;;
    compd.eq $r16 = $r16, 0xdeadbeefffeeddcc
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 8[$r1]
    ;;
    compd.eq $r16 = $r16, 0xAABBCCDDEEFF0102
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 16[$r1]
    ;;
    compd.eq $r16 = $r16, 0x0304050607080910
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 24[$r1]
    ;;
    compd.eq $r16 = $r16, 0x1112131415161718
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 32[$r1]
    ;;
    compd.eq $r16 = $r16, 0x4444444444444444
    ;;
    cb.deqz $r16, 1f
    ;;
    make $r0 = 0
    ;;
    make $r2 = 2
    ;;
    make $r3 = 3
    ;;
    goto check_args_are_correct_user /* tail call */
    ;;
1:
    make $r0 = 0
    ret
    ;;
