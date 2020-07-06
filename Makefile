BOARD="CONTROLLINO_Boards:avr:controllino_maxi"
PORT="/dev/ttyACM0"
NAME=smart-home.ino

.PHONY: all tftp upload 

all: $(NAME).elf

# also creates .hex and .bin
$(NAME).elf: $(NAME) $(wildcard *.h) $(wildcard *.cpp)
	echo '#define BUILD_TAG "$(shell git describe --always --dirty)"' > version.h
	echo '#define BUILD_DATE "$(shell date +%Y-%m-%d)"' >> version.h
	echo '#define BUILD_TIME "$(shell date +%H:%M:%S)"' >> version.h
	arduino-cli compile -b $(BOARD) --output-dir .
	objcopy -I ihex -O binary $(NAME).hex $(NAME).bin

tftp: $(NAME).elf
	echo "@" | nc -w0 192.168.1.252 23 && sleep 3 || true
	(echo "binary" ; echo "trace" ; echo "verbose" ; echo "put $(NAME).bin") | tftp 192.168.1.252

upload: $(NAME).elf
	arduino-cli upload -v -b $(BOARD) -p $(PORT) -i $(NAME)
