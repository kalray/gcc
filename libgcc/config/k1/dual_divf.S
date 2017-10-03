	.text

	.align 8
	.globl u2float
	.type	u2float, @function
u2float:
	ret
	;;
	.size	u2float, .-u2float

	.align 8
	.globl float2u
	.type	float2u, @function
float2u:
	ret
	;;
	.size	float2u, .-float2u

	.align 8
	.globl dual_divf_p0
	.type	dual_divf_p0, @function
dual_divf_p0:
	fsinvn $r1 = $r0
	and $r7 = $r0, 22, 0
	;;
	and $r4 = $r1, 2
	and $r3 = $r0, 31, 31
	;;
	cb.odd $r1, .L16
	;;
	cb.nez $r4, .L17
	;;
.L6:
	or $r8 = $r7, 29, 23
	make $r9 = 0x3f800000
	;;
	fsdiv $r63 = $r9, $r8
	;;
	get $r32 = $cs
	;;
	extfz $r33 = $r32, 8+2-1, 8
	comp.ne $r34 = $r3, 0
	;;
	comp.eq $r35 = $r33, 1
	comp.eq $r37 = $r33, 2
	;;
	and $r36 = $r34, $r35
	and $r38 = $r34, $r37
	;;
	cb.nez $r36, .L18
	;;
	cb.nez $r38, .L19
	;;
.L8:
	get $r1 = $cs
	;;
	get $r1 = $cs
	;;
	make $r41 = 0x3f800000
	sll $r43 = $r33,24
	;;
	ffms.rn.s $r42 = $r41, $r8, $r63
	or $r44 = $r43, 9, 8
	;;
	ffma.rn.s $r45 = $r63, $r42, $r63
	;;
	ffms.rn.s $r46 = $r41, $r8, $r45
	;;
	ffma.rn.s $r47 = $r45, $r46, $r45
	;;
	ffms.rn.s $r48 = $r41, $r8, $r47
	;;
	ffma.rn.s $r0 = $r47, $r48, $r47
	;;
	hfxb $cs, $r44
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	ret
	;;
.L17:
	clz $r0 = $r7
	sll $r2 = $r7,1
	;;
	add $r5 = $r0, -9
	;;
	sll $r6 = $r2,$r5
	;;
	xor $r7 = $r6, 8388608
	goto .L6
	;;
.L18:
	make $r40 = 33555200
	;;
	hfxb $cs, $r40
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L8
	;;
.L16:
	make $r49 = 0x3f800000
	;;
	ffms.rn.s $r50 = $r49, $r0, $r1
	;;
	ffma.rn.s $r51 = $r1, $r50, $r1
	;;
	ffms.rn.s $r52 = $r49, $r0, $r51
	;;
	ffma.rn.s $r53 = $r51, $r52, $r51
	;;
	ffms.rn.s $r54 = $r49, $r0, $r53
	;;
	ffma.rn.s $r0 = $r53, $r54, $r53
	ret
	;;
.L19:
	make $r39 = 16777984
	;;
	hfxb $cs, $r39
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L8
	;;
	.size	dual_divf_p0, .-dual_divf_p0

	.align 8
	.globl dual_divf_p1
	.type	dual_divf_p1, @function
dual_divf_p1:
	fcdiv $r3 = $r0, $r1
	extfz $r63 = $r1, 23+8-1, 23
	;;
	and $r4 = $r3, 9, 9
	copy $r6 = $r3
	and $r33 = $r0, 22, 0
	sbf $r38 = $r63, 127
	;;
	cb.odd $r3, .L64
	;;
	and $r8 = $r6, 2
	and $r7 = $r6, 4
	and $r35 = $r1, 22, 0
	;;
	cb.eqz $r4, .L22
	;;
	and $r3 = $r3, 10, 10
	;;
	cb.nez $r3, .L65
	;;
	cb.eqz $r8, .L27
	;;
	clz $r32 = $r35
	sll $r5 = $r35,1
	;;
	add $r9 = $r32, -9
	add $r38 = $r32, 118
	;;
	sll $r34 = $r5,$r9
	;;
	xor $r35 = $r34, 8388608
	;;
.L27:
	xor $r36 = $r1, $r0
	or $r39 = $r35, 29, 23
	;;
	and $r3 = $r36, 31, 31
	cb.nez $r7, .L66
	;;
	extfz $r37 = $r0, 23+8-1, 23
	or $r40 = $r33, 29, 23
	;;
	sbf $r41 = $r37, 127
	;;
.L29:
	get $r46 = $cs
	;;
	extfz $r47 = $r46, 8+2-1, 8
	comp.ne $r49 = $r3, 0
	;;
	comp.eq $r48 = $r47, 1
	comp.eq $r51 = $r47, 2
	;;
	and $r50 = $r48, $r49
	and $r52 = $r49, $r51
	;;
	cb.nez $r50, .L67
	;;
	cb.nez $r52, .L68
	;;
