#ifndef PROTO_CM_H
#define PROTO_CM_H

#include "kbproto.h"
#include "proto_qmk.h"
#include "rawhid/hiddevice.h"

#include "zstring.h"
#include "zbinary.h"
using namespace LibChaos;

class ProtoCM : public ProtoQMK {
public:
    enum pok3r_rgb_cmd {
        RESET       = 0x4, //!< Reset command.
        RESET_BL    = 1,    //!< Reset to bootloader.
        RESET_FW    = 0,    //!< Reset to firmware.

        READ        = 0x1, //!< Read command.
        READ_VER    = 0x2,  //!< Read version string.
    };

public:
    //! Construct unopened device.
    ProtoCM(zu16 vid, zu16 pid, zu16 boot_pid);
    //! Construct open device from open HIDDevice.
    ProtoCM(zu16 vid, zu16 pid, zu16 boot_pid, bool builtin, ZPointer<HIDDevice> dev, zu32 fw_addr);

    ProtoCM(const ProtoCM &) = delete;
    ~ProtoCM();

    //! Find and open keyboard device.
    bool open();
    void close();
    bool isOpen() const;

    bool isBuiltin();

    //! Reset and re-open device.
    bool rebootFirmware(bool reopen = true);
    //! Reset to bootloader and re-open device.
    bool rebootBootloader(bool reopen = true);

    bool getInfo();

    //! Read the firmware version from the keyboard.
    ZString getVersion();

    KBStatus clearVersion();
    KBStatus setVersion(ZString version);

    //! Dump the contents of flash.
    ZBinary dumpFlash();
    //! Update the firmware.
    bool writeFirmware(const ZBinary &fwbin);

    bool eraseAndCheck();

    void test();

    //! Erase flash pages starting at \a start, ending on the page of \a end.
    bool eraseFlash(zu32 start, zu32 length);
    //! Read 64 bytes at \a addr.
    bool readFlash(zu32 addr, ZBinary &bin);
    //! Write 52 bytes at \a addr.
    bool writeFlash(zu32 addr, ZBinary bin);

    //! Get CRC of firmware.
    zu32 crcFlash(zu32 addr, zu32 len);

private:
    zu32 baseFirmwareAddr() const;
    //! Send command
    bool sendCmd(zu8 cmd, zu8 a1, ZBinary data = ZBinary());
    //! Recv command.
    bool recvCmd(ZBinary &data);
    //! Send command and recv response.
    bool sendRecvCmd(zu8 cmd, zu8 a1, ZBinary &data);

public:
    static void decode_firmware(ZBinary &bin);
    static void encode_firmware(ZBinary &bin);

    static void info_section(ZBinary data);

private:
    bool builtin;
    bool debug;
    bool nop;
    zu16 vid;
    zu16 pid;
    zu16 boot_pid;
    zu32 fw_addr;
};

#endif // PROTO_CM_H
