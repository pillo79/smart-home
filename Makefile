BOARD="CONTROLLINO_Boards:avr:controllino_maxi"
PORT="/dev/ttyACM0"
NAME=home

.PHONY: all upload
 
all: $(NAME).elf
$(NAME).elf: $(NAME).ino $(wildcard *.h) $(wildcard *.cpp)
	arduino-cli compile -b $(BOARD) -o $(NAME)

upload: $(NAME.elf)
	arduino-cli upload -b $(BOARD) -p $(PORT) -i $(NAME)
