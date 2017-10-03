	.align 8
	.globl __divsf3
	.type	__divsf3, @function
__divsf3:
	fsdiv $r4 = $r0, $r1
	copy $r3 = $r0
	make $r8 = 0x3f800000
	;;
	copy $r0 = $r4
	ffms.rn.s $r32 = $r8, $r1, $r4
	;;
	cb.even $r4, .specialCases
	;;
.standardCase:
	ffma.rn.s $r9 = $r4, $r32, $r4
	;;
	ffms.rn.s $r32 = $r8, $r1, $r9
	;;
	ffma.rn.s $r6 = $r9, $r32, $r9
	;;
	ffms.rn.s $r7 = $r8, $r1, $r6
	;;
	ffma.rn.s $r4 = $r6, $r7, $r6
	;;
	fmul.rn.s $r5 = $r3, $r4
	;;
	ffms.rn.s $r2 = $r3, $r1, $r5
	;;
	ffma.rn.s $r0 = $r5, $r2, $r4
	;;
	ffms.rn.s $r3 = $r3, $r1, $r0
	;;
	ffma $r0 = $r0, $r3, $r4
	ret
	;;
.specialCases:
	and $r33 = $r4, 9, 9
	and $r34 = $r4, 10, 10
	;;
	cb.eqz $r33, .L8
	;;
	cb.nez $r34, .L55
	;;
	and $r38 = $r4, 2
	and $r0 = $r4, 4
	copy $r6 = $r3
	and $r5 = $r3, 22, 0
	;;
	and $r4 = $r1, 22, 0
	and $r35 = $r1, 31, 31
	;;
	and $r36 = $r3, 31, 31
	;;
	cb.nez $r38, .L56
	;;
	extfz $r43 = $r1, 23+8-1, 23
	or $r4 = $r4, 29, 23
	xor $r32 = $r35, $r36
	;;
	sbf $r34 = $r43, 127
	;;
	cb.eqz $r0, .L17
	;;
.L62:
	clz $r44 = $r5
	;;
	add $r47 = $r44, -8
	add $r9 = $r44, 118
	;;
	sll $r46 = $r5,$r47
	;;
	xor $r45 = $r46, 8388608
	;;
	or $r5 = $r45, 29, 23
	;;
.L18:
	fsdiv $r7 = $r5, $r4
	;;
	get $r49 = $cs
	;;
	extfz $r6 = $r49, 8+2-1, 8
	;;
	comp.ne $r33 = $r6, 1
	;;
	cb.eqz $r33, .L57
	;;
	comp.ne $r51 = $r6, 2
	;;
	cb.eqz $r51, .L58
	;;
.L20:
	get $r37 = $cs
	;;
	get $r38 = $cs
	;;
	make $r61 = 0x3f800000
	sbf $r34 = $r9, $r34
	;;
	ffms.rn.s $r0 = $r61, $r4, $r7
	;;
	ffma.rn.s $r62 = $r7, $r0, $r7
	;;
	ffms.rn.s $r63 = $r61, $r4, $r62
	;;
	ffma.rn.s $r59 = $r62, $r63, $r62
	;;
	ffms.rn.s $r60 = $r61, $r4, $r59
	;;
	ffma.rn.s $r7 = $r59, $r60, $r59
	;;
	fmul.rn.s $r58 = $r5, $r7
	;;
	ffms.rn.s $r57 = $r5, $r4, $r58
	;;
	ffma.rn.s $r55 = $r58, $r57, $r7
	;;
	ffms.rn.s $r56 = $r5, $r4, $r55
	;;
	ffma $r9 = $r55, $r56, $r7
	;;
	extfz $r54 = $r9, 23+8-1, 23
	ffms $r4 = $r5, $r4, $r9
	copy $r40 = $r9
	;;
	add $r8 = $r54, $r34
	;;
	comp.le $r53 = $r8, 254
	;;
	cb.eqz $r53, .L59
	;;
	and $r37 = $r37, 32
	;;
	cb.lez $r8, .L26
	;;
	and $r33 = $r9, 22, 30
	;;
	or $r40 = $r33, $r32
	sll $r32 = $r8,23
	;;
	or $r0 = $r40, $r32
	;;
	cb.eqz $r37, .L60
	;;
.L27:
	sll $r37 = $r6,24
	ffms $r1 = $r3, $r1, $r0
	;;
	or $r36 = $r37, 9, 8
	;;
	hfxb $cs, $r36
	;;
	barrier
	;;
	and $r35 = $r38, 16
	;;
	cb.eqz $r35, .L61
	;;
.L28:
	make $r39 = 0x0
	;;
	fcomp.oeq $r1 = $r1, $r39
	;;
	cb.nez $r1, .L8
	;;
	make $r39 = 2097152
	;;
	hfxb $cs, $r39
	;;
.L8:
	ret
	;;
.L56:
	clz $r39 = $r4
	sll $r41 = $r4,1
	xor $r32 = $r35, $r36
	;;
	add $r42 = $r39, -9
	add $r34 = $r39, 118
	;;
	sll $r39 = $r41,$r42
	;;
	xor $r4 = $r39, 8388608
	;;
	or $r4 = $r4, 29, 23
	;;
	cb.nez $r0, .L62
	;;
.L17:
	extfz $r48 = $r6, 23+8-1, 23
	or $r5 = $r5, 29, 23
	;;
	sbf $r9 = $r48, 127
	goto .L18
	;;
.L55:
	and $r0 = $r4, 31, 31
	;;
	get $r1 = $cs
	;;
	extfz $r1 = $r1, 8+2-1, 8
	;;
	comp.ne $r35 = $r1, 1
	;;
	cb.eqz $r35, .L63
	;;
	comp.ne $r36 = $r1, 2
	;;
	cb.eqz $r36, .L64
	;;
	comp.ne $r37 = $r1, 3
	;;
	cb.eqz $r37, .L12
	;;
.L13:
	or $r0 = $r0, 30, 23
	;;
.L72:
	ret
	;;
.L63:
	cb.eqz $r0, .L13
	;;
.L12:
	or $r0 = $r0, 2139095039
	ret
	;;
.L59:
	make $r7 = 2097152
	;;
	hfxb $cs, $r7
	;;
	make $r2 = 524288
	;;
	hfxb $cs, $r2
	;;
	sll $r5 = $r6,24
	;;
	or $r4 = $r5, 9, 8
	;;
	hfxb $cs, $r4
	;;
	barrier
	;;
	comp.ne $r3 = $r6, 2
	;;
	cb.eqz $r3, .L65
	;;
	cb.eqz $r33, .L66
	;;
	comp.ne $r9 = $r6, 3
	;;
	cb.nez $r9, .L24
	;;
.L23:
	or $r0 = $r32, 2139095039
	ret
	;;
.L57:
	cb.eqz $r32, .L20
	;;
	make $r50 = 33555200
	;;
	hfxb $cs, $r50
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L20
	;;
.L58:
	cb.eqz $r32, .L20
	;;
	make $r52 = 16777984
	;;
	hfxb $cs, $r52
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L20
	;;
.L26:
	comp.lt $r41 = $r8, -25
	;;
	cb.nez $r41, .L29
	;;
	make $r0 = 0x0
	ffma $r7 = $r9, $r4, $r7
	;;
	fcomp.oeq $r42 = $r4, $r0
	;;
	cb.nez $r42, .L44
	;;
	fcomp.oeq $r43 = $r7, $r9
	;;
	cb.nez $r43, .L33
	;;
	fcomp.uge $r44 = $r0, $r4
	;;
	cb.eqz $r44, .L48
	;;
	fcomp.olt $r45 = $r4, $r0
	;;
	cb.eqz $r45, .L33
	;;
.L47:
	make $r2 = 16777728
	;;
.L30:
	comp.ge $r49 = $r34, -125
	;;
	cb.nez $r49, .L36
	;;
	make $r50 = 0xd800000
	add $r34 = $r34, 100
	;;
	fmul $r9 = $r9, $r50
	;;
