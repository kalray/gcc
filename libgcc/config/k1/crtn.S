#ifdef __K1_64__
#define LOAD_PTR ld
#else
#define LOAD_PTR lwz
#endif

              .section .init
              LOAD_PTR    $r0 = 16[$r12]
              addd   $r12 = $r12, 8
              ;;
              set   $ra = $r0
              ;;
              ret
              ;;
              .previous

              .section .fini
              LOAD_PTR    $r0 = 16[$r12]
              addd  $r12 = $r12, 8
              ;;
              set  $ra = $r0
              ;;
              ret
              ;;
              .previous