.L31:
	get $r55 = $cs
	;;
	get $r56 = $cs
	;;
	make $r57 = 768
	;;
	get $r58 = $cs
	;;
	extfz $r59 = $r58, 8+2-1, 8
	;;
	hfxb $cs, $r57
	;;
	barrier
	;;
	and $r60 = $r6, 12, 11
	copy $r6 = $r2
	;;
	cb.nez $r60, .L32
	;;
	sbf $r61 = $r38, 127
	;;
	and $r62 = $r61, 255
	;;
	sll $r62 = $r62,23
	;;
	fmul $r6 = $r2, $r62
	;;
.L32:
	and $r8 = $r6, 30, 0
	sll $r4 = $r59,24
	;;
	fmul $r2 = $r40, $r8
	or $r63 = $r4, 9, 8
	;;
	ffms $r7 = $r40, $r2, $r39
	;;
	ffma $r33 = $r2, $r7, $r8
	;;
	ffms $r32 = $r40, $r33, $r39
	;;
	hfxb $cs, $r63
	;;
	barrier
	;;
	ffma $r60 = $r33, $r32, $r8
	sbf $r61 = $r41, $r38
	;;
	extfz $r5 = $r60, 23+8-1, 23
	ffms $r36 = $r40, $r60, $r39
	copy $r39 = $r60
	and $r46 = $r60, 22, 30
	;;
	add $r38 = $r5, $r61
	;;
	comp.le $r9 = $r38, 254
	;;
	cb.eqz $r9, .L69
	;;
	cb.lez $r38, .L36
	;;
	and $r48 = $r55, 32
	or $r49 = $r46, $r3
	sll $r3 = $r38,23
	;;
	or $r3 = $r49, $r3
	;;
	cb.eqz $r48, .L70
	;;
.L37:
	ffms $r51 = $r0, $r1, $r3
	sll $r52 = $r47,24
	;;
	or $r53 = $r52, 9, 8
	;;
	hfxb $cs, $r53
	;;
	barrier
	;;
	and $r54 = $r56, 16
	;;
	cb.eqz $r54, .L71
	;;
.L38:
	make $r56 = 0x0
	;;
	fcomp.oeq $r57 = $r51, $r56
	;;
	cb.nez $r57, .L22
	;;
	make $r58 = 2097152
	;;
	hfxb $cs, $r58
	;;
.L22:
	copy $r0 = $r3
	ret
	;;
.L64:
	fmul.rn.s $r41 = $r0, $r2
	;;
	ffms.rn.s $r42 = $r0, $r1, $r41
	;;
	ffma.rn.s $r43 = $r41, $r42, $r2
	;;
	ffms.rn.s $r1 = $r0, $r1, $r43
	;;
	ffma $r3 = $r43, $r1, $r2
	;;
	copy $r0 = $r3
	ret
	;;
.L66:
	clz $r42 = $r33
	;;
	add $r43 = $r42, -8
	add $r41 = $r42, 118
	;;
	sll $r44 = $r33,$r43
	;;
	xor $r45 = $r44, 8388608
	;;
	or $r40 = $r45, 29, 23
	goto .L29
	;;
.L65:
	and $r3 = $r6, 31, 31
	;;
	get $r5 = $cs
	;;
	extfz $r38 = $r5, 8+2-1, 8
	comp.eq $r35 = $r3, 0
	comp.ne $r36 = $r3, 0
	;;
	comp.eq $r34 = $r38, 2
	comp.eq $r9 = $r38, 1
	comp.eq $r40 = $r38, 3
	;;
	and $r37 = $r34, $r35
	and $r39 = $r9, $r36
	;;
	or $r0 = $r40, $r37
	;;
	cb.nez $r39, .L34
	;;
	cb.nez $r0, .L34
	;;
.L35:
	or $r3 = $r3, 30, 23
	goto .L22
	;;
.L69:
	and $r59 = $r1, 31, 31
	and $r60 = $r0, 31, 31
	make $r61 = 2097152
	;;
	hfxb $cs, $r61
	;;
	make $r62 = 524288
	;;
	hfxb $cs, $r62
	;;
	sll $r62 = $r47,24
	;;
	or $r6 = $r62, 9, 8
	;;
	hfxb $cs, $r6
	;;
	barrier
	;;
	comp.ne $r63 = $r60, $r59
	comp.eq $r8 = $r47, 2
	comp.eq $r2 = $r60, $r59
	comp.eq $r33 = $r47, 3
	;;
	and $r7 = $r48, $r63
	and $r4 = $r8, $r2
	;;
	or $r32 = $r33, $r7
	;;
	cb.nez $r4, .L34
	;;
	cb.eqz $r32, .L35
	;;
