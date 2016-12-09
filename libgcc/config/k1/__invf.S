	.file	"____invf.c"
	.text


	.align 8
	.globl __invf
	.type	__invf, @function
__invf:
	copy $r3 = $r0
	fsinv $r0 = $r0
	;;
	copy $r2 = $r0
	;;
	cb.odd $r0, .L44
	;;
	and $r0 = $r0, 9, 9
	;;
	cb.eqz $r0, .L45
	;;
	and $r0 = $r2, 10, 10
	;;
	cb.nez $r0, .L46
	;;
	and $r2 = $r2, 2
	and $r4 = $r3, 22, 0
	and $r0 = $r3, 31, 31
	;;
	cb.nez $r2, .L47
	;;
	extfz $r5 = $r3, 23+8-1, 23
	;;
	sbf $r32 = $r5, 127
	;;
.L10:
	or $r4 = $r4, 29, 23
	make $r6 = 0x3f800000
	;;
	fsdiv $r6 = $r6, $r4
	;;
	get $r7 = $cs
	;;
	extfz $r7 = $r7, 8+2-1, 8
	;;
	comp.ne $r37 = $r7, 1
	;;
	cb.eqz $r37, .L48
	;;
	comp.ne $r2 = $r7, 2
	;;
	cb.eqz $r2, .L49
	;;
.L12:
	get $r36 = $cs
	;;
	get $r35 = $cs
	;;
	make $r33 = 768
	;;
	get $r8 = $cs
	;;
	hfxb $cs, $r33
	;;
	barrier
	;;
	make $r5 = 0x3f800000
	and $r8 = $r8, $r33
	;;
	ffms $r2 = $r5, $r4, $r6
	sll $r8 = $r8,16
	;;
	or $r8 = $r8, $r33
	;;
	ffma $r6 = $r6, $r2, $r6
	;;
	ffms $r2 = $r5, $r4, $r6
	;;
	ffma $r6 = $r6, $r2, $r6
	;;
	ffms $r2 = $r5, $r4, $r6
	;;
	ffma $r6 = $r6, $r2, $r6
	;;
	ffms $r9 = $r5, $r4, $r6
	;;
	ffma $r9 = $r6, $r9, $r6
	;;
	ffms $r2 = $r5, $r4, $r9
	;;
	hfxb $cs, $r8
	;;
	barrier
	;;
	ffma $r9 = $r9, $r2, $r6
	;;
	extfz $r8 = $r9, 23+8-1, 23
	ffms $r4 = $r5, $r4, $r9
	copy $r2 = $r9
	;;
	add $r8 = $r8, $r32
	;;
	comp.le $r34 = $r8, 254
	;;
	cb.eqz $r34, .L50
	;;
	cb.lez $r8, .L18
	;;
	and $r2 = $r9, 22, 30
	sll $r8 = $r8,23
	and $r36 = $r36, 32
	;;
	or $r0 = $r2, $r0
	;;
	or $r0 = $r0, $r8
	;;
	cb.eqz $r36, .L51
	;;
.L19:
	sll $r7 = $r7,24
	make $r1 = 0x3f800000
	;;
	ffms $r3 = $r1, $r3, $r0
	or $r7 = $r7, 9, 8
	;;
	hfxb $cs, $r7
	;;
	barrier
	;;
	and $r35 = $r35, 16
	;;
	cb.eqz $r35, .L52
	;;
.L20:
	make $r1 = 0x0
	;;
	fcomp.oeq $r3 = $r3, $r1
	;;
	cb.nez $r3, .L6
	;;
	make $r1 = 2097152
	;;
	hfxb $cs, $r1
	;;
.L6:
	ret
	;;
.L45:
	copy $r0 = $r2
	ret
	;;
.L44:
	get $r4 = $cs
	;;
	get $r5 = $cs
	;;
	make $r1 = 768
	;;
	hfxb $cs, $r1
	;;
	barrier
	;;
	make $r1 = 0x3f800000
	and $r5 = $r5, 9, 8
	;;
	ffms $r2 = $r1, $r3, $r0
	sll $r5 = $r5,16
	;;
	or $r5 = $r5, 9, 8
	;;
	ffma $r0 = $r0, $r2, $r0
	;;
	ffms $r2 = $r1, $r3, $r0
	;;
	ffma $r0 = $r0, $r2, $r0
	;;
	ffms $r3 = $r1, $r3, $r0
	;;
	hfxb $cs, $r5
	;;
	barrier
	;;
	not $r1 = $r4
	;;
	and $r1 = $r1, 62
	;;
	hfxb $cs, $r1
	;;
	ffma $r0 = $r0, $r3, $r0
	ret
	;;
.L47:
	clz $r32 = $r4
	sll $r4 = $r4,1
	;;
	add $r2 = $r32, -9
	add $r32 = $r32, 118
	;;
	sll $r4 = $r4,$r2
	;;
	xor $r4 = $r4, 8388608
	goto .L10
	;;
.L46:
	and $r0 = $r2, 31, 31
	;;
	or $r0 = $r0, 30, 23
	ret
	;;
.L50:
	make $r1 = 2097152
	;;
	hfxb $cs, $r1
	;;
	make $r1 = 524288
	;;
	hfxb $cs, $r1
	;;
	sll $r1 = $r7,24
	;;
	or $r1 = $r1, $r33
	;;
	hfxb $cs, $r1
	;;
	barrier
	;;
	comp.ne $r1 = $r7, 2
	;;
	cb.eqz $r1, .L53
	;;
	cb.eqz $r37, .L54
	;;
	comp.ne $r7 = $r7, 3
	;;
	cb.eqz $r7, .L15
	;;
