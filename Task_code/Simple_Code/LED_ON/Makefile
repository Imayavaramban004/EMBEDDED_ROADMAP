CC= arm-none-eabi-gcc
MACH= cortex-m4
CFLAGS= -c -mcpu=$(MACH) -std=gnu11 -o0
LED_ON_Output_Makefile.o:LED_ON.c
	$(CC) $(CFLAGS) -o $@ $^ 
startup_LED_ON.o:startup_LED_ON.c
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm -rf *.o *.elf
all:LED_ON_Output_Makefile.o startup_LED_ON.o
