
class XConfigParseError(Exception):
    pass

class XConfig(object):
    def __init__(self, config):
        self.config = config
        self.controllertype = config >> 17 & 3
        self.blocktype = config >> 4 & 3
        
        self.pagesz = 0x200
        self.metasz = 0x10
        self.metatype = 0
        self.blocksz = 0
        self.sizeblocks = 0
        self.sizesmallblocks = 0
        self.fsblocks = 0
        ctype = self.controllertype
        btype = self.blocktype
        
        if ctype == 0:
            self.metatype = 0
            self.blocksz = 0x20
            if btype == 0:
                msg = 'nand type 0:0 is invalid'
                raise XConfigParseError(msg)
            elif btype == 1:
                self.sizeblocks = 0x400
                self.fsblocks = 0x3E0
            elif btype == 2:
                self.sizeblocks = 0x800
                self.fsblocks = 0x7C0
            elif btype == 3:
                self.sizeblocks = 0x1000
                self.fsblocks = 0xF80
        elif ctype == 1 and btype == 0:
            msg = 'nand type 1:0 is invalid'
            raise XConfigParseError(msg)
        elif ctype in (1, 2) and btype in (0, 1):
            self.metatype = 1
            self.blocksz = 0x20
            if btype == 0 or (btype == 1 and ctype == 1):
                self.sizeblocks = 0x400
                self.fsblocks = 0x3E0
            elif ctype == 2 and btype == 1:
                self.sizeblocks = 0x1000
                self.fsblocks = 0xF80
        elif ctype in (1, 2) and btype in (2, 3):
            self.metatype = 2
            if btype == 2:
                self.blocksz = 0x100
                self.sizeblocks = 1 << ((config >> 19 & 3) + (config >> 21 & 15) + 23) >> 17
                self.fsblocks = 0x1E0
            elif btype == 3:
                self.blocksz = 0x200
                self.sizeblocks = 1 << ((config >> 19 & 3) + (config >> 21 & 15) + 23) >> 18
                self.fsblocks = 0xF0
        else:
            msg = 'controller type %s is invalid' % ctype
            raise XConfigParseError(msg)
        self.sizesmallblocks = self.sizeblocks * (self.blocksz / 0x20)
        self.blocksperlittle = self.blocksz / 0x20
    
    def printConfig(self):
        fmt = """
        FlashConfig:\t%X
        PageSize:\t%X
        MetaSize:\t%X
        MetaType:\t%X
        BlockSize:\t%X
        SizeInBlocks:\t%X
        SizeInSBlocks:\t%X
        FileBlocks:\t%X
        """ % (
            self.config,
            self.pagesz,
            self.metasz,
            self.metatype,
            self.blocksz,
            self.sizeblocks,
            self.sizesmallblocks,
            self.fsblocks,
        )
        print fmt



