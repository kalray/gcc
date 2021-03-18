    .text
    .align 8
    .global check_args_are_correct
    .type   check_args_are_correct, @function
check_args_are_correct:
    andd $r16 = $sp, 0x1f
    ;;
    cb.dnez $r16, 1f
    ;;
    compd.eq $r15 = $r0, 0
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r1, 0xabcdef9955555555
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r2, 0xdeadbeef12345678
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r3, 0xabcdef9955555555
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r4, 0xdeadbeef12345678
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
    compd.eq $r15 = $r7, 7
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r8, 0xabcdef9955555555
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r9, 0xdedebeef12345678
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r10, 10
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r11, 11
    ;;
    cb.deqz $r15, 1f
    ;;
    make $r0 = 0
    ;;
    make $r1 = 0xabcdef9955555555
    ;;
    make $r2 = 0xdeadbeef12345678
    ;;
    make $r3 = 0xabcdef9955555555
    ;;
    make $r4 = 0xdeadbeef12345678
    ;;
    make $r5 = 5
    ;;
    make $r6 = 6
    ;;
    make $r7 = 7
    ;;
    make $r8 = 0xabcdef9955555555
    ;;
    make $r9 = 0xdedebeef12345678
    ;;
    make $r10 = 10
    ;;
    make $r11 = 11
    ;;
    goto check_args_are_correct_user /* tail call */
    ;;
1:
    make $r0 = 0
    ret
    ;;
