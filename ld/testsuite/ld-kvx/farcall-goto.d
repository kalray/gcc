#name: kvx-farcall-goto
#source: farcall-goto.s
#as:
#ld: -Ttext 0x1000 --section-start .foo=0x20001000
#objdump: -dr
#...

Disassembly of section .text:

.* <__bar_veneer>:
    .*:	00 00 40 e0 04 00 08 00                         	make \$r16 = .* \(0x.*\);;

    .*:	10 00 d8 0f                                     	igoto \$r16;;

    .*:	00 f0 03 7f                                     	nop ;;

.* <_start>:
    .*:	.. ff ff 17                                     	goto .* <__bar_veneer>;;

    .*:	00 00 d0 0f                                     	ret ;;


Disassembly of section .foo:

.* <bar>:
.*:	00 00 d0 0f                                     	ret ;;

