INCLUDE memorymap.ld

SECTIONS {
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
        _etext = .;        
        _sidata = _etext;  
    } >FLASHBOOT

    /* initialized data, placed in flash but copied into RAM upon boot */
    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;        
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
