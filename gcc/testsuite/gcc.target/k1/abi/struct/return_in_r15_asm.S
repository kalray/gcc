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
    sbfd $r12 = $r1, $r12
    get $r16 = $ra
    ;;
    copyd $r15 = $r12
    ;;
    addd $r12 = $r12, -24
    ;;
    sd 16[$r12] = $r15
    ;;
    sd 0[$r12] = $r16
    ;;
    sd 8[$r12] = $r1
    ;;
    icall $r0
    ;;
    ld $r16 = 16[$r12]
    ;;
    compd.eq $r16 = $r0, $r16
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 0[$r0]
    ;;
    compd.eq $r16 = $r16, 0x0
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 8[$r0]
    ;;
    compd.eq $r16 = $r16, 0x1
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 16[$r0]
    ;;
    compd.eq $r16 = $r16, 0x0000000300000002
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 24[$r0]
    ;;
    compd.eq $r16 = $r16, 0xabababababababab
    ;;
    cb.deqz $r16, 1f
    ;;
    ld $r16 = 32[$r0]
    ;;
    compd.eq $r16 = $r16, 0xabababababababab
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
    addd $r12 = $r12, 24
    ret
    ;;
