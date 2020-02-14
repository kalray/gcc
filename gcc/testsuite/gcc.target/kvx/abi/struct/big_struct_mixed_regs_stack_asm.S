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
    compd.eq $r15 = $r7, 7
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r8, 8
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r9, 9
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r11, 5
    ;;
    cb.deqz $r15, 1f
    ;;
    ld $r16 = 0[$r10]
    ;;
    compd.eq $r16 = $r16, 0xdeadbeefffeeddcc
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 8[$r10]
    ;;
    compd.eq $r16 = $r16, 0xAABBCCDDEEFF0102
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 16[$r10]
    ;;
    compd.eq $r16 = $r16, 0x0304050607080910
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 24[$r10]
    ;;
    compd.eq $r16 = $r16, 0x1112131415161718
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 32[$r10]
    ;;
    compd.eq $r16 = $r16, 0x4444444444444444
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 0[$sp]
    ;;
    compd.eq $r16 = $r16, 6
    ;;
    cb.deqz $r16, 1f
    ;;
    make $r0 = 0
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
    make $r7 = 7
    ;;
    make $r8 = 8
    ;;
    make $r9 = 9
    ;;
    make $r11 = 5
    ;;
    goto check_args_are_correct_user /* tail call */
    ;;
1:
    make $r0 = 0
    ret
    ;;
