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

    // ベンダIDレジスタを読み取る
    uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function);

    // デバイスIDレジスタを読み取る
    uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function);

    // ヘッダタイプレジスタを読み取る
    uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function);

    // クラスコードレジスタを読み取る
    //     返される 32 ビット整数の構造は次の通り．
    //   - 31:24 : ベースクラス
    //   - 23:16 : サブクラス
    //   - 15:8  : インターフェース
    //   - 7:0   : リビジョン
    uint32_t ReadClassCode(uint8_t bus, uint8_t device, uint8_t function);

    // バス番号レジスタを読み取る（ヘッダタイプ 1 用）
    //     返される 32 ビット整数の構造は次の通り．
    //   - 23:16 : サブオーディネイトバス番号
    //   - 15:8  : セカンダリバス番号
    //   - 7:0   : リビジョン番号
    uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function);

    // 単一ファンクションの場合に真を返す
    bool IsSingleFunctionDevice(uint8_t header_type);

    struct Device {
        uint8_t bus, device, function, header_type;
    };

    // ScanAllBus()により発見されたPCIデバイスの一覧
    inline std::array<Device, 32> devices;

    // devicesの有効な要素数
    inline int num_device;

    // PCIデバイスを全て探索し devicesに格納する
    Error ScanAllBus();
}
