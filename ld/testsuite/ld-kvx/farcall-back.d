#name: kvx-farcall-back
#source: farcall-back.s
#as:
#ld: -Ttext 0x1000 --section-start .foo=0x80001000
#objdump: -dr

#...

Disassembly of section .text:

.* <__bar2_veneer>:
    .*:	00 .. 40 e0 08 00 20 00                         	make \$r16 = .* \(0x.*\);;

    .*:	10 00 d8 0f                                     	igoto \$r16;;

.* <__bar3_veneer>:
    .*:	00 .. 40 e0 0c 00 20 00                         	make \$r16 = .* \(0x.*\);;

    .*:	10 00 d8 0f                                     	igoto \$r16;;


.* <__bar1_veneer>:
    .*:	00 .. 40 e0 04 00 20 00                         	make \$r16 = .* \(0x.*\);;

    .*:	10 00 d8 0f                                     	igoto \$r16;;

    .*:	00 f0 03 7f                                     	nop ;;

.* <_start>:
    .*:	.. ff ff 1f                                     	call .* <__bar1_veneer>;;

    .*:	.. ff ff 17                                     	goto .* <__bar1_veneer>;;

    .*:	.. ff ff 1f                                     	call .* <__bar2_veneer>;;

    .*:	.. ff ff 17                                     	goto .* <__bar2_veneer>;;

    .*:	.. ff ff 1f                                     	call .* <__bar3_veneer>;;

    .*:	.. ff ff 17                                     	goto .* <__bar3_veneer>;;

    .*:	00 00 d0 0f                                     	ret ;;

	...

.* <_back>:
    .*:	00 00 d0 0f                                     	ret ;;

Disassembly of section .foo:

.* <___start_veneer>:
.*:	00 .. 40 e0 04 00 00 00                         	make \$r16 = .* \(0x.*\);;

.*:	10 00 d8 0f                                     	igoto \$r16;;

.* <___back_veneer>:
.*:	00 .. 40 e0 08 00 00 00                         	make \$r16 = .* \(0x.*\);;

.*:	10 00 d8 0f                                     	igoto \$r16;;

.* <bar1>:
.*:	00 00 d0 0f                                     	ret ;;

.*:	.. ff ff 17                                     	goto .* <___start_veneer>;;

	...

.* <bar2>:
.*:	00 00 d0 0f                                     	ret ;;

.*:	.. fb ff 17                                     	goto .* <___start_veneer>;;

	...

.* <bar3>:
.*:	00 00 d0 0f                                     	ret ;;

.*:	.. f7 ff 17                                     	goto .* <___back_veneer>;;

