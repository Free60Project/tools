#!/usr/bin/env python
import usb
import struct
import StringIO

from xflash import XConfig

class DeviceNotFoundError(Exception):
    pass

CMD_DATA_READ   = 0x01
CMD_DATA_WRITE  = 0x02
CMD_DATA_INIT   = 0x03
CMD_DATA_DEINIT = 0x04
CMD_DATA_STATUS = 0x05
CMD_DATA_ERASE  = 0x06
CMD_DATA_EXEC   = 0x07
CMD_DEV_VERSION = 0x08
CMD_XSVF_EXEC   = 0x09
CMD_XBOX_PWRON  = 0x10
CMD_XBOX_PWROFF = 0x11
CMD_DEV_UPDATE  = 0xF0

class XFlash(object):
    def __init__(self, idVendor=0xffff, idProduct=0x4):
        self.idVendor = idVendor
        self.idProduct = idProduct
        self.dev = None
    
    def deviceFind(self):
        self.dev = usb.core.find(idVendor=self.idVendor,
                                 idProduct=self.idProduct)
        
        if self.dev is None:
            raise DeviceNotFoundError(self.idVendor, self.idProduct)
        
        self.dev.set_configuration()
        
        self.ep_out = 0x05
        self.ep_in  = 0x82
    
    def deviceReset(self):
        if self.dev is None:
            return
        
        self.dev.reset()
        self.dev.set_configuration()
    
    def deviceCmd(self, cmd, argA=0, argB=0, timeout=None):
        buf = struct.pack('<LL', argA, argB)
        self.dev.ctrl_transfer(bmRequestType    = usb.TYPE_VENDOR,
                               bRequest         = cmd,
                               wValue           = 0,
                               wIndex           = 0,
                               data_or_wLength  = buf,
                               timeout          = timeout)
    
    def deviceUpdate(self):
        try:
            self.deviceCmd(CMD_DEV_UPDATE)
        except: pass
    
    def deviceVersion(self):
        self.deviceCmd(CMD_DEV_VERSION, 0 , 4)
        buf = self.dev.read(self.ep_in, 4) # read 4 bytes
        buf = ''.join([chr(x) for x in buf])
        return struct.unpack('<L', buf)[0]
        
    def xsvfInit(self):
        self.deviceReset()
        self.deviceVersion()
        self.deviceVersion()
        return self.deviceVersion()
    
    def xsvfWrite(self, buf):
        self.deviceCmd(CMD_DATA_WRITE, 0, len(buf))
        self.dev.write(self.ep_out, buf)
    
    def xsvfExecute(self):
        self.deviceCmd(CMD_XSVF_EXEC, timeout=10000)
        return self.flashStatus()
    
    def __flash_status(self, cmd=None):
        if not cmd is None:
            self.deviceCmd(cmd)
        
        buf = self.dev.read(self.ep_in, 4) # read 4 bytes
        buf = ''.join([chr(x) for x in buf])
        return struct.unpack('<L', buf)[0]
    
    def flashInit(self):
        config = self.__flash_status(CMD_DATA_INIT)
        self.xc = XConfig.XConfig(config)
        return config
    
    def flashDeInit(self):
        self.deviceCmd(CMD_DATA_DEINIT)
    
    def flashStatus(self):
        return self.__flash_status(CMD_DATA_STATUS)
    
    def flashErase(self, block):
        self.deviceCmd(CMD_DATA_ERASE, block)
        if self.deviceVersion() >= 3:
            self.deviceCmd(CMD_DATA_EXEC, block)
        return self.flashStatus()
    
    def flashRead(self, block):
        self.deviceCmd(CMD_DATA_READ, block, 0x4200)
        buf = self.dev.read(self.ep_in, 0x4200)
        buf = ''.join([chr(x) for x in buf])
        status = self.flashStatus()
        return (status, buf)
    
    def flashReadBlock(self, block):
        """
        Read a full block, in whatever mode the flash config
        says is right
        """
        buf = ''
        status = 0
        adjblock = block * self.xc.blocksperlittle
        for b in range(adjblock, adjblock+self.xc.blocksperlittle):
            (stts, bff) = self.flashRead(b)
            status |= stts
            buf += bff
        return (status, buf)
    
    def flashWrite(self, block, buf):
        if len(buf) < 0x4200:
            raise ValueError('buffer is not long enough')
        self.deviceCmd(CMD_DATA_WRITE, block, 0x4200)
        self.dev.write(self.ep_out, buf)
        if self.deviceVersion() >= 3:
            self.deviceCmd(CMD_DATA_EXEC, block)
        return self.flashStatus()
    
    def flashWriteBlock(self, block, buf):
        """
        write a full block, in whatever mode the flash config
        says is right
        """
        status = 0
        adjblock = block * self.xc.blocksperlittle
        for b in range(self.xc.blocksperlittle):
            blk = adjblock + b
            bff = b * 0x4200
            status |= self.flashWrite(blk, buf[bff:bff+0x4200])
        return status
    def consolePowerOn(self):
        self.deviceCmd(CMD_XBOX_PWRON)
    
    def consolePowerOff(self):
        self.deviceCmd(CMD_XBOX_PWROFF)
    
    @classmethod
    def compress(cls, ib):
        ob = StringIO.StringIO()
        
        rs = re = rl = 0
        while rs < len(ib):
            # check for repeat
            if ib[rs] == ib[rs + 1]:
                re = rs
                while re < len(ib) and ib[rs] == ib[re]:
                    re += 1
                rl = re - rs
                ob.write(ib[rs:rs+2])
                ob.write(chr(rl - 2))
                rs += rl
            else:
                ob.write(ib[rs])
                rs += 1
        return ob.getvalue()

# def calcecc(data):
#   assert len(data) == 0x210
#   val = 0
#   for i in range(0x1066):
#     if not i & 31:
#       v = ~struct.unpack("<L", data[i/8:i/8+4])[0]
#     val ^= v & 1
#     v >>= 1
#     if val & 1:
#       val ^= 0x6954559
#     val >>= 1
# 
#   val = ~val
#   return data[:-4] + struct.pack("<L", (val << 6) & 0xFFFFFFFF)
# 
# def addecc(data, block = 0, off_8 = "\x00" * 4):
#   res = ""
#   while len(data):
#     d = (data[:0x200] + "\x00" * 0x200)[:0x200]
#     data = data[0x200:]
# 
#     d += struct.pack("<L4B4s4s", block / 32, 0, 0xFF, 0, 0, off_8, "\0\0\0\0")
#     d = calcecc(d)
#     block += 1
#     res += d
#   return res
