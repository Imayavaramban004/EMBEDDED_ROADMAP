CC= arm-none-eabi-gcc
MACH= cortex-m4
CFLAGS= -c -mcpu=$(MACH) -std=gnu11 -O0
LDFLAGS= -nostdlib -T LED_ON.ld
LED_ON_Output_Makefile.o:LED_ON.c
	$(CC) $(CFLAGS) -o $@ $^ 
startup_LED_ON.o:startup_LED_ON.c
	$(CC) $(CFLAGS) -o $@ $^
final.elf: LED_ON.o startup_LED_ON.o
	$(CC) $(LDFLAGS) -o $@ $^
clean:
	rm -rf *.o *.elf
all:LED_ON_Output_Makefile.o startup_LED_ON.o final.elf
