
/* Entry point */
ENTRY(_start)

KVX_BOOT_ADDRESS      = DEFINED(KVX_BOOT_ADDRESS) ? KVX_BOOT_ADDRESS : 0x0;
DEBUG_SIZE           = DEFINED(DEBUG_SIZE) ? DEBUG_SIZE : 32;

STACK_SIZE        = 20K;


/* Definition of sections */
SECTIONS
{
  . = KVX_BOOT_ADDRESS;

  /* The rest of the boot stuff */
  .boot :
  {
    *(.boot)
    . = ALIGN(8);
  }

  .text   ALIGN(8) :
  {
      KEEP(*(.init))
      KEEP(*(.fini))
      *(.text)
      *(.text.*)
    . = ALIGN(4096);
  }

  .rodata   ALIGN(8) :
  {
    *(.rodata)
    *(.rodata.*)
    . = ALIGN(8);
  }

  .data   ALIGN(8):
  {
    *(.data)
    *(.data.*)
    . = ALIGN(8);
  }

  .bss   ALIGN(8):
  {
    _bss_start = .;
    *(.bss)
    *(.bss.*)
    *(COMMON)
    *(.gnu.linkonce.b.*)
    . = ALIGN(8);
    _bss_end = .;
  }

  /* Stack area for RM */
  .stack ALIGN(0x1000) :
  {
     _rm_stack_end = ABSOLUTE(.);
     RM_STACK_END = ABSOLUTE(.);
     . += STACK_SIZE;
     _rm_stack_start = ABSOLUTE(.);
     RM_STACK_START = ABSOLUTE(.);
  }
}
