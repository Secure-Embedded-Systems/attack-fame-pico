__RAMSIZE__    = 0x10000;  /* 64 KB */
__RAMAPPSIZE__ = 0x08000;  /* 32 KB, size used for application code in RAM */

MEMORY
{
    FLASHBOOT (rx)  : ORIGIN = 0x00100000, LENGTH = 0x010000 /* 64K boot rom */
    FLASHAPP  (rx)  : ORIGIN = 0x00110000, LENGTH = 0x3F0000 /* rest of flash, 4 MB - 64K */
    RAMAPP (xrw)    : ORIGIN = 0x00000000, LENGTH = __RAMAPPSIZE__
    RAM    (xrw)    : ORIGIN = __RAMAPPSIZE__, LENGTH = __RAMSIZE__ - __RAMAPPSIZE__    
}

/* the following symbols are visible as external int variables in C */
__bootrom_start__ = ORIGIN(FLASHBOOT);
__bootrom_size__  = LENGTH(FLASHBOOT);
__approm_start__  = ORIGIN(FLASHAPP);
__approm_size__   = LENGTH(FLASHAPP);
__appram_start__  = ORIGIN(RAMAPP);
__appram_size__   = LENGTH(RAMAPP);
