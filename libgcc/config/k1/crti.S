
              .section .init
              .globl _init
              .type _init, function
              .align 8
_init:	
#ifdef __K1_64__	
              getd  $r0r1 = $ra
              addd  $r12r13 = $r12r13, -8
              ;;
              sd    16[$r12r13] = $r0r1
              ;;
              .previous

#else	
              get   $r0 = $ra
              add   $r12 = $r12, -8
              ;;
              sw    16[$r12] = $r0
              ;;
              .previous
#endif /* __K1_64__ */
	
              .section .fini
              .globl _fini
              .type _fini, function
              .align 8
_fini:
#ifdef __K1_64__	
              getd  $r0r1 = $ra
              addd  $r12r13 = $r12r13, -8
              ;;
              sd    16[$r12r13] = $r0r1
              ;;
              .previous

#else	
              get   $r0 = $ra
              add   $r12 = $r12, -8
              ;;
              sw    16[$r12] = $r0
              ;;
              .previous
#endif /* __K1_64__ */
