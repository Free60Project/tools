xboxdumper v0.13
(c) 2001 Andrew de Quincey

**UPDATE**: oz_paulb modified code to deal with Xbox360 images.
            See "XBOX360" define in the Makefile.  Still a 
            work in progress.


This program allows files on an xbox hard disk to be dumped out.


Two basic commands are supported:


xboxdumper list <partition number> <xbox image filename>

This will dump the directory tree of the specified partition.

(e.g. "./xboxdumper.sh list 1 xboximage.bin" )




xboxdumper dump <xbox filename> <output filename> <partition number> <image filename>

This will dump the file <xbox filename> into the file <output filename> 
from the specified partition number. The full directory path to the file 
using / or \ should be supplied.

(e.g. "./xboxdumper.sh dump /voice.afs voice.afs 1 xboximage.bin" )


<partition number> may be between 0 and 4 inclusively. Partition 0 is not 
confirmed yet.

I will not supply images of the xbox hard disk, so please don't bother 
asking.
