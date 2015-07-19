DIRS=	bulkloop eeprom i2c lights reset serial serial-to-cdc-acm timers
     
.PHONY: dirs $(DIRS) clean
     
dirs: $(DIRS)
clean: $(DIRS)
     
$(DIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

