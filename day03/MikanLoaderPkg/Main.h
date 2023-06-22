#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/PrintLib.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Protocol/DiskIo2.h>
#include  <Protocol/BlockIo.h>

struct MemoryMap {
    UINTN buffer_size;
    VOID* buffer;
    UINTN map_size;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
};


