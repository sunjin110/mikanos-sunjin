## ハマったところ

### error
lld-link: error: undefined symbol: gEfiLoadedImageProtocolGuid

### resolve
Loader.infのProtocolsに  
gEfiLoadedImageProtocolGuidを追加  

### 参考
https://zenn.dev/nnabeyang/scraps/fd9caa55515852

## memory mapが見れた  
```csv
Index, Type, Type(name), PhysicalStart, NumberOfPages, Attribute
0, 3, EfiBootServicesCode, 00000000, 1, F
1, 7, EfiConventionalMemory, 00001000, 9F, F
2, 7, EfiConventionalMemory, 00100000, 700, F
3, A, EfiACPIMemoryNVS, 00800000, 8, F
4, 7, EfiConventionalMemory, 00808000, 8, F
5, A, EfiACPIMemoryNVS, 00810000, F0, F
6, 4, EfiBootServicesData, 00900000, B00, F
7, 7, EfiConventionalMemory, 01400000, 2B36, F
8, 4, EfiBootServicesData, 03F36000, 20, F
9, 7, EfiConventionalMemory, 03F56000, 276E, F
10, 1, EfiLoaderCode, 066C4000, 2, F
11, 4, EfiBootServicesData, 066C6000, A, F
12, 9, EfiACPIReclaimMemory, 066D0000, 1, F
13, 4, EfiBootServicesData, 066D1000, 1BB, F
14, 3, EfiBootServicesCode, 0688C000, A8, F
15, A, EfiACPIMemoryNVS, 06934000, 12, F
16, 0, EfiReservedMemoryType, 06946000, 1C, F
17, 3, EfiBootServicesCode, 06962000, 10A, F
18, 6, EfiRuntimeServicesData, 06A6C000, 5, F
19, 5, EfiRuntimeServicesCode, 06A71000, 5, F
20, 6, EfiRuntimeServicesData, 06A76000, 5, F
21, 5, EfiRuntimeServicesCode, 06A7B000, 5, F
22, 6, EfiRuntimeServicesData, 06A80000, 5, F
23, 5, EfiRuntimeServicesCode, 06A85000, 7, F
24, 6, EfiRuntimeServicesData, 06A8C000, 8F, F
25, 4, EfiBootServicesData, 06B1B000, 730, F
26, 7, EfiConventionalMemory, 0724B000, 4, F
27, 4, EfiBootServicesData, 0724F000, 6, F
28, 7, EfiConventionalMemory, 07255000, 1, F
29, 4, EfiBootServicesData, 07256000, 7C5, F
30, 7, EfiConventionalMemory, 07A1B000, 1, F
31, 3, EfiBootServicesCode, 07A1C000, 17F, F
32, 5, EfiRuntimeServicesCode, 07B9B000, 30, F
33, 6, EfiRuntimeServicesData, 07BCB000, 24, F
34, 0, EfiReservedMemoryType, 07BEF000, 4, F
35, 9, EfiACPIReclaimMemory, 07BF3000, 8, F
36, A, EfiACPIMemoryNVS, 07BFB000, 4, F
37, 4, EfiBootServicesData, 07BFF000, 201, F
38, 7, EfiConventionalMemory, 07E00000, 8D, F
39, 4, EfiBootServicesData, 07E8D000, 20, F
40, 3, EfiBootServicesCode, 07EAD000, 20, F
41, 4, EfiBootServicesData, 07ECD000, 9, F
42, 3, EfiBootServicesCode, 07ED6000, 1E, F
43, 6, EfiRuntimeServicesData, 07EF4000, 84, F
44, A, EfiACPIMemoryNVS, 07F78000, 88, F
45, 6, EfiRuntimeServicesData, FFC00000, 400, 1

```
