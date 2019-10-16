BOARD="CONTROLLINO_Boards:avr:controllino_maxi"
PORT="/dev/ttyACM0"
NAME=home

.PHONY: all tftp upload 

all: $(NAME).elf

# also creates .hex and .bin
$(NAME).elf: $(NAME).ino $(wildcard *.h) $(wildcard *.cpp)
	arduino-cli compile -v -b $(BOARD) -o $(NAME)
	objcopy -I ihex -O binary $(NAME).hex $(NAME).bin

tftp: $(NAME).elf
	echo "@" | nc -w0 192.168.1.252 23
	sleep 5
	(echo "binary" ; echo "put $(NAME).bin") | tftp 192.168.1.252

upload: $(NAME).elf
	arduino-cli upload -v -b $(BOARD) -p $(PORT) -i $(NAME)
