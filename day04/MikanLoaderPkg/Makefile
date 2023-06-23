run:
	# shellで 
	@make build
	@make format
	@make mount
	@make run-qemu

mount_dist:
	# memoryMapを確認するためのもの
	hdiutil attach -mountpoint mnt sunjin.img

init:
	@make link
	@make source_edk2
	@make edk2_submodule_init
	@make edk_base_tool_build
	@make edit_conf_target

link:
	# 配置場所により適宜pathを変える
	rm ~/develop/edk2/MikanLoaderPkg
	ln -s ~/develop/mikanos-sunjin/day04/MikanLoaderPkg ~/develop/edk2/

source_edk2:
	cd ~/develop/edk2/ && source edksetup.sh

edk2_submodule_init:
	cd ~/develop/edk2 && git submodule update --init

edk2_base_tool_build:
	# edk2/BaseTools/Source/C/Common/Decompress.c の #define UINT8_MAX 0xffの前に #undef UINT8_MAXを記述
	cd ~/develop/edk2 && make -C /Users/yun_sunjin/develop/edk2/BaseTools/Source/C

edit_conf_target:
	./Edit_Conf_target.sh

build:
	cd ~/develop/mikanos-sunjin/day03/kernel && make build
	cd ~/develop/edk2 && build


format:
	qemu-img create -f raw sunjin.img 200M
	mkfs.fat -n 'SUNJIN OS' -s 2 -f 2 -R 32 -F 32 sunjin.img

mount:
	hdiutil attach -mountpoint mnt sunjin.img
	mkdir -p mnt/EFI/BOOT
	cp ~/develop/edk2/Build/MikanLoaderX64/DEBUG_CLANGPDB/X64/Loader.efi mnt/EFI/BOOT/BOOTX64.EFI
	cp ~/develop/mikanos-sunjin/day03/kernel/kernel.elf mnt/
	sleep 3
	hdiutil detach mnt

run-qemu:
	qemu-system-x86_64 -drive if=pflash,file=OVMF_CODE.fd -drive if=pflash,file=OVMF_VARS.fd -hda sunjin.img -monitor stdio