#pragma once

#include <cstdint>
#include <array>

#include "error.hpp"

namespace pci {
    // CONFIG_ADDRESS レジスタの IOポートアドレス
    const uint16_t kConfigAddress = 0x0cf8;

    // CONFIG_DATA レジスタの IO ポートアドレス
    const uint16_t kConfigData = 0x0cfc;

    // CONFIG_ADDRESSに指定された整数を書き込む
    void WriteAddress(uint32_t address);

    // CONFIG_DATAに指定された整数を書き込む
    void WriteData(uint32_t value);

    // CONFIG_DATAから32ビット整数を読み込む
    uint32_t ReadData();

    // TODO
}