DIRS=	bulkloop debugdevice eeprom i2c lights reset serial timers usbmon_c
ifneq ($(shell uname),Darwin)
DIRS+=  debugdevice_full_duplex
endif
     
.PHONY: dirs $(DIRS) clean
     
dirs: $(DIRS)
clean: $(DIRS)
     
$(DIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

