import argparse
import os
import pprint
import sys

from XFlash import XFlash, DeviceNotFoundError
from XConfig import XConfig, XConfigParseError
from XStatus import *

pp = pprint.PrettyPrinter()

class ConsoleUI:
    def opStart(self, name, plural):
        self.name = name[0].upper() + name[1:]
        self.plural = plural
        sys.stdout.write(self.name + '\t')
    
    def opProgress(self,progress, total=-1):
        if (total >= 0): 
            prstr = '0x%04X / 0x%04X' % (progress, total)
        else:
            prstr = '0x%04X' % (progress)
        
        sys.stdout.write(prstr.ljust(20))
        sys.stdout.write('\b' * 20)
        sys.stdout.flush()
    
    def opProgressErr(self, block, error):
        sys.stdout.write('\b' * 20)
        msg = 'Error: %X %s block %X' % (error,
                                         self.plural,
                                         block)
        sys.stdout.write(msg.ljust(40) + '\n')
        self.opStart(self.name, self.plural)
    
    def opEnd(self, result):
        sys.stdout.write('\b' * 2)
        sys.stdout.write(result.ljust(20))
        sys.stdout.write("\n")

def hex2int(instring):
    try:
        return int(instring, 16)
    except ValueError:
        msg = '%s is not a valid hexadecimal number' % (instring)
        raise argparse.ArgumentTypeError(msg)

def main(argv=None):
    parser = argparse.ArgumentParser(description='XBox 360 NAND Flasher')
    actions = parser.add_subparsers(title='Operations', dest='action')
    
    read = actions.add_parser('read', help='Dumps an image from the NAND')
    read.add_argument('file', nargs=1, type=argparse.FileType('w'),
                      help='The file to dump the NAND to')
    read.add_argument('start', nargs='?', metavar='start',
                      action='store', type=hex2int, default=0,
                      help='The block to start the action from')
    read.add_argument('length', nargs='?', metavar='length',
                      action='store', type=hex2int, default=None,
                      help='The count of blocks to perform the action to')
    
    write = actions.add_parser('write', help='Writes an image into the NAND')
    write.add_argument('file', nargs=1, type=argparse.FileType('r'),
                       help='The image file to write to the NAND')
    write.add_argument('start', nargs='?', metavar='start',
                       action='store', type=hex2int, default=0,
                       help='The block to start the action from')
    write.add_argument('length', nargs='?', metavar='length',
                       action='store', type=hex2int, default=None,
                       help='The count of blocks to perform the action to')
    
    erase = actions.add_parser('erase', help='Erases blocks in the NAND')
    erase.add_argument('start', nargs='?', metavar='start',
                       action='store', type=hex2int, default=0,
                       help='The block to start the action from')
    erase.add_argument('length', nargs='?', metavar='length',
                       action='store', type=hex2int, default=None,
                       help='The count of blocks to perform the action to')
    
    xsvf = actions.add_parser('xsvf', help='Flash a CPLD with an xsvf file')
    xsvf.add_argument('file', nargs=1, type=argparse.FileType('r'), help='The xsvf file to read from')
    
    update = actions.add_parser('update',
                                help='Jumps into the bootloader of the NAND Flashing device for updating the firmware')
    shutdown = actions.add_parser('poweroff',
                                  help='Shuts down the attached XBox 360')
    poweron = actions.add_parser('poweron',
                                 help='Powers up the attached XBox 360')
    
    arguments = parser.parse_args(argv)
    
    ui = ConsoleUI()
    xf = XFlash()
    xc = None
    doOp = None
    
    try:
        xf.deviceFind()
    except DeviceNotFoundError:
        print 'XFlash USB device not found'
        sys.exit(1)
    
    xf.deviceReset()
    
    start = length = end = 0
    
    print "Using XFlash @ [%s:%s]" % (xf.dev.bus, xf.dev.address)
    if arguments.action in ('erase', 'write', 'read'):
        try:
            vers = xf.deviceVersion()
            print 'ARM Version %s' % (vers)
            flashconfig = xf.flashInit()
            print 'FlashConfig: 0x%08X' % (flashconfig)
            xc = XConfig(flashconfig)
        except XConfigParseError as e:
            print 'Flash Config is invalid!'
            print e
            sys.exit(1)
        except:
            xf.flashDeInit()
        start = arguments.start
        length = arguments.length or (xc.sizesmallblocks - start)
        end = start + length
        
        if end > xc.sizesmallblocks:
            print 'Error: tried to read past the nand length'
            print 'NandSize: %X\tOperationEnd: %X' % (xc.sizeblocks, end)
            sys.exit(1)
    
    if arguments.action == 'erase':
        doOp = xf.flashErase
    
    if arguments.action == 'read':
        def doOp(b):
            (status, buf) = xf.flashRead(b)
            arguments.file[0].write(buf)
            return status
    
    if arguments.action == 'write':
        def doOp(b):
            blocksize = 528 * 32
            buf = arguments.file[0].read(blocksize)
            if len(buf) < blocksize:
                buf += ('\xFF' * (blocksize-len(buf)))
            return xf.flashWrite(b, buf)
    
    if arguments.action == 'xsvf':
        vers = xf.xsvfInit()
        if vers < 3:
            print 'ARM Version %s does not support xsvf flashing!' % vers
            sys.exit(1)
        print 'ARM Version %s' % (vers)
        fbuf = arguments.file[0].read()
        print 'Read 0x%x bytes OK' % (len(fbuf))
        buf = XFlash.compress(fbuf)
        print 'Compressed to 0x%x bytes OK' % (len(buf))
        xf.xsvfWrite(buf)
        print '0x%x bytes sent OK' % (len(buf))
        print 'Executing File...',
        sys.stdout.flush()
        status = xf.xsvfExecute()
        print 'OK!' if status == 0 else 'FAIL!'
        xf.deviceReset()
    
    if arguments.action == 'update':
        xf.deviceUpdate()
    
    if arguments.action == 'poweron':
        xf.flashPowerOn()
    
    if arguments.action == 'poweroff':
        xf.flashPowerOff()
    
    if arguments.action in ('read', 'write', 'erase'):
        plural = arguments.action[:-1] + 'ing'
        if arguments.action == 'read':
            plural = 'reading'
        ui.opStart(arguments.action, plural)
        try:
            for b in xrange(start, end):
                status = doOp(b)
                if statusIsError(status):
                    ui.opProgressErr(b, status)
                ui.opProgress(b, end-1)
        except KeyboardInterrupt:
            ui.opEnd('INTERRUPTED!')
            path = os.path.abspath(arguments.file[0].name)
            if os.path.isfile(path):
                os.remove(path)
        except:
            ui.opEnd('FAIL!')
            raise
        else:
            ui.opEnd('%X OK!' % (length))

if __name__ == '__main__':
    main()