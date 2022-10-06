INCLUDE memorymap.ld

SECTIONS {
    /* text segment, loaded at RAMAPP but stored in FLASHAPP */
    .text :
    {
        . = ALIGN(4);
        *(.text)           
        *(.text*)          
        *(.rodata)         
        *(.rodata*)        
        *(.srodata)        
        *(.srodata*)       
        . = ALIGN(4);
    } >RAMAPP AT >FLASHAPP

    _etext = ORIGIN(FLASHAPP) + SIZEOF(.text);
    _sidata = _etext;
    
    /* initialized data, placed in flash but copied into RAM upon boot */
    .data : AT ( _etext )
    {
        . = ALIGN(4);
        _sdata = .;        
        _ram_start = .;    
        . = ALIGN(4);
        *(.data)           
        *(.data*)          
        *(.sdata)          
        *(.sdata*)         
        . = ALIGN(4);
        _edata = .;        
    } >RAM

    /* uninitialized data */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;   
    } >RAM

    .heap :
    {
        . = ALIGN(4);
        _heap_start = .;
    } >RAM
}
