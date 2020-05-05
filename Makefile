GNUEFI = /usr/local/include/efi
EFILIBS = /usr/local/lib/gnuefi

EFICFLAGS = -ffreestanding -I${GNUEFI} -I${GNUEFI}/x86_64 -I${GNUEFI}/protocol -c
EFICC = x86_64-w64-mingw32-gcc

EFILDFLAGS = -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main

EFICFILES = $(wildcard efi/*.c)
EFIOFILES = $(patsubst %.c,%.o,$(EFICFILES))

KERNELCFILES = $(wildcard kernel/*.c)

fat.img: BOOTX64.efi kernel.elf
	dd if=/dev/zero of=fat.img bs=1k count=1440
	mformat -i fat.img -f 1440 ::
	mmd -i fat.img ::/EFI
	mmd -i fat.img ::/EFI/BOOT
	mcopy -i fat.img BOOTX64.efi ::/EFI/BOOT
	mcopy -i fat.img kernel.elf ::/EFI/BOOT/kernel.elf
	mcopy -i fat.img assets/font.bmp ::/EFI/BOOT/font.bmp

BOOTX64.efi: ${EFIOFILES}
	${EFICC} ${EFILDFLAGS} -o BOOTX64.efi ${EFIOFILES} -lgcc

efi/%.o: efi/%.c
	${EFICC} ${EFICFLAGS} -o $@ $^

kernel.elf: ${KERNELCFILES}
	nasm -felf64 -o kernel/isr.o kernel/isr.asm
	gcc -ffreestanding -nodefaultlibs -nostdlib -e main kernel/isr.o ${KERNELCFILES} -o kernel.elf

clean:
	rm -f fat.img
	rm -f *.bin
	rm -f BOOTX64.efi
	rm -f */*.o
	rm -f kernel.elf

run: fat.img
	qemu-system-x86_64 -monitor stdio -no-reboot -no-shutdown -net none --bios /usr/share/ovmf/x64/OVMF.fd -drive file=fat.img,format=raw,index=0,media=disk
