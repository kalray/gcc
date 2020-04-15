
tmpdir/dump:     file format elf32-kvx


Disassembly of section .plt:

00000160 <foo@plt-0x20>:
	...

00000180 <foo@plt>:
 180:	10 00 c4 0f                                     	get \$r16 = \$pc;;

.*:	10 2a 40 b0 40 00 04 18                         	lwz \$r16 = [0-9]* \(0x[0-9a-f]*\)\[\$r16\];;

 18c:	10 00 d8 0f                                     	igoto \$r16;;


Disassembly of section .text:

00000190 <_start>:
	...
1000018c:	02 00 00 10                                     	goto 10000194 <__foo_veneer>;;

10000190:	00 00 d0 0f                                     	ret ;;


10000194 <__foo_veneer>:
10000194:	00 60 40 e0 00 00 00 00                         	make \$r16 = 384 \(0x180\);;

1000019c:	10 00 d8 0f                                     	igoto \$r16;;

100001a0:	00 00 00 00                                     	errop ;;

