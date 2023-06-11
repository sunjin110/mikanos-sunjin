typedef unsigned short CHAR16;
typedef unsigned long long EFI_STATUS;
typedef void *EFI_HANDLE;

struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef EFI_STATUS (*EFI_TEXT_STRING) (
    struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *self,
    CHAR16 *string);

typedef struct _EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL {
    void *dummy;  
    EFI_TEXT_STRING output_string;
} EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef struct {
    char dummy[52];
    EFI_HANDLE console_out_handle;
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *con_out;
} EFI_SYSTEM_TABLE;

EFI_STATUS EfiMain(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
    system_table->con_out->output_string(system_table->con_out, L"Hello, Sunjin\n");
    system_table->con_out->output_string(system_table->con_out, L"Let's develop OS\n");
    // halt
    while (1);
    return 0;
}
