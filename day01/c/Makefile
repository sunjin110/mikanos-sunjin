
run:
	@make build
	@make format
	@make mount
	@make run-qemu

build:
	clang -target x86_64-pc-win32-coff -mno-red-zone -fno-stack-protector -fshort-wchar -Wall -o main.o -c main.c
	lld-link /subsystem:efi_application /entry:EfiMain /out:hello.efi main.o



format:
	qemu-img create -f raw sunjin.img 200M
	mkfs.fat -n 'SUNJIN OS' -s 2 -f 2 -R 32 -F 32 sunjin.img

mount:
	hdiutil attach -mountpoint mnt sunjin.img
	mkdir -p mnt/EFI/BOOT
	cp hello.efi mnt/EFI/BOOT/BOOTX64.EFI
	sleep 3
	hdiutil detach mnt

run-qemu:
	qemu-system-x86_64 -drive if=pflash,file=OVMF_CODE.fd -drive if=pflash,file=OVMF_VARS.fd -hda sunjin.img