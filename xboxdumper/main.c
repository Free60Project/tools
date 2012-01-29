// Main entry point for xbox dumper
// (c) 2001 Andrew de Quincey

#include "mytypes.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>

#include "file64.h"

#include "util.h"
#include "fatx.h"

#if	XBOX360

#define	MAX_PARTITIONS	32
u_int64_t partitionAddresses[MAX_PARTITIONS];
u_int64_t partitionSizes[MAX_PARTITIONS];
int	partitionCount = 0;


// FIXME!
u_int64_t partitionAddresses__HardDrive[] = {0x120eb0000ULL, 0x130eb0000ULL };
u_int64_t partitionSizes__HardDrive[] = {     0x12000000ULL, 0x377680000ULL };
int	partitionCount__HardDrive = (sizeof(partitionAddresses__HardDrive) / sizeof(partitionAddresses__HardDrive[0]));


u_int64_t partitionAddresses__MemCard[] = { 0x0UL,
                                   0x7ff000UL };
u_int64_t partitionSizes__MemCard[] = { 0x7ff000UL,
                               (0x3E20000UL-0x7ff000UL) };
int	partitionCount__MemCard = (sizeof(partitionAddresses__MemCard) / sizeof(partitionAddresses__MemCard[0]));



#else	// !XBOX360

// The partition details
u_int64_t partitionAddresses[] = { 0x80000UL,
                                   0x2ee80000UL,
                                   0x5dc80000UL,
                                   0x8ca80000UL,
                                   0xabe80000UL };
u_int64_t partitionSizes[] = { 0x2ee80000UL,
                               0x2ee80000UL,
                               0x2ee80000UL,
                               0x1f400000UL,
                               0x131F00000UL };
int	partitionCount = (sizeof(partitionAddresses) / sizeof(partitionAddresses[0]));

#endif // XBOX360

/**
 * Output syntax
 */
void syntax();

char *Pname;

/**
 * Main entry point
 */
int main(int argc, char* argv[]) {
  fd64_t sourceFd;
  FATXPartition* partition;
  char* sourceFilename;
  char* extractFilename;
  char* outputFilename;
  int partitionId;
  fd64_t outputFd;
  int listFiles = 0;
  int extractFile = 0;
#if	XBOX360
  int is_MemCard = 0;
#endif // XBOX360


  Pname = argv[0];

#if	XBOX360
  is_MemCard = 0;

  while(argc > 1) {
    if (argv[1][0] != '-') {
        break;
    }
    if (!strcmp(argv[1], "-memcard")) {
        is_MemCard = 1;
    }
    ++argv;
    --argc;
  }
  if (is_MemCard) {
    memcpy(partitionAddresses, partitionAddresses__MemCard, sizeof(partitionAddresses__MemCard));
    memcpy(partitionSizes, partitionSizes__MemCard, sizeof(partitionSizes__MemCard));
    partitionCount = partitionCount__MemCard;
  } else {
    memcpy(partitionAddresses, partitionAddresses__HardDrive, sizeof(partitionAddresses__HardDrive));
    memcpy(partitionSizes, partitionSizes__HardDrive, sizeof(partitionSizes__HardDrive));
    partitionCount = partitionCount__HardDrive;
  }
#endif // XBOX360

  // parse the arguments
  if (argc < 4) {
    syntax();
  }
  if (!strcmp(argv[1], "list")) {
    // extract details
    listFiles = 1;
    if ((!isdigit(argv[2][0])) || (strlen(argv[2]) != 1)) {
      syntax();
    }
    partitionId = atoi(argv[2]);
    sourceFilename = argv[3];
  } else if (!strcmp(argv[1], "dump")) {
    // ensure we still have enough args
    if (argc < 6) {
      syntax();
    }

    // extract config
    extractFile = 1;
    extractFilename = argv[2];
    outputFilename = argv[3];
    if ((!isdigit(argv[4][0])) || (strlen(argv[4]) != 1)) {
      syntax();
    }
    partitionId = atoi(argv[4]);
    sourceFilename = argv[5];
  } else {
    syntax();
  }

  // check partition id
  if ((partitionId < 0) || (partitionId >= partitionCount)) {
    syntax();
  }

  // open output file
  if (extractFile) {
    outputFd = open64(outputFilename,
		      O_RDWR | O_TRUNC | O_CREAT);
    if (IS_INVALID_FD64(outputFd)) {
      error("Unable to open output file %s", outputFilename);
    }
  }

  // open the file
  if (IS_INVALID_FD64((sourceFd = open64(sourceFilename, O_RDWR)))) {
    error("Unable to open source file %s", sourceFilename);
  }

  // open the partition
  partition = openPartition(sourceFd,
                            partitionAddresses[partitionId],
                            partitionSizes[partitionId]);

  // dump the directory tree
  if (listFiles) {
    dumpTree(partition, STDOUT_FD64);
  }
  if (extractFile) {
    dumpFile(partition, extractFilename, outputFd);
  }

  // close output file
  if (extractFile) {
    close64(outputFd);
  }

  // close the partition
  closePartition(partition);

  // close the file
  close64(sourceFd);
}

/**
 * Output syntax
 */
void syntax() {
  error("Syntax: xboxdumper <list|dump <FATX filename> <output filename>> <partition ID (0|1|2|3|4)> <XBOX image file>");
}
