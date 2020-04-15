#name: kvx-farcall-goto-section
#source: farcall-goto-section.s
#as:
#ld: -Ttext 0x1000 --section-start .foo=0x20001000
#objdump: -dr
#...

Disassembly of section .text:

.* <___veneer>:
    1000:	00 01 40 e0 04 00 08 00                         	make \$r16 = 536875012 \(0x20001004\);;

    1008:	10 00 d8 0f                                     	igoto \$r16;;

    100c:	00 00 00 00                                     	errop ;;


.* <___veneer>:
    1010:	00 00 40 e0 04 00 08 00                         	make \$r16 = 536875008 \(0x20001000\);;

    1018:	10 00 d8 0f                                     	igoto \$r16;;

    101c:	00 00 00 00                                     	errop ;;


.* <_start>:
    1020:	fc ff ff 17                                     	goto 1010 <___veneer>;;

    1024:	f7 ff ff 17                                     	goto 1000 <___veneer>;;

    1028:	00 00 d0 0f                                     	ret ;;


Disassembly of section .foo:

.* <bar>:
.*:	00 00 d0 0f                                     	ret ;;


.* <bar2>:
.*:	00 00 d0 0f                                     	ret ;;

