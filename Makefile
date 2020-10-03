TARGET = uvc
OBJS    = src/main.o src/utils.o src/format_conversion.o stubs/sceDmacplus_driver.o

INCDIR   = include
CFLAGS   = -G0 -Wall -O2 -MMD -MP
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS  = $(CFLAGS)
LIBDIR   =
LDFLAGS  =
LIBS     = -lpspusb_driver -lpspusbbus_driver
DEPS     = $(OBJS:.o=.d)

PSP_FW_VERSION = 660
BUILD_PRX = 1
EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = UVC - USB Video Class

EXTRA_CLEAN=$(DEPS)
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

copy: EBOOT.PBP
	@mkdir -p $(PSP_MS)/PSP/GAME/$(TARGET)
	@cp EBOOT.PBP $(PSP_MS)/PSP/GAME/$(TARGET)
	@sync
	@echo "Copied!"

copyprx: $(TARGET).prx
	@cp $^ $(PSP_MS)/seplugins/
	@sync
	@echo "Copied!"

-include $(DEPS)
