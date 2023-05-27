ENTRY(nomain)

SECTIONS
{
    . = 0x08048000 + SIZEOF_HEADERS;
    mysection : {*(.text) *(.data) *(.rodata)}
}