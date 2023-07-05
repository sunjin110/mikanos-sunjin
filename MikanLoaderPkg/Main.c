#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/DiskIo2.h>
#include <Protocol/BlockIo.h>
#include <Guid/FileInfo.h>

#include "frame_buffer_config.hpp"
#include "elf.hpp"

struct MemoryMap
{
  UINTN buffer_size;
  VOID *buffer;
  UINTN map_size;
  UINTN map_key;
  UINTN descriptor_size;
  UINT32 descriptor_version;
};

EFI_STATUS GetMemoryMap(struct MemoryMap *map)
{
  if (map->buffer == NULL)
  {
    return EFI_BUFFER_TOO_SMALL;
  }

  map->map_size = map->buffer_size;
  return gBS->GetMemoryMap(
      &map->map_size,
      (EFI_MEMORY_DESCRIPTOR *)map->buffer,
      &map->map_key,
      &map->descriptor_size,
      &map->descriptor_version);
}

const CHAR16 *GetMemoryTypeUnicode(EFI_MEMORY_TYPE type)
{
  switch (type)
  {
  case EfiReservedMemoryType:
    return L"EfiReservedMemoryType";
  case EfiLoaderCode:
    return L"EfiLoaderCode";
  case EfiLoaderData:
    return L"EfiLoaderData";
  case EfiBootServicesCode:
    return L"EfiBootServicesCode";
  case EfiBootServicesData:
    return L"EfiBootServicesData";
  case EfiRuntimeServicesCode:
    return L"EfiRuntimeServicesCode";
  case EfiRuntimeServicesData:
    return L"EfiRuntimeServicesData";
  case EfiConventionalMemory:
    return L"EfiConventionalMemory";
  case EfiUnusableMemory:
    return L"EfiUnusableMemory";
  case EfiACPIReclaimMemory:
    return L"EfiACPIReclaimMemory";
  case EfiACPIMemoryNVS:
    return L"EfiACPIMemoryNVS";
  case EfiMemoryMappedIO:
    return L"EfiMemoryMappedIO";
  case EfiMemoryMappedIOPortSpace:
    return L"EfiMemoryMappedIOPortSpace";
  case EfiPalCode:
    return L"EfiPalCode";
  case EfiPersistentMemory:
    return L"EfiPersistentMemory";
  case EfiMaxMemoryType:
    return L"EfiMaxMemoryType";
  default:
    return L"InvalidMemoryType";
  }
}
// #@@range_end(get_memory_type)

// #@@range_begin(save_memory_map)
EFI_STATUS SaveMemoryMap(struct MemoryMap *map, EFI_FILE_PROTOCOL *file)
{
  CHAR8 buf[256];
  UINTN len;

  CHAR8 *header =
      "Index, Type, Type(name), PhysicalStart, NumberOfPages, Attribute\n";
  len = AsciiStrLen(header);
  file->Write(file, &len, header);

  Print(L"map->buffer = %08lx, map->map_size = %08lx\n",
        map->buffer, map->map_size);

  EFI_PHYSICAL_ADDRESS iter;
  int i;
  for (iter = (EFI_PHYSICAL_ADDRESS)map->buffer, i = 0;
       iter < (EFI_PHYSICAL_ADDRESS)map->buffer + map->map_size;
       iter += map->descriptor_size, i++)
  {
    EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)iter;
    len = AsciiSPrint(
        buf, sizeof(buf),
        "%u, %x, %-ls, %08lx, %lx, %lx\n",
        i, desc->Type, GetMemoryTypeUnicode(desc->Type),
        desc->PhysicalStart, desc->NumberOfPages,
        desc->Attribute & 0xffffflu);
    file->Write(file, &len, buf);
  }

  return EFI_SUCCESS;
}

EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL **root)
{
  EFI_LOADED_IMAGE_PROTOCOL *loaded_image;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;

  gBS->OpenProtocol(
      image_handle,
      &gEfiLoadedImageProtocolGuid,
      (VOID **)&loaded_image,
      image_handle,
      NULL,
      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

  gBS->OpenProtocol(
      loaded_image->DeviceHandle,
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID **)&fs,
      image_handle,
      NULL,
      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

  fs->OpenVolume(fs, root);

  return EFI_SUCCESS;
}

// Graphics Output Protocolを取得する
EFI_STATUS OpenGOP(EFI_HANDLE image_handle, EFI_GRAPHICS_OUTPUT_PROTOCOL **gop)
{
  UINTN num_gop_handles = 0;
  EFI_HANDLE *gop_handles = NULL;
  gBS->LocateHandleBuffer(
      ByProtocol,
      &gEfiGraphicsOutputProtocolGuid,
      NULL,
      &num_gop_handles,
      &gop_handles);

  gBS->OpenProtocol(
      gop_handles[0],
      &gEfiGraphicsOutputProtocolGuid,
      (VOID **)gop,
      image_handle,
      NULL,
      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

  FreePool(gop_handles);

  return EFI_SUCCESS;
}

const CHAR16 *GetPixelFormatUnicode(EFI_GRAPHICS_PIXEL_FORMAT fmt)
{
  switch (fmt)
  {
  case PixelRedGreenBlueReserved8BitPerColor:
    return L"PixelRedGreenBlueReserved8BitPerColor";
  case PixelBlueGreenRedReserved8BitPerColor:
    return L"PixelBlueGreenRedReserved8BitPerColor";
  case PixelBitMask:
    return L"PixelBitMask";
  case PixelFormatMax:
    return L"PixelFormatMax";
  default:
    return L"InvalidPixelFormat";
  }
}

void Halt(void)
{
  while (1)
    __asm__("hlt");
}

EFI_STATUS EFIAPI UefiMain(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table)
{

  EFI_STATUS status;

  // memmapの内容をファイルに書き込む
  CHAR8 memmap_buf[4096 * 4];
  struct MemoryMap memmap = {sizeof(memmap_buf), memmap_buf, 0, 0, 0, 0};
  status = GetMemoryMap(&memmap);
  if (EFI_ERROR(status))
  {
    Print(L"failed to get memory map: %r\n", status);
    Halt();
  }

  EFI_FILE_PROTOCOL *root_dir;
  status = OpenRootDir(image_handle, &root_dir);
  if (EFI_ERROR(status))
  {
    Print(L"failed to open root directory: %r\n", status);
    Halt();
  }

  EFI_FILE_PROTOCOL *memmap_file;
  status = root_dir->Open(
      root_dir, &memmap_file, L"\\memmap",
      EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
  if (EFI_ERROR(status))
  {
    Print(L"failed to open file '\\memmap': %r\n", status);
    Print(L"Ignored.");
  }
  else
  {
    status = SaveMemoryMap(&memmap, memmap_file);
    if (EFI_ERROR(status))
    {
      Print(L"failed to save memory map: %r\n", status);
      Halt();
    }
    status = memmap_file->Close(memmap_file);
    if (EFI_ERROR(status))
    {
      Print(L"failed to close memory map: %r\n", status);
      Halt();
    }
  }

  // gop
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  status = OpenGOP(image_handle, &gop);
  if (EFI_ERROR(status))
  {
    Print(L"failed to open GOP: %r\n", status);
    Halt();
  }

  Print(L"Resolution: %ux%u, Pixcel Format: %s, %u pixels/line\n",
        gop->Mode->Info->HorizontalResolution,
        gop->Mode->Info->VerticalResolution,
        GetPixelFormatUnicode(gop->Mode->Info->PixelFormat),
        gop->Mode->Info->PixelsPerScanLine);

  Print(L"Frame Buffer: 0x%0lx - 0x%0lx, Size: %lu bytes\n",
        gop->Mode->FrameBufferBase,
        gop->Mode->FrameBufferBase + gop->Mode->FrameBufferSize,
        gop->Mode->FrameBufferSize);

  // kernel.elfを呼び出す
  // kernel fileを読み込む
  EFI_FILE_PROTOCOL *kernel_file;
  status = root_dir->Open(
      root_dir, &kernel_file, L"\\kernel.elf",
      EFI_FILE_MODE_READ, 0);
  if (EFI_ERROR(status))
  {
    Print(L"failed to open file '\\kernel.elf': %r\n", status);
    Halt();
  }

  // 開いたファイルの全体を読み込むためのメモリを確保する
  UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12; // CHAR16 * 12分多めに確保する
  UINT8 file_info_buffer[file_info_size];
  status = kernel_file->GetInfo(
      kernel_file, &gEfiFileInfoGuid,
      &file_info_size, file_info_buffer);
  if (EFI_ERROR(status))
  {
    Print(L"failed to get file information: %r\n", status);
    Halt();
  }

  EFI_FILE_INFO *file_info = (EFI_FILE_INFO *)file_info_buffer;
  UINTN kernel_file_size = file_info->FileSize;

  EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;
  status = gBS->AllocatePages(
      AllocateAddress, EfiLoaderData,
      (kernel_file_size + 0xfff) / 0x1000, &kernel_base_addr);
  if (EFI_ERROR(status))
  {
    Print(L"failed to allocate pages: %r\n", status);
    Halt();
  }

  status = kernel_file->Read(kernel_file, &kernel_file_size, (VOID *)kernel_base_addr);
  if (EFI_ERROR(status))
  {
    Print(L"kernel error: %r\n", status);
    Halt();
  }

  Print(L"Kernel: 0x%0lx (%lu bytes)\n", kernel_base_addr, kernel_file_size);

  // 今まで動いていたBoot Serviceを停止する
  status = gBS->ExitBootServices(image_handle, memmap.map_key);
  if (EFI_ERROR(status))
  {
    status = GetMemoryMap(&memmap);
    if (EFI_ERROR(status))
    {
      Print(L"failed to get memory map: %r\n", status);
      Halt();
    }
    status = gBS->ExitBootServices(image_handle, memmap.map_key);
    if (EFI_ERROR(status))
    {
      Print(L"Could not exit bool service: %r\n", status);
      Halt();
    }
  }

  // カーネルを起動する
  // macだとentrypointのアドレスがちょい違うけど、いけてるんやろというやつhttps://zenn.dev/karaage0703/scraps/b2705131673377
  UINT64 entry_addr = *(UINT64 *)(kernel_base_addr + 24);

  // ピクセルを自由に描くために必要な情報を渡す処理
  enum PixelFormat pixel_format = 0;
  switch (gop->Mode->Info->PixelFormat)
  {
  case PixelRedGreenBlueReserved8BitPerColor:
    pixel_format = kPixelRGBResv8BitPerColor;
    break;
  case PixelBlueGreenRedReserved8BitPerColor:
    pixel_format = kPixelBGRResv8BitPerColor;
    break;
  default:
    Print(L"Unimplemented pixel format: %d\n", gop->Mode->Info->PixelFormat);
    Halt();
  }
  struct FrameBufferConfig config = {
      .frame_buffer = (UINT8 *)gop->Mode->FrameBufferBase,
      .pixels_per_scan_line = gop->Mode->Info->PixelsPerScanLine,
      .horizontal_resolution = gop->Mode->Info->HorizontalResolution,
      .vertical_resolution = gop->Mode->Info->VerticalResolution,
      .pixel_format = pixel_format,
  };

  // typedef void EntryPointType(UINT64, UINT64);
  // https://qiita.com/yamoridon/items/4905765cc6e4f320c9b5#34-%E3%83%96%E3%83%BC%E3%83%88%E3%83%AD%E3%83%BC%E3%83%80%E3%81%8B%E3%82%89%E3%83%94%E3%82%AF%E3%82%BB%E3%83%AB%E3%82%92%E6%8F%8F%E3%81%8F%E3%81%AE%E8%A3%9C%E8%B6%B3
  // typedef void __attribute__((sysv_abi)) EntryPointType(UINT64, UINT64);
  // typedef void __attribute__((sysv_abi)) EntryPointType(const struct FrameBufferConfig*);

  typedef void EntryPointType(const struct FrameBufferConfig *);

  EntryPointType *entry_point = (EntryPointType *)entry_addr;
  // entry_point();
  entry_point(&config);

  Print(L"All done\n");

  Halt();
  return EFI_SUCCESS;
}
