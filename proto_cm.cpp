#include "proto_cm.h"
#include "zlog.h"

#define UPDATE_PKT_LEN      64

#define UPDATE_ERROR        0xaaff

#define VER_ADDR            0x3000

#define WAIT_SLEEP          5

#define HEX(A) (ZString::ItoS((zu64)(A), 16))

ProtoCM::ProtoCM(zu16 vid_, zu16 pid_, zu16 boot_pid_) :
    ProtoCM(vid_, pid_, boot_pid_, false, new HIDDevice, 0)
{

}

ProtoCM::ProtoCM(zu16 vid_, zu16 pid_, zu16 boot_pid_, bool builtin_, ZPointer<HIDDevice> dev_, zu32 fw_addr_) :
    ProtoQMK(PROTO_CM, dev_),
    builtin(builtin_), debug(false), nop(false),
    vid(vid_), pid(pid_), boot_pid(boot_pid_),
    fw_addr(fw_addr_)
{
    //dev->setStream(true);
}

ProtoCM::~ProtoCM(){

}

bool ProtoCM::open(){
    // Try firmware vid and pid
    if(dev->open(vid, pid, UPDATE_USAGE_PAGE, UPDATE_USAGE)){
        builtin = false;
        return true;
    }
    // Try builtin vid and pid
    if(dev->open(vid, boot_pid, UPDATE_USAGE_PAGE, UPDATE_USAGE)){
        builtin = true;
        return true;
    }
    return false;
}

void ProtoCM::close(){
    dev->close();
}

bool ProtoCM::isOpen() const {
    return dev->isOpen();
}

bool ProtoCM::isBuiltin(){
    return builtin;
}

bool ProtoCM::rebootFirmware(bool reopen){
    //if(!builtin){
    //    LOG("In Firmware");
    //    return true;
    //}

    LOG("Reset to Firmware");
    if(!sendCmd(RESET, RESET_FW))
        return false;
    close();

    if(reopen){
        ZThread::sleep(WAIT_SLEEP);

        // does not work
        if(!open()){
            ELOG("open error");
            return false;
        }

        if(builtin)
            return false;
    }
    return true;
}

bool ProtoCM::rebootBootloader(bool reopen){
    //if(builtin){
    //    LOG("In Bootloader");
    //    return true;
    //}

    LOG("Reset to Bootloader");
    if(!sendCmd(RESET, RESET_BL))
        return false;
    close();

    if(reopen){
        ZThread::sleep(WAIT_SLEEP);

        // does not work
        if(!open()){
            ELOG("open error");
            return false;
        }

        if(!builtin)
            return false;
    }
    return true;
}

bool ProtoCM::getInfo(){
    // not implemented
    return false;
}

ZString ProtoCM::getVersion(){
    DLOG("getVersion");

    ZBinary data;
    if(!sendRecvCmd(READ, READ_VER, data))
        return "ERROR";
    //RLOG(data.dumpBytes(3, 8));

    ZBinary tst;
    tst.fill(0xFF, 60);

    ZString ver;
    if(data.getSub(4) == tst){
        ver = "CLEARED";
    } else {
        data.seek(4);
        zu32 len = MIN(data.readleu32(), 60U);
        ver.parseUTF8(data.raw()+4, len);
    }
    DLOG("version: " << ver);

    return ver;
}

KBStatus ProtoCM::clearVersion(){
    return ERR_NOT_IMPLEMENTED;
}

KBStatus ProtoCM::setVersion(ZString version){
    return ERR_NOT_IMPLEMENTED;
}

ZBinary ProtoCM::dumpFlash(){
    // not implemented
    return false;
}

bool ProtoCM::writeFirmware(const ZBinary &fwbinin){
    // not implemented
    return false;
}

bool ProtoCM::eraseAndCheck(){
    // not implemented
    return false;
}

void ProtoCM::test(){
    // not implemented
    return;
}

bool ProtoCM::eraseFlash(zu32 start, zu32 length){
    // not implemented
    return false;
}

bool ProtoCM::readFlash(zu32 addr, ZBinary &bin){
    // not implemented
    return false;
}

bool ProtoCM::writeFlash(zu32 addr, ZBinary bin){
    // not implemented
    return false;
}

zu32 ProtoCM::crcFlash(zu32 addr, zu32 len){
    // not implemented
    return 0;
}

zu32 ProtoCM::baseFirmwareAddr() const {
    return fw_addr;
}

bool ProtoCM::sendCmd(zu8 cmd, zu8 a1, ZBinary data){
    if(data.size() > 52){
        ELOG("bad data size");
        return false;
    }

    ZBinary packet(UPDATE_PKT_LEN);
    packet.fill(0);
    packet.writeu8(cmd);    // command
    packet.writeu8(a1);     // argument
    packet.seek(4);
    packet.write(data);     // data

    DLOG("send:");
    DLOG(ZLog::RAW << packet.dumpBytes(4, 8));

    // Send packet
    if(!dev->send(packet, (cmd == RESET ? true : false))){
        ELOG("send error");
        return false;
    }
    return true;
}

bool ProtoCM::recvCmd(ZBinary &data){
    // Recv packet
    data.resize(UPDATE_PKT_LEN);
    if(!dev->recv(data)){
        ELOG("recv error");
        return false;
    }

    if(data.size() != UPDATE_PKT_LEN){
        DLOG("bad recv size");
        return false;
    }

    DLOG("recv:");
    DLOG(ZLog::RAW << data.dumpBytes(4, 8));

    return true;
}

bool ProtoCM::sendRecvCmd(zu8 cmd, zu8 a1, ZBinary &data){
    if(!sendCmd(cmd, a1, data))
        return false;
    return recvCmd(data);
}

void ProtoCM::decode_firmware(ZBinary &bin){
    // not implemented
    return;
}

void ProtoCM::encode_firmware(ZBinary &bin){
    // not implemented
    return;
}

void ProtoCM::info_section(ZBinary data){
    // not implemented
    return;
}
