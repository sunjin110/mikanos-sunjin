
#include "pci.hpp"
#include "asmfunc.hpp"

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

    // devices[num_device]に情報を書き込み、num_deviceをincrement
    Error AddDevice(const Device &device) {
        if (num_device == devices.size()) {
            return MAKE_ERROR(Error::kFull);
        }

        devices[num_device] = device;
        num_device++;
        return SUCCESS();
    }

    Error ScanBus(uint8_t bus);

    // 指定のファンクションをdevicesに追加する
    // もしPCI-PCIブリッジならセカンダリバスに対しScanBusを実行する
    Error ScanFunction(uint8_t bus, uint8_t device, uint8_t function) {
        uint8_t header_type = ReadHeaderType(bus, device, function);
        ClassCode class_code = ReadClassCode(bus, device, function);
        Device dev = {
            .bus = bus,
            .device = device,
            .function = function,
            .header_type = header_type,
            .class_code = class_code,
        };
        if (Error err = AddDevice(dev)) {
            return err;
        }
        uint8_t base = class_code.base;
        uint8_t sub = class_code.sub;

        if (base == 0x06u && sub == 0x04u) {
            // standard PCI-PCI bridge
            uint32_t bus_numbers = ReadBusNumbers(bus, device, function);
            uint8_t secondary_bus = (bus_numbers >> 8) & 0xffu;
            return ScanBus(secondary_bus);
        }

        return SUCCESS();
    }

    Error ScanDevice(uint8_t bus, uint8_t device) {

        // 先に一つだけ実行
        if (Error err = ScanFunction(bus, device, 0)) {
            return err;
        }

        // それで終わりならOK
        if (IsSingleFunctionDevice(ReadHeaderType(bus, device, 0))) {
            return SUCCESS();
        }

        // 複数ある場合は、こっちを実行
        for (uint8_t function = 1; function < 8; function++) {
            if (ReadVendorId(bus, device, function) == 0xffffu) {
                continue;
            }
            if (Error err = ScanFunction(bus, device, function)) {
                return err;
            }
        }
        return SUCCESS();
    }

    // 指定のデバイス番号の各ファンクションをスキャンする

    // 指定のバス番号の各デバイスをスキャンする
    // 有効なデバイスを見つけたらScanDeviceを実行する
    Error ScanBus(uint8_t bus) {
        for (uint8_t device = 0; device < 32; device++) {
            if (ReadVendorId(bus, device, 0) == 0xffffu) {
                continue;
            }
            if (Error err = ScanDevice(bus, device)) {
                return err;
            }
        }
        return SUCCESS();
    }
}

namespace pci {
    void WriteAddress(uint32_t address) {
        IoOut32(kConfigAddress, address);
    }

    void WriteData(uint32_t value) {
        IoOut32(kConfigData, value);
    }

    uint32_t ReadData() {
        return IoIn32(kConfigData);
    }

    uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function) {
        WriteAddress(MakeAddress(bus, device, function, 0x00));
        return ReadData() & 0xffffu;
    }

    uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function) {
        WriteAddress(MakeAddress(bus, device, function, 0x00));
        return ReadData() >> 16;
    }

    uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function) {
        WriteAddress(MakeAddress(bus, device, function, 0x0c));
        return (ReadData() >> 16) & 0xffu;
    }

    ClassCode ReadClassCode(uint8_t bus, uint8_t device, uint8_t function) {
        WriteAddress(MakeAddress(bus, device, function, 0x08));
        uint32_t reg = ReadData();
        ClassCode class_code;
        class_code.base = (reg >> 24) & 0xffu;
        class_code.sub = (reg >> 16) & 0xffu;
        class_code.interface = (reg >> 8) & 0xffu;
        return class_code;
    }

    uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function) {
        WriteAddress(MakeAddress(bus, device, function, 0x18));
        return ReadData();
    }

    bool IsSingleFunctionDevice(uint8_t header_type) {
        return (header_type & 0x80u) == 0;
    }

    Error ScanAllBus() {
        num_device = 0;

        uint8_t header_type = ReadHeaderType(0, 0, 0);
        if (IsSingleFunctionDevice(header_type)) {
            return ScanBus(0);
        }

        for (uint8_t function = 1; function < 8; function++) {
            if (ReadVendorId(0, 0, function) == 0xffffu) {
                continue;
            }
            if (Error err = ScanBus(function)) {
                return err;
            }
        }
        return SUCCESS();
    }
}
