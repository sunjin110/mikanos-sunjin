# mikanos-sunjin

## macで実装するために必ずみる必要があるもの  

https://qiita.com/yamoridon/items/4905765cc6e4f320c9b5#34-%E3%83%96%E3%83%BC%E3%83%88%E3%83%AD%E3%83%BC%E3%83%80%E3%81%8B%E3%82%89%E3%83%94%E3%82%AF%E3%82%BB%E3%83%AB%E3%82%92%E6%8F%8F%E3%81%8F%E3%81%AE%E8%A3%9C%E8%B6%B3

## readelfを入れる  
https://qiita.com/kusuwada/items/9b11b5db17442fe634f0  

```sh
brew update
brew install binutils

echo 'export PATH="/opt/homebrew/opt/binutils/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

breadelf --version
```

```sh
🐼 day03>readelf -h kernel/kernel.elf 
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x101120
  Start of program headers:          64 (bytes into file)
  Start of section headers:          1072 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         4
  Size of section headers:           64 (bytes)
  Number of section headers:         14
  Section header string table index: 12
  ```