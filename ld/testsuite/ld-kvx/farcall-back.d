#name: kvx-farcall-back
#source: farcall-back.s
#as:
#ld: -Ttext 0x1000 --section-start .foo=0x80001000
#objdump: -dr

#...

Disassembly of section .text:

.* <__bar2_veneer>:
    1000:	00 0a 40 e0 08 00 20 00                         	make \$r16 = 2147491880 \(0x80002028\);;

    1008:	10 00 d8 0f                                     	igoto \$r16;;

    100c:	00 00 00 00                                     	errop ;;


.* <__bar3_veneer>:
    1010:	00 0c 40 e0 0c 00 20 00                         	make \$r16 = 2147495984 \(0x80003030\);;

    1018:	10 00 d8 0f                                     	igoto \$r16;;

    101c:	00 00 00 00                                     	errop ;;


.* <__bar1_veneer>:
    1020:	00 08 40 e0 04 00 20 00                         	make \$r16 = 2147487776 \(0x80001020\);;

    1028:	10 00 d8 0f                                     	igoto \$r16;;

    102c:	00 00 00 00                                     	errop ;;


.* <_start>:
    1030:	fc ff ff 1f                                     	call 1020 <__bar1_veneer>;;

    1034:	fb ff ff 17                                     	goto 1020 <__bar1_veneer>;;

    1038:	f2 ff ff 1f                                     	call 1000 <__bar2_veneer>;;

    103c:	f1 ff ff 17                                     	goto 1000 <__bar2_veneer>;;

    1040:	f4 ff ff 1f                                     	call 1010 <__bar3_veneer>;;

    1044:	f3 ff ff 17                                     	goto 1010 <__bar3_veneer>;;

    1048:	00 00 d0 0f                                     	ret ;;

	...

.* <_back>:
    204c:	00 00 d0 0f                                     	ret ;;

Disassembly of section .foo:

.* <___start_veneer>:
.*:	00 0c 40 e0 04 00 00 00                         	make \$r16 = 4144 \(0x1030\);;

.*:	10 00 d8 0f                                     	igoto \$r16;;

.*:	00 00 00 00                                     	errop ;;


.* <___back_veneer>:
.*:	00 13 40 e0 08 00 00 00                         	make \$r16 = 8268 \(0x204c\);;

.*:	10 00 d8 0f                                     	igoto \$r16;;

.*:	00 00 00 00                                     	errop ;;


.* <bar1>:
.*:	00 00 d0 0f                                     	ret ;;

.*:	f7 ff ff 17                                     	goto 80001000 <___start_veneer>;;

	...

.* <bar2>:
.*:	00 00 d0 0f                                     	ret ;;

.*:	f5 fb ff 17                                     	goto 80001000 <___start_veneer>;;

	...

.* <bar3>:
.*:	00 00 d0 0f                                     	ret ;;

.*:	f7 f7 ff 17                                     	goto 80001010 <___back_veneer>;;

