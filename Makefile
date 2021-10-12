monitor:
	platformio device monitor

upload:
	platformio run --target upload

debug:
	platformio debug --interface gdb