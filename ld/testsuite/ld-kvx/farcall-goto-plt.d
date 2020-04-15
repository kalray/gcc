#name: kvx-farcall-goto-plt
#source: farcall-goto-plt.s
#as:
#ld: -shared
#objdump: -dr
#...

Disassembly of section .plt:

.* <foo@plt-0x20>:
	...

.* <foo@plt>:
.*:	10 00 c4 0f                                     	get \$r16 = \$pc;;

.*:	10 4e 40 b8 40 00 04 18                         	ld \$r16 = [0-9]* \(0x[0-9a-b]*\)\[\$r16\];;

.*:	10 00 d8 0f                                     	igoto \$r16;;

.*:	00 00 00 00                                     	errop ;;


Disassembly of section .text:

.* <_start>:
	...
    .*:	02 00 00 10                                     	goto .* <__foo_veneer>;;

    .*:	00 00 d0 0f                                     	ret ;;


.* <__foo_veneer>:
    .*:	00 86 40 e0 00 00 00 00                         	make \$r16 = [0-9]* \(0x[0-9a-b]*\);;

    .*:	10 00 d8 0f                                     	igoto \$r16;;

    .*:	00 00 00 00                                     	errop ;;

