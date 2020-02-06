    .text
    .align 8
    .global check_return_is_correct
    .type   check_return_is_correct, @function
check_return_is_correct:
    andd $r16 = $sp, 0x1f
    ;;
    cb.deqz $r16, 1f
    ;;
    make $r0 = 0
    ret
    ;;
1:
    get $r16 = $ra
    ;;
    addd $r12 = $r12, -16
    ;;
    sd 0[$r12] = $r16
    icall $r0
    ;;
    compd.eq $r15 = $r0, 0x0
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r1, 0x1
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r2, 0x2
    ;;
    cb.deqz $r15, 1f
    ;;
    compd.eq $r15 = $r3, 0x3
    ;;
    cb.deqz $r15, 1f
    ;;
    make $r0 = 1
    ;;
    goto 2f
    ;;
1:
    make $r0 = 0
    ;;
2:
    ld $r16 = 0[$r12]
    ;;
    set $ra = $r16
    ;;
    addd $r12 = $r12, 16
    ret
    ;;
