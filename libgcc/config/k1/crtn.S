#ifdef __K1_64__
              .section .init
              ld    $r0r1 = 16[$r12r13]
              addd   $r12r13 = $r12r13, 8
              ;;
              setd   $ra = $r0r1
              ;;
              ret
              ;;
              .previous

              .section .fini
              ld    $r0r1 = 16[$r12r13]
              addd  $r12r13 = $r12r13, 8
              ;;
              setd  $ra = $r0r1
              ;;
              ret
              ;;
              .previous
#else
              .section .init
              lw    $r0 = 16[$r12]
              add   $r12 = $r12, 8
              ;;
              set   $ra = $r0
              ;;
              ret
              ;;
              .previous

              .section .fini
              lw    $r0 = 16[$r12]
              add   $r12 = $r12, 8
              ;;
              set   $ra = $r0
              ;;
              ret
              ;;
              .previous
#endif /* __K1_64__ */