.L36:
	sll $r58 = $r40,9
	neg $r56 = $r8
	add $r55 = $r34, 127
	;;
	srl $r57 = $r58,1
	and $r53 = $r55, 255
	;;
	or $r0 = $r57, 31, 31
	sll $r4 = $r53,23
	;;
	srl $r54 = $r0,$r56
	;;
	and $r52 = $r54, 511
	;;
	comp.ne $r51 = $r52, 256
	;;
	cb.eqz $r51, .L67
	;;
.L37:
	fmul $r0 = $r9, $r4
	;;
	cb.eqz $r37, .L68
	;;
.L38:
	or $r0 = $r32, $r0
	make $r5 = 0x0
	;;
	ffms $r4 = $r3, $r1, $r0
	;;
	fcomp.oeq $r3 = $r4, $r5
	;;
	cb.eqz $r3, .L69
	;;
.L39:
	get $r8 = $cs
	;;
	and $r7 = $r8, 32
	;;
	cb.eqz $r7, .L40
	;;
	make $r9 = 1048576
	;;
	hfxb $cs, $r9
	;;
.L40:
	sll $r40 = $r6,24
	;;
	or $r6 = $r40, 9, 8
	;;
	hfxb $cs, $r6
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	ret
	;;
.L61:
	make $r38 = 16
	;;
	hfxb $cs, $r38
	;; /* Can't issue next in the same bundle */
	goto .L28
	;;
.L60:
	make $r34 = 32
	;;
	hfxb $cs, $r34
	;; /* Can't issue next in the same bundle */
	goto .L27
	;;
.L29:
	make $r36 = 1048576
	;;
	hfxb $cs, $r36
	;;
	make $r35 = 2097152
	;;
	hfxb $cs, $r35
	;;
	sll $r34 = $r6,24
	;;
	or $r0 = $r34, 9, 8
	;;
	hfxb $cs, $r0
	;;
	barrier
	;;
	cb.eqz $r33, .L70
	;;
	comp.ne $r33 = $r6, 2
	;;
	cb.eqz $r33, .L71
	;;
.L43:
	copy $r0 = $r32
	;;
.L74:
	ret
	;;
.L66:
	comp.ne $r8 = $r36, $r35
	;;
	cb.nez $r8, .L23
	;;
.L24:
	or $r0 = $r32, 30, 23
	;;
.L73:
	ret
	;;
.L71:
	cb.eqz $r32, .L43
	;;
.L42:
	or $r0 = $r32, 1
	ret
	;;
.L64:
	cb.eqz $r0, .L12
	;;
	or $r0 = $r0, 30, 23
	goto .L72
	;;
.L65:
	comp.eq $r6 = $r36, $r35
	;;
	cb.nez $r6, .L23
	;;
	or $r0 = $r32, 30, 23
	goto .L73
	;;
.L44:
	make $r2 = 768
	goto .L30
	;;
.L69:
	make $r2 = 2097152
	;;
	hfxb $cs, $r2
	;; /* Can't issue next in the same bundle */
	goto .L39
	;;
.L68:
	make $r63 = 32
	;;
	hfxb $cs, $r63
	;; /* Can't issue next in the same bundle */
	goto .L38
	;;
.L33:
	fcomp.une $r46 = $r7, $r9
	;;
	cb.nez $r46, .L30
	;;
	make $r0 = 0x0
	;;
	fcomp.uge $r47 = $r0, $r4
	;;
	cb.eqz $r47, .L47
	;;
	fcomp.olt $r48 = $r4, $r0
	;;
	cb.eqz $r48, .L30
	;;
.L48:
	make $r2 = 33554688
	goto .L30
	;;
.L67:
	add $r60 = $r8, 24
	;;
	sll $r59 = $r0,$r60
	;;
	cb.nez $r59, .L37
	;;
	get $r62 = $cs
	;;
	extfz $r61 = $r62, 8+2-1, 8
	;;
	cb.nez $r61, .L37
	;;
	hfxb $cs, $r2
	;; /* Can't issue next in the same bundle */
	goto .L37
	;;
.L70:
	cb.eqz $r32, .L42
	;;
	copy $r0 = $r32
	goto .L74
	;;
	.size	__divsf3, .-__divsf3
	.ident	"GCC: (GNU) 4.5.3 20101216 (prerelease) [Kalray Compiler 1.0.0-0 389e0d6]"
