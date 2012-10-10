#!/usr/bin/env python
# (c) 2012 g33k @ libxenon.org 

import argparse
import struct
import sys
import usb


ep_out = 0x03
ep_in  = 0x82

ISD2100 = [[0x11, "ISD2130", 0x20000], [0x10, "ISD2115A", 0x10000], [0x01, "ISD2110A", 0xB000]]

            
def cmd(cmd, argA=0, argB=0):
  # argA big-endian, argB little-endian 
  buffer = struct.pack(">L", argA) + struct.pack("<L", argB) 
  devhandle.controlMsg(requestType = usb.TYPE_VENDOR, request = cmd, value = 0, index = 0, buffer = buffer)

def getStatus():
  cmd(0x80)
  return devhandle.bulkRead(ep_in, 1, 1000)[0]
    
def getInt():
  cmd(0x81)
  return devhandle.bulkRead(ep_in, 2, 1000)[:2]
    
def powerUp():
  cmd(0x82)

def powerDown():
  cmd(0x83)

def reset():
  cmd(0x84)

def getId():
  cmd(0x85)
  return devhandle.bulkRead(ep_in, 4, 1000)[:4]
    
def readFlash(addr, len):
  cmd(0x86, addr, len)

def writeFlash(addr, len):
  cmd(0x87, addr, len)

def flush():
  cmd(0x88)

def playVP(index):
  cmd(0x89, index)

def playVPRN(reg):
  cmd(0x8A, reg)

def playVM(index):
  cmd(0x8B, index)

def playVMRN(reg):
  cmd(0x8C, reg)

def stop():
  cmd(0x8D)


def printStatus(status):
  print "   status: 0x%02x" % (status)
  print "       PD:   %d" % ((status & 0x80) > 0)
  print " DBUF_RDY:   %d" % ((status & 0x40) > 0)
  print "      INT:   %d" % ((status & 0x20) > 0)
  print "   VM_BSY:   %d" % ((status & 0x04) > 0)
  print " CBUF_FUL:   %d" % ((status & 0x02) > 0)
  print "  CMD_BSY:   %d" % ((status & 0x01) > 0)

def checkPU():
  if ((getStatus() & 0x80) > 0):
    print "Powerup device first!"
    exit()

def checkDevice():
  checkPU()
	
  devid = getId()[3]
  device = None
  for dev in ISD2100:
    if dev[0] == devid:
      device = dev

  if device == None:
    print "Unknown device"
    exit()
  
  return device

# main
parser = argparse.ArgumentParser(description='ISD2100 Flasher', add_help=False)
subparsers = parser.add_subparsers(title='Operations', dest='action')
subparsers.add_parser('status', help='Shows status')
subparsers.add_parser('int', help='Shows interrupt status')
subparsers.add_parser('pwrup', help='Powerup device')
subparsers.add_parser('pwrdwn', help='Powerdown device')
subparsers.add_parser('reset', help='Reset device')
subparsers.add_parser('id', help='Shows device id')
parser_read = subparsers.add_parser('read', help='Dumps device')
parser_read.add_argument('file', nargs=1, type=argparse.FileType('w'), help='The file to dump to')
parser_write = subparsers.add_parser('write', help='Writes device')
parser_write.add_argument('file', nargs=1, type=argparse.FileType('r'), help='The file to write to the device')
subparsers.add_parser('flush', help='Flushs device')
parser_voice = subparsers.add_parser('voice', help='Plays voice prompt')
parser_voice.add_argument('index', nargs=1, metavar='index', action='store', type=int, help='index')  
parser_voicerg = subparsers.add_parser('voicerg', help='Plays voice prompt in register')
parser_voicerg.add_argument('reg', nargs=1, metavar='reg', action='store', type=int, help='register')  
parser_macro = subparsers.add_parser('macro', help='Plays voice macro')
parser_macro.add_argument('index', nargs=1, metavar='index', action='store', type=int, help='index')  
parser_macrorg = subparsers.add_parser('macrorg', help='Plays voice macro in register')
parser_macrorg.add_argument('reg', nargs=1, metavar='reg', action='store', type=int, help='register')  
subparsers.add_parser('stop', help='Stops play')
subparsers.add_parser('help', help='Prints help')

arguments = parser.parse_args()

if arguments.action == 'help':
  parser.print_help()
  exit()

usbdev = None
devhandle = None
  
for bus in usb.busses():
  for dev in bus.devices:
    if dev.idVendor == 0xFFFF and dev.idProduct == 4:
      usbdev = dev
  
if not usbdev:
  print "USB hardware not found."
  sys.exit(1)
  
devhandle = usbdev.open()
devhandle.setConfiguration(1)
devhandle.claimInterface(0)

if arguments.action == 'status':
  printStatus(getStatus())
  
if arguments.action == 'int':
  data = getInt()
  printStatus(data[0])
  print "\n      INT: 0x%02x" % (data[1])
  print "  MPT_ERR:   %d" % ((data[1] & 0x40) > 0)
  print "   WR_FIN:   %d" % ((data[1] & 0x20) > 0)
  print "  CMD_ERR:   %d" % ((data[1] & 0x10) > 0)
  print "  OVF_ERR:   %d" % ((data[1] & 0x08) > 0)
  print "  CMD_FIN:   %d" % ((data[1] & 0x04) > 0)

if arguments.action == 'pwrup':
  powerUp()

if arguments.action == 'pwrdwn':
  powerDown()

if arguments.action == 'reset':
  reset()

if arguments.action == 'id':
  checkPU()

  data = getId()
  device = None
  for dev in ISD2100:
    if dev[0] == data[3]:
      device = dev

  if device == None:
    print "Unknown device"
    exit()
  print " PART_ID: 0x%02x" % (data[0])
  print "  MAN_ID: 0x%02x" % (data[1])
  print "MEM_TYPE: 0x%02x" % (data[2])
  print "  DEV_ID: 0x%02x (memsize: 0x%x => %s)" % (data[3], device[2], device[1])

if arguments.action == 'read':
  device = checkDevice()
  cnt = device[2]
  print "found %s  size: 0x%x" % (device[1], cnt)

  print "reading..."
  readFlash(0x00, cnt)
  while cnt > 0:
    dat = devhandle.bulkRead(ep_in, 64, 1000)
    arguments.file[0].write(''.join([chr(x) for x in dat]))
    cnt -= 64
  print "done"

if arguments.action == 'write':
  device = checkDevice()
  cnt = device[2]
  print "found %s  size: 0x%x" % (device[1], cnt)

  print "writing..."
  writeFlash(0x00, cnt)
  while cnt > 0:
    dat = arguments.file[0].read(64)
    if len(dat) < 64:
      dat += ('\xFF' * (64 - len(dat)))
    devhandle.bulkWrite(ep_out, dat, 1000)
    cnt -= 64
  print "done"

if arguments.action == 'flush':
  device = checkDevice()
  cnt = device[2]
  print "found %s  size: 0x%x" % (device[1], cnt)
  flush()

if arguments.action == 'voice':
  checkPU()
  print "found %s" % (checkDevice()[1])
  playVP(arguments.index[0])

if arguments.action == 'voicerg':
  checkPU()
  print "found %s" % (checkDevice()[1])
  playVPRN(arguments.reg[0])

if arguments.action == 'macro':
  checkPU()
  print "found %s" % (checkDevice()[1])
  playVM(arguments.index[0])

if arguments.action == 'macrorg':
  checkPU()
  print "found %s" % (checkDevice()[1])
  playVMRN(arguments.reg[0])

if arguments.action == 'stop':
  checkPU()
  print "found %s" % (checkDevice()[1])
  stop()
