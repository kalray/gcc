#name: kvx-farcall-goto-none-function
#source: farcall-goto-none-function.s
#as:
#ld: -Ttext 0x1000 --section-start .foo=0x20001000
#objdump: -dr
#...

Disassembly of section .text:

.* <__bar_veneer>:
    1000:	00 00 40 e0 04 00 08 00                         	make \$r16 = 536875008 \(0x20001000\);;

    1008:	10 00 d8 0f                                     	igoto \$r16;;

    100c:	00 00 00 00                                     	errop ;;


.* <_start>:
    1010:	fc ff ff 17                                     	goto 1000 <__bar_veneer>;;

    1014:	00 00 d0 0f                                     	ret ;;


Disassembly of section .foo:

.* <bar>:
.*:	00 00 d0 0f                                     	ret ;;