.L16:
	or $r0 = $r0, 30, 23
	;;
.L58:
	ret
	;;
.L48:
	cb.eqz $r0, .L12
	;;
	make $r2 = 33555200
	;;
	hfxb $cs, $r2
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L12
	;;
.L49:
	cb.eqz $r0, .L12
	;;
	make $r2 = 16777984
	;;
	hfxb $cs, $r2
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	goto .L12
	;;
.L18:
	comp.lt $r5 = $r8, -25
	;;
	cb.nez $r5, .L21
	;;
	make $r5 = 0x0
	ffma $r6 = $r9, $r4, $r6
	;;
	fcomp.oeq $r37 = $r4, $r5
	;;
	cb.nez $r37, .L35
	;;
	fcomp.oeq $r37 = $r6, $r9
	;;
	cb.nez $r37, .L25
	;;
	fcomp.uge $r37 = $r5, $r4
	;;
	cb.eqz $r37, .L39
	;;
	fcomp.olt $r5 = $r4, $r5
	;;
	cb.eqz $r5, .L25
	;;
.L38:
	make $r1 = 16777728
	;;
.L22:
	comp.ge $r4 = $r32, -125
	;;
	cb.nez $r4, .L28
	;;
	make $r4 = 0xd800000
	add $r32 = $r32, 100
	;;
	fmul $r9 = $r9, $r4
	;;
.L28:
	sll $r2 = $r2,9
	neg $r5 = $r8
	add $r4 = $r32, 127
	;;
	srl $r2 = $r2,1
	and $r4 = $r4, 255
	;;
	or $r2 = $r2, 31, 31
	sll $r4 = $r4,23
	;;
	srl $r5 = $r2,$r5
	;;
	and $r5 = $r5, 511
	;;
	comp.ne $r5 = $r5, 256
	;;
	cb.eqz $r5, .L55
	;;
.L29:
	fmul $r9 = $r9, $r4
	make $r1 = 32
	;;
	hfxb $cs, $r1
	;;
	or $r0 = $r0, $r9
	make $r1 = 0x3f800000
	;;
	ffms $r3 = $r1, $r3, $r0
	make $r1 = 0x0
	;;
	fcomp.oeq $r3 = $r3, $r1
	;;
	cb.eqz $r3, .L56
	;;
.L30:
	get $r1 = $cs
	;;
	and $r1 = $r1, 32
	;;
	cb.eqz $r1, .L31
	;;
	make $r1 = 1048576
	;;
	hfxb $cs, $r1
	;;
.L31:
	sll $r7 = $r7,24
	;;
	or $r7 = $r7, 9, 8
	;;
	hfxb $cs, $r7
	;;
	barrier
	;; /* Can't issue next in the same bundle */
	ret
	;;
.L21:
	make $r1 = 1048576
	;;
	hfxb $cs, $r1
	;;
	sll $r1 = $r7,24
	;;
	or $r1 = $r1, 9, 8
	;;
	hfxb $cs, $r1
	;;
	barrier
	;;
	cb.eqz $r37, .L57
	;;
	comp.ne $r7 = $r7, 2
	;;
	cb.nez $r7, .L6
	;;
	cb.eqz $r0, .L6
	;;
.L33:
	or $r0 = $r0, 1
	ret
	;;
.L54:
	cb.eqz $r0, .L16
	;;
.L15:
	or $r0 = $r0, 2139095039
	ret
	;;
.L52:
	make $r1 = 16
	;;
	hfxb $cs, $r1
	;; /* Can't issue next in the same bundle */
	goto .L20
	;;
.L51:
	make $r1 = 32
	;;
	hfxb $cs, $r1
	;; /* Can't issue next in the same bundle */
	goto .L19
	;;
.L53:
	cb.eqz $r0, .L15
	;;
	or $r0 = $r0, 30, 23
	goto .L58
	;;
.L35:
	copy $r1 = $r33
	goto .L22
	;;
.L56:
	make $r1 = 2097152
	;;
	hfxb $cs, $r1
	;; /* Can't issue next in the same bundle */
	goto .L30
	;;
.L25:
	fcomp.une $r6 = $r6, $r9
	;;
	cb.nez $r6, .L22
	;;
	make $r5 = 0x0
	;;
	fcomp.uge $r6 = $r5, $r4
	;;
	cb.eqz $r6, .L38
	;;
	fcomp.olt $r4 = $r4, $r5
	;;
	cb.eqz $r4, .L22
	;;
.L39:
	make $r1 = 33554688
	goto .L22
	;;
.L55:
	add $r8 = $r8, 24
	;;
	sll $r2 = $r2,$r8
	;;
	cb.nez $r2, .L29
	;;
	get $r2 = $cs
	;;
	extfz $r2 = $r2, 8+2-1, 8
	;;
	cb.nez $r2, .L29
	;;
	hfxb $cs, $r1
	;; /* Can't issue next in the same bundle */
	goto .L29
	;;
.L57:
	cb.eqz $r0, .L33
	;;
	ret
	;;
	.size	__invf, .-__invf
	.ident	"GCC: (GNU) 4.5.4 20110503 (prerelease) [Kalray Compiler 1.0.0-0 7ec0843]"

