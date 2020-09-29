	.set noreorder

#include "pspimport.s"

IMPORT_START	"sceDmacplus_driver",0x00010000
IMPORT_FUNC	"sceDmacplus_driver",0x241601AE,sceDmacplusLcdcGetFormat
IMPORT_FUNC	"sceDmacplus_driver",0xCCCF6069,sceDmacplusLcdcGetBaseAddr
