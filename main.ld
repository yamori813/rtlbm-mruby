SECTIONS
{
  . = 0x80010000;
  .text : {
    start.o(.text)
    *(.text)
  }
  .init : {
    KEEP (*(.init))
  }
  .fini : {
    KEEP (*(.fini))
  }
  .rdata : { *(.rdata) }
  .data : { *(.data) }
  _gp = ALIGN(16);
  .sdata : { *(.sdata) }
  . = ALIGN(4);
   PROVIDE (edata  =  .);
   _edata  =  .;
  _fbss = .;
  .sbss : { *(.sbss) }
  .bss : { *(.bss) }
  . = ALIGN(32);
  . = ALIGN(32);
  PROVIDE (end = .);
  _end = .;
/*
  .preinit_array    :
  {
    PROVIDE (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE (__preinit_array_end = .);
  }
  .init_array    :
  {
     PROVIDE (__init_array_start = .);
     KEEP (*(SORT(.init_array.*)))
     KEEP (*(.init_array))
     PROVIDE (__init_array_end = .);
  }
  .fini_array    :
  {
    PROVIDE (__fini_array_start = .);
    KEEP (*(.fini_array))
    KEEP (*(SORT(.fini_array.*)))
    PROVIDE (__fini_array_end = .);
  }
*/

}
