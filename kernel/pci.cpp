
#include "pci.hpp"

namespace {
    using namespace pci;

    // CONFIG_ADDRESS用の32ビット整数を生成する
    // このC++のコードはPCI（Peripheral Component Interconnect）アドレスを生成しています。PCIアドレスは、バス番号、デバイス番号、ファンクション番号、レジスタアドレスから成ります。これらの部分は通常、32ビットの値にパックされ、各部分は特定のビット位置に配置されます。
    // この関数は、それぞれのパラメータを正しい位置にシフトして、結果の32ビット値を生成します。各パラメータの位置は次のとおりです：
    // ビット31は有効ビットです。これはPCIアドレスの先頭に常に1を配置することを意味します。この値は shl(1, 31) によって生成されます。
    // ビット23-16はバス番号を表します。これは shl(bus, 16) によって生成されます。
    // ビット15-11はデバイス番号を表します。これは shl(device, 11) によって生成されます。
    // ビット10-8はファンクション番号を表します。これは shl(function, 8) によって生成されます。
    // 最後の部分はレジスタアドレスで、ビット7-2を占めます。これは直接設定され、上位2ビットは0にクリアされます（reg_addr & 0xfcu）。このマスク操作は、reg_addr が8ビットの値であると仮定し、上位2ビットを0に設定します。
    // これらの値はビット単位でOR演算子 | を用いて結合され、結果の32ビットPCIアドレスが生成されます。
    // 補足として、shl はラムダ関数です。これは関数内で定義された小さな無名の関数で、ビットシフト操作を行います。この関数は2つのパラメータを取り、x を bits ビットだけ左にシフトします。
    uint32_t MakeAddress(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg_addr) {
        auto shl = [](uint32_t x, unsigned int bits) {
            return x << bits;
        };

        return shl(1, 31) // enable bit
            | shl(bus, 16)
            | shl(device, 11)
            | shl(function, 8)
            | (reg_addr & 0xfcu);
    }
}