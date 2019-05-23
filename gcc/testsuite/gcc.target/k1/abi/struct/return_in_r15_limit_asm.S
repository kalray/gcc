    .text
    .align 8
    .global check_return_is_correct
    .type   check_return_is_correct, @function
check_return_is_correct:
    andd $r16 = $sp, 7
    ;;
    cb.deqz $r16, 1f
    ;;
    make $r0 = 0
    ret
    ;;
1:
    sbfd $r12 = $r1, $r12
    get $r16 = $ra
    ;;
    copyd $r15 = $r12
    ;;
    addd $r12 = $r12, -16
    ;;
    sd 0[$r12] = $r16
    ;;
    sd 8[$r12] = $r1
    ;;
    icall $r0
    ;;
    ld $r16 = 0[$r15]
    ;;
    compd.eq $r16 = $r16, 0x0
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 8[$r15]
    ;;
    compd.eq $r16 = $r16, 0x1
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 16[$r15]
    ;;
    compd.eq $r16 = $r16, 0x2
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 24[$r15]
    ;;
    compd.eq $r16 = $r16, 0x3
    ;;
    cb.deqz $r16, 1f
    ;;
    lbz $r16 = 32[$r15]
    ;;
    compd.eq $r16 = $r16, 0x88
    ;;
    cb.deqz $r16, 1f
    ;;
    make $r0 = 1
    ;;
    goto 2f
    ;;
1:
    make $r0 = 0
    ;;
2:
    ld $r1 = 8[$r12]
    ;;
    ld $r16 = 0[$r12]
    addd $r12 = $r12, $r1
    ;;
    set $ra = $r16
    ;;
    addd $r12 = $r12, 16
    ret
    ;;
