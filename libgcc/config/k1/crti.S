#include "32_64_macros.S"

              .section .init
              .globl _init
              .type _init, function
              .align 8
_init:	
              get   $r0 = $ra
              ADD_PTR  $r12 = $r12, -8
              ;;
              STR_PTR 16[$r12] = $r0
              ;;
              .previous
	
              .section .fini
              .globl _fini
              .type _fini, function
              .align 8
_fini:
              get   $r0 = $ra
              ADD_PTR   $r12 = $r12, -8
              ;;
              STR_PTR 16[$r12] = $r0
              ;;
              .previous
