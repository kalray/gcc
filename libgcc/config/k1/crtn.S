#include "32_64_macros.S"

              .section .init
              LOAD_PTR    $r0 = 16[$r12]
              ADD_PTR   $r12 = $r12, 8
              ;;
#ifndef __K1_64__
	      sxwd $r12 = $r12
              ;;
#endif

              set   $ra = $r0
              ;;
              ret
              ;;
              .previous

              .section .fini
              LOAD_PTR    $r0 = 16[$r12]
              ADD_PTR  $r12 = $r12, 8
              ;;
#ifndef __K1_64__
	      sxwd $r12 = $r12
              ;;
#endif

              set  $ra = $r0
              ;;
              ret
              ;;
              .previous