.L34:
	or $r3 = $r3, 2139095039
	;;
	copy $r0 = $r3
	ret
	;;
.L67:
	make $r54 = 33555200
	;;
	hfxb $cs, $r54
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L31
	;;
.L68:
	make $r53 = 16777984
	;;
	hfxb $cs, $r53
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L31
	;;
.L36:
	comp.lt $r34 = $r38, -25
	;;
	cb.nez $r34, .L39
	;;
	make $r44 = 0x0
	ffma $r46 = $r60, $r36, $r8
	make $r57 = 768
	;;
	fcomp.oeq $r45 = $r36, $r44
	;;
	cb.nez $r45, .L40
	;;
	fcomp.uge $r48 = $r44, $r36
	fcomp.une $r50 = $r60, $r46
	make $r57 = 33554688
	;;
	xor $r49 = $r48, 1
	;;
	and $r51 = $r50, $r49
	;;
	cb.eqz $r51, .L72
	;;
.L40:
	comp.ge $r58 = $r61, -125
	;;
	cb.nez $r58, .L41
	;;
	make $r59 = 0xd800000
	add $r61 = $r61, 100
	;;
	fmul $r60 = $r60, $r59
	;;
.L41:
	sll $r62 = $r39,9
	neg $r8 = $r38
	add $r4 = $r38, 24
	add $r5 = $r61, 127
	;;
	srl $r62 = $r62,1
	and $r9 = $r5, 255
	;;
	or $r6 = $r62, 31, 31
	sll $r36 = $r9,23
	;;
	srl $r2 = $r6,$r8
	sll $r7 = $r6,$r4
	;;
	and $r63 = $r2, 511
	comp.eq $r32 = $r7, 0
	;;
	comp.eq $r33 = $r63, 256
	;;
	and $r38 = $r33, $r32
	;;
	cb.nez $r38, .L73
	;;
.L43:
	make $r35 = 32
	;;
	hfxb $cs, $r35
	;;
	fmul $r37 = $r60, $r36
	make $r40 = 0x0
	;;
	or $r3 = $r3, $r37
	;;
	ffms $r0 = $r0, $r1, $r3
	;;
	fcomp.oeq $r1 = $r0, $r40
	;;
	cb.eqz $r1, .L74
	;;
.L45:
	get $r42 = $cs
	;;
	and $r43 = $r42, 32
	;;
	cb.eqz $r43, .L46
	;;
	make $r44 = 1048576
	;;
	hfxb $cs, $r44
	;;
.L46:
	sll $r47 = $r47,24
	;;
	or $r45 = $r47, 9, 8
	;;
	hfxb $cs, $r45
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L22
	;;
.L71:
	make $r55 = 16
	;;
	hfxb $cs, $r55
	;; /* Can't issue next in the same bundle */
	goto .L38
	;;
.L70:
	make $r50 = 32
	;;
	hfxb $cs, $r50
	;; /* Can't issue next in the same bundle */
	goto .L37
	;;
.L39:
	make $r0 = 1048576
	;;
	hfxb $cs, $r0
	;;
	make $r1 = 2097152
	;;
	hfxb $cs, $r1
	;;
	sll $r35 = $r47,24
	;;
	or $r37 = $r35, 9, 8
	;;
	hfxb $cs, $r37
	;;
	barrier
	;;
	comp.eq $r40 = $r3, 0
	comp.eq $r42 = $r47, 2
	;;
	and $r41 = $r48, $r40
	and $r43 = $r49, $r42
	;;
	cb.nez $r41, .L47
	;;
	cb.eqz $r43, .L22
	;;
.L47:
	or $r3 = $r3, 1
	goto .L22
	;;
.L74:
	make $r41 = 2097152
	;;
	hfxb $cs, $r41
	;; /* Can't issue next in the same bundle */
	goto .L45
	;;
.L73:
	get $r39 = $cs
	;;
	extfz $r34 = $r39, 8+2-1, 8
	;;
	cb.nez $r34, .L43
	;;
	hfxb $cs, $r57
	;; /* Can't issue next in the same bundle */
	goto .L43
	;;
.L72:
	fcomp.olt $r52 = $r36, $r44
	fcomp.oeq $r54 = $r60, $r46
	make $r57 = 16777728
	;;
	and $r53 = $r50, $r52
	and $r55 = $r54, $r49
	;;
	cb.nez $r53, .L52
	;;
	cb.nez $r55, .L40
	;;
	and $r56 = $r54, $r52
	make $r57 = 33554688
	;;
	cmove.eqz $r57 = $r56, 0
	goto .L40
	;;
.L52:
	make $r57 = 16777728
	goto .L40
	;;
	.size	dual_divf_p1, .-dual_divf_p1
	.ident	"GCC: (GNU) 4.9.1 20140424 (prerelease) [Kalray Compiler unknown 3b3a051-dirty]"
