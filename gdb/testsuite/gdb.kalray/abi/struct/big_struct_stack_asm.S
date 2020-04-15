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
    compd.eq $r15 = $r10, 10
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r11, 11
    ;;
    cb.deqz $r15, 1f
    ;;
    ld $r15 = 0[$sp]
    ;;
    ld $r15 = 0[$r15]
    ;;
    compd.eq $r15 = $r15, 0xdeadbeefffeeddcc
    ;;
    cb.deqz $r15, 1f
    ;;
    ld $r15 = 0[$sp]
    ;;
    ld $r15 = 8[$r15]
    ;;
    compd.eq $r15 = $r15, 0xAABBCCDDEEFF0102
    ;;
    cb.deqz $r15, 1f
    ;;
    ld $r15 = 0[$sp]
    ;;
    ld $r15 = 16[$r15]
    ;;
    compd.eq $r15 = $r15, 0x0304050607080910
    ;;
    cb.deqz $r15, 1f
    ;;
    ld $r15 = 0[$sp]
    ;;
    ld $r15 = 24[$r15]
    ;;
    compd.eq $r15 = $r15, 0x1112131415161718
    ;;
    cb.deqz $r15, 1f
    ;;
    ld $r15 = 0[$sp]
    ;;
    ld $r15 = 32[$r15]
    ;;
    compd.eq $r15 = $r15, 0x4444444444444444
    ;;
    cb.deqz $r15, 1f
    ;;
    ld $r16 = 8[$sp]
    ;;
    compd.eq $r16 = $r16, 5
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 16[$sp]
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
