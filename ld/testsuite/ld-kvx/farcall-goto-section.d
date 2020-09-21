#name: kvx-farcall-goto-section
#source: farcall-goto-section.s
#as:
#ld: -Ttext 0x1000 --section-start .foo=0x20001000
#objdump: -dr
#...

Disassembly of section .text:

.* <___veneer>:
    .*:	.. .. 40 e0 .. .. .. ..                         	make \$r16 = .* \(0x.*\);;

    .*:	10 00 d8 0f                                     	igoto \$r16;;

.* <___veneer>:
    .*:	.. .. 40 e0 .. .. .. ..                         	make \$r16 = .* \(0x.*\);;

    .*:	10 00 d8 0f                                     	igoto \$r16;;


.* <_start>:
    .*:	.. ff ff 17                                     	goto .* <___veneer>;;

    .*:	.. ff ff 17                                     	goto .* <___veneer>;;

    .*:	00 00 d0 0f                                     	ret ;;


Disassembly of section .foo:

.* <bar>:
.*:	00 00 d0 0f                                     	ret ;;


.* <bar2>:
.*:	00 00 d0 0f                                     	ret ;;

