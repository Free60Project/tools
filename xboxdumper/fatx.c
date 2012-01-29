// Functions for processing FATX partitions
// (c) 2001 Andrew de Quincey

#include "mytypes.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "file64.h"

#include "util.h"
#include "fatx.h"
#include "utility.h"

/**
 * Checks if the current entry is the last entry in a directory
 *
 * @param entry Raw entry data
 * @return 1 If it is the last entry, 0 if not
 */
int checkForLastDirectoryEntry(FATX_DIR_ENT *pde);

/**
 * Gets the next cluster in the cluster chain
 *
 * @param partition FATX partition
 * @param clusterId Cluster to find the netx cluster for
 * @return ID of the next cluster in the chain, or -1 if there is no next cluster
 */
u_int32_t getNextClusterInChain(FATXPartition* partition, int clusterId);

/**
 * Load data for a cluster
 *
 * @param partition FATX partition
 * @param clusterId ID of the cluster to load
 * @param clusterData Where to store the data (must be at least the cluster size)
 */
void loadCluster(FATXPartition* partition, int clusterId, unsigned char* clusterData);


/**
 * Recursively dump the directory tree
 *
 * @param partition FATX Partition
 * @param outputStream Stream to output to
 * @param clusterId ID of cluster to start at
 * @param nesting Nesting level of directory tree
 */
void _dumpTree(FATXPartition* partition,
               fd64_t outputStream,
               int clusterId, int nesting);



/**
 * Recursively down a directory tree to find and extract a specific file
 *
 * @param partition FATX Partition
 * @param filename Filename to extract
 * @param outputStream Stream to output to
 * @param clusterId ID of cluster to start at
 */
void _recurseToFile(FATXPartition* partition,
                    char* filename,
                    fd64_t outputStream,
                    int clusterId);



/**
 * Dump a file to supplied outputStream
 *
 * @param partition FATX partition
 * @param outputStream Stream to output to
 * @param clusterId Starting cluster ID of file
 * @param fileSize Size of file
 */
void _dumpFile(FATXPartition* partition, fd64_t outputStream,
               int clusterId, u_int32_t fileSize);

/**
 * Open a FATX partition
 *
 * @param sourceFd Source file
 * @param partitionOffset Offset into above file that partition starts at
 * @param partitionSize Size of partition in bytes
 */
FATXPartition* openPartition(fd64_t sourceFd,
                             u_int64_t partitionOffset,
                             u_int64_t partitionSize) {
  FATX_PARTITION_HDR	phdr;
  size_t readSize;
  FATXPartition* partition;
  u_int32_t chainTableSize;

  int i;

  // load the partition header
  lseek64(sourceFd, partitionOffset, SEEK_SET);
  readSize = read64(sourceFd,
                  &phdr, sizeof(phdr));
  if (readSize != sizeof(phdr)) {
    error("Out of data while reading partition header");
  }

  phdr.fph_magic = bswap32(phdr.fph_magic);
  phdr.fph_volume_id = bswap32(phdr.fph_volume_id);
  phdr.fph_cluster_nsects = bswap32(phdr.fph_cluster_nsects);
  phdr.fph_num_fat_tables = bswap32(phdr.fph_num_fat_tables);

  // check the magic
  if (phdr.fph_magic != FATX_PARTITION_MAGIC) {
    error("No FATX partition found at requested offset");
  }

  // make up new structure
  partition = (FATXPartition*) malloc(sizeof(FATXPartition));
  if (partition == NULL) {
    error("Out of memory");
  }

  // setup the easy bits
  partition->sourceFd = sourceFd;
  partition->partitionStart = partitionOffset;
  partition->partitionSize = partitionSize;
  partition->clusterSize = (phdr.fph_cluster_nsects * 512);
  partition->clusterCount = partition->partitionSize / partition->clusterSize;
  partition->chainMapEntrySize = (partition->clusterCount >= 0xfff4) ? 4 : 2;

  // Now, work out the size of the cluster chain map table
  chainTableSize = partition->clusterCount * partition->chainMapEntrySize;
  if (chainTableSize % FATX_CHAINTABLE_BLOCKSIZE) {
    // round up to nearest FATX_CHAINTABLE_BLOCKSIZE bytes
    chainTableSize =
      ((chainTableSize / FATX_CHAINTABLE_BLOCKSIZE) + 1) * FATX_CHAINTABLE_BLOCKSIZE;
  }

  // Load the cluster chain map table
  partition->clusterChainMap.words = (u_int16_t*) malloc(chainTableSize);
  if (partition->clusterChainMap.words == NULL) {
    error("Out of memory");
  }

  lseek64(sourceFd, partitionOffset + FATX_PARTITION_HEADERSIZE, SEEK_SET);
  readSize = read64(sourceFd,
                  partition->clusterChainMap.words,
                  chainTableSize);
  if (readSize != chainTableSize) {
    error("Out of data while reading cluster chain map table");
  }
  {
	int idx;

	if (partition->chainMapEntrySize == 4) {
		for (idx = 0; idx < partition->clusterCount; idx++) {
			partition->clusterChainMap.dwords[idx] = bswap32(partition->clusterChainMap.dwords[idx]);
		}
	} else {
		for (idx = 0; idx < partition->clusterCount; idx++) {
			partition->clusterChainMap.words[idx] = bswap16(partition->clusterChainMap.words[idx]);
		}
	}
  }
  // Work out the address of cluster 1
  partition->cluster1Address =
    partitionOffset + FATX_PARTITION_HEADERSIZE + chainTableSize;

  // All done
  return partition;
}


/**
 * Close a FATX partition
 */
void closePartition(FATXPartition* partition) {
  free(partition->clusterChainMap.words);
  free(partition);
}


/**
 * Dump a file to the supplied stream. If file is a directory,
 * the directory listing will be dumped
 *
 * @param partition The FATX partition
 * @param outputStream Stream to output data to
 * @param filename Filename of file to dump
 */
void dumpFile(FATXPartition* partition, char* filename, fd64_t outputStream) {
  int i = 0;

  // convert any '\' to '/' characters
  while(filename[i] != 0) {
    if (filename[i] == '\\') {
      filename[i] = '/';
    }
    i++;
  }

  // skip over any leading / characters
  i=0;
  while((filename[i] != 0) && (filename[i] == '/')) {
    i++;
  }

  // OK, start off the recursion at the root FAT
  _recurseToFile(partition, filename + i, outputStream, FATX_ROOT_FAT_CLUSTER);
}

/**
 * Dump entire directory tree to supplied stream
 *
 * @param partition The FATX partition
 * @param outputStream Stream to output data to
 */
void dumpTree(FATXPartition* partition, fd64_t outputStream) {
  // OK, start off the recursion at the root FAT
  _dumpTree(partition, outputStream, FATX_ROOT_FAT_CLUSTER, 0);
}


/**
 * Recursively dump the directory tree
 *
 * @param partition FATX Partition
 * @param outputStream Stream to output to
 * @param clusterId ID of cluster to start at
 * @param nesting Nesting level of directory tree
 */
void _dumpTree(FATXPartition* partition,
              fd64_t outputStream,
              int clusterId, int nesting) {
  unsigned char* curEntry;
  unsigned char clusterData[partition->clusterSize];
  int readSize;
  int i;
  int j;
  int endOfDirectory;
  u_int32_t filenameSize;
  u_int32_t flags;
  u_int32_t entryClusterId;
  u_int32_t fileSize;
  DosDateTime date1;
  DosDateTime date2;
  DosDateTime date3;
  char filename[50];
  char writeBuf[512];
  char flagsStr[5];
  FATX_DIR_ENT	dirent;

  // OK, output all the directory entries
  endOfDirectory = 0;
  while(clusterId != -1) {
    // load cluster data
    loadCluster(partition, clusterId, clusterData);

    // loop through it, outputing entries
    for(i=0; i< partition->clusterSize / FATX_DIRECTORYENTRY_SIZE; i++) {
      // work out the currentEntry
      curEntry = clusterData + (i * FATX_DIRECTORYENTRY_SIZE);

      memcpy(&dirent, curEntry, sizeof(dirent));
	dirent.fde_first_cluster = bswap32(dirent.fde_first_cluster);
	dirent.fde_file_size_bytes = bswap32(dirent.fde_file_size_bytes);
	dirent.fde_file_mod_datetime = bswap32(dirent.fde_file_mod_datetime);
	dirent.fde_file_create_datetime = bswap32(dirent.fde_file_create_datetime);
	dirent.fde_file_access_datetime = bswap32(dirent.fde_file_access_datetime);

      // first of all, check that it isn't an end of directory marker
      if (checkForLastDirectoryEntry(&dirent)) {
        endOfDirectory = 1;
        break;
      }

      // get the filename size
      filenameSize = dirent.fde_name_len;

      // check if file is deleted
      if (filenameSize == 0xE5) {
        continue;
      }

      // check size is OK
      if ((filenameSize < 1) || (filenameSize > FATX_FILENAME_MAX)) {
        printf("Invalid filename size: %i in cluster %d, loop #%d\noz   :", filenameSize, clusterId, i);
        hexdump(clusterData, partition->clusterSize);
        error("Invalid filename size: %i in cluster %d, loop #%d", filenameSize, clusterId, i);
      }

      // extract the filename
      memset(filename, 0, 50);
      memcpy(filename, dirent.fde_name, filenameSize);
      filename[filenameSize] = 0;

      // get rest of data
      flags = dirent.fde_attributes;
      entryClusterId = dirent.fde_first_cluster;
      fileSize = dirent.fde_file_size_bytes;
      loadDosDateTime(&date1, ((dirent.fde_file_mod_datetime >> 16) & 0xffff), (dirent.fde_file_mod_datetime & 0xffff));
      loadDosDateTime(&date2, ((dirent.fde_file_create_datetime >> 16) & 0xffff), (dirent.fde_file_create_datetime & 0xffff));
      loadDosDateTime(&date3, ((dirent.fde_file_access_datetime >> 16) & 0xffff), (dirent.fde_file_access_datetime & 0xffff));

      // wipe fileSize
      if (flags & FATX_FILEATTR_DIRECTORY) {
        fileSize = 0;
      }

      // zap flagsStr
      strcpy(flagsStr, "    ");

      // work out other flags
      if (flags & FATX_FILEATTR_READONLY) {
        flagsStr[0] = 'R';
      }
      if (flags & FATX_FILEATTR_HIDDEN) {
        flagsStr[1] = 'H';
      }
      if (flags & FATX_FILEATTR_SYSTEM) {
        flagsStr[2] = 'S';
      }
      if (flags & FATX_FILEATTR_ARCHIVE) {
        flagsStr[3] = 'A';
      }

      // check we don't have any unknown flags
      if (flags & 0xc8) {
        printf("WARNING: file %s has unknown flags %x\n", filename, flags);
      }

      // Output it
      for(j=0; j< nesting; j++) {
        writeBuf[j] = ' ';
      }
      sprintf(writeBuf+nesting, "/%s  [%s] (SZ:%i CL:%x D1:%s D2:%s D3:%s)\n",
              filename, flagsStr, fileSize, entryClusterId,
              formatDosDate(&date1), formatDosDate(&date2), formatDosDate(&date3));
      write64(outputStream, writeBuf, strlen(writeBuf));

      // If it is a sub-directory, recurse
      if (flags & FATX_FILEATTR_DIRECTORY) {
        _dumpTree(partition, outputStream, entryClusterId, nesting+1);
      }
    }

    // have we hit the end of the directory yet?
    if (endOfDirectory) {
      break;
    }

    // Find next cluster
    clusterId = getNextClusterInChain(partition, clusterId);
  }
}


/**
 * Recursively down a directory tree to find and extract a specific file
 *
 * @param partition FATX Partition
 * @param filename Filename to extract
 * @param outputStream Stream to output to
 * @param clusterId ID of cluster to start at
 */
void _recurseToFile(FATXPartition* partition,
                    char* filename,
                    fd64_t outputStream,
                    int clusterId) {
  unsigned char* curEntry;
  unsigned char clusterData[partition->clusterSize];
  int readSize;
  int i;
  int j;
  int endOfDirectory;
  u_int32_t filenameSize;
  u_int32_t flags;
  u_int32_t entryClusterId;
  u_int32_t fileSize;
  char seekFilename[50];
  char foundFilename[50];
  char* slashPos;
  int lookForDirectory = 0;
  int lookForFile = 0;
  FATX_DIR_ENT	dirent;


  // work out the filename we're looking for
  slashPos = strchr(filename, '/');
  if (slashPos == NULL) {
    // looking for file
    lookForFile = 1;

    // check seek filename size
    if (strlen(filename) > FATX_FILENAME_MAX) {
      error("Bad filename supplied (one leafname is too big)");
    }

    // copy the filename to look for
    strcpy(seekFilename, filename);
  } else {
    // looking for directory
    lookForDirectory = 1;

    // check seek filename size
    if ((slashPos - filename) > FATX_FILENAME_MAX) {
      error("Bad filename supplied (one leafname is too big)");
    }

    // copy the filename to look for
    strncpy(seekFilename, filename, slashPos - filename);
    seekFilename[slashPos - filename] = 0;
  }

  // lowercase it
  for(i=0; i< strlen(seekFilename); i++) {
    seekFilename[i] = tolower(seekFilename[i]);
  }

  // OK, search through directory entries
  endOfDirectory = 0;
  while(clusterId != -1) {
    // load cluster data
    loadCluster(partition, clusterId, clusterData);

    // loop through it, outputing entries
    for(i=0; i< partition->clusterSize / FATX_DIRECTORYENTRY_SIZE; i++) {
      // work out the currentEntry
      curEntry = clusterData + (i * FATX_DIRECTORYENTRY_SIZE);

      memcpy(&dirent, curEntry, sizeof(dirent));
	dirent.fde_first_cluster = bswap32(dirent.fde_first_cluster);
	dirent.fde_file_size_bytes = bswap32(dirent.fde_file_size_bytes);
	dirent.fde_file_mod_datetime = bswap32(dirent.fde_file_mod_datetime);
	dirent.fde_file_create_datetime = bswap32(dirent.fde_file_create_datetime);
	dirent.fde_file_access_datetime = bswap16(dirent.fde_file_access_datetime);

      // first of all, check that it isn't an end of directory marker
      if (checkForLastDirectoryEntry(&dirent)) {
        endOfDirectory = 1;
        break;
      }

      // get the filename size
      filenameSize = dirent.fde_name_len;

      // check if file is deleted
      if (filenameSize == 0xE5) {
        continue;
      }

      // check size is OK
      if ((filenameSize < 1) || (filenameSize > FATX_FILENAME_MAX)) {
        error("Invalid filename size: %i", filenameSize);
      }

      // extract the filename
      memset(foundFilename, 0, 50);
      memcpy(foundFilename, dirent.fde_name, filenameSize);
      foundFilename[filenameSize] = 0;

      // get rest of data
      flags = dirent.fde_attributes;
      entryClusterId = dirent.fde_first_cluster;
      fileSize = dirent.fde_file_size_bytes;

      // lowercase foundfilename
      for(j=0; j< strlen(foundFilename); j++) {
        foundFilename[j] = tolower(foundFilename[j]);
      }

      // is it what we're looking for...
      if (!strcmp(foundFilename, seekFilename)) {
        // if we're looking for a directory and found a directory
        if (lookForDirectory) {
          if (flags & FATX_FILEATTR_DIRECTORY) {
            _recurseToFile(partition, slashPos+1, outputStream, entryClusterId);
            return;
          } else {
            error("File not found");
          }
        }

        // if we're looking for a file and found a file
        if (lookForFile) {
          if (!(flags & FATX_FILEATTR_DIRECTORY)) {
            _dumpFile(partition, outputStream, entryClusterId, fileSize);
            return;
          } else {
            error("File not found");
          }
        }
      }
    }

    // have we hit the end of the directory yet?
    if (endOfDirectory) {
      break;
    }

    // Find next cluster
    clusterId = getNextClusterInChain(partition, clusterId);
  }

  // not found it!
  error("File not found");
}



/**
 * Dump a file to supplied outputStream
 *
 * @param partition FATX partition
 * @param outputStream Stream to output to
 * @param clusterId Starting cluster ID of file
 * @param fileSize Size of file
 */
void _dumpFile(FATXPartition* partition, fd64_t outputStream,
               int clusterId, u_int32_t fileSize) {
  unsigned char clusterData[partition->clusterSize];
  int writtenSize;

  // loop, outputting clusters
  while(clusterId != -1) {
    // Load the cluster data
    loadCluster(partition, clusterId, clusterData);

    // Now, output it
    writtenSize =
      write64(outputStream, clusterData,
            (fileSize <= partition->clusterSize) ? fileSize : partition->clusterSize);
    fileSize -=writtenSize;

    // Find next cluster
    clusterId = getNextClusterInChain(partition, clusterId);
  }

  // check we actually found enough data
  if (fileSize != 0) {
    error("Hit end of cluster chain before file size was zero");
  }
}



/**
 * Checks if the current entry is the last entry in a directory
 *
 * @param entry Raw entry data
 * @return 1 If it is the last entry, 0 if not
 */
int checkForLastDirectoryEntry(FATX_DIR_ENT *pde) {
  // if the filename length byte is 0 or 0xff,
  // this is the last entry
  if ((pde->fde_name_len == 0xff) || (pde->fde_name_len == 0)) {
    return 1;
  }

  // wasn't last entry
  return 0;
}


/**
 * Gets the next cluster in the cluster chain
 *
 * @param partition FATX partition
 * @param clusterId Cluster to find the netx cluster for
 * @return ID of the next cluster in the chain, or -1 if there is no next cluster
 */
u_int32_t getNextClusterInChain(FATXPartition* partition, int clusterId) {
  int nextClusterId;
  u_int32_t eocMarker;
  u_int32_t rootFatMarker;
  u_int32_t maxCluster;

  // check
  if (clusterId < 1) {
    error("Attempt to access invalid cluster: %i", clusterId);
  }

  // get the next ID
  if (partition->chainMapEntrySize == 2) {
    nextClusterId = partition->clusterChainMap.words[clusterId];
    eocMarker = 0xffff;
    rootFatMarker = 0xfff8;
    maxCluster = 0xfff4;
  } else if (partition->chainMapEntrySize == 4) {
    nextClusterId = partition->clusterChainMap.dwords[clusterId];
    eocMarker = 0xffffffff;
    rootFatMarker = 0xfffffff8;
    maxCluster = 0xfffffff4;
  } else {
    error("Unknown cluster chain map entry size: %i", partition->chainMapEntrySize);
  }

  // is it the end of chain?
  if ((nextClusterId == eocMarker) || (nextClusterId == rootFatMarker)) {
    return -1;
  }

  // is it something else unknown?
  if (nextClusterId == 0) {
    error("Cluster chain problem: Next cluster after %i is unallocated!", clusterId);
  }
  if (nextClusterId > maxCluster) {
    error("Cluster chain problem: Next cluster after %i has invalid value: %i", clusterId, nextClusterId);
  }

  // OK!
  return nextClusterId;
}


/**
 * Load data for a cluster
 *
 * @param partition FATX partition
 * @param clusterId ID of the cluster to load
 * @param clusterData Where to store the data (must be at least the cluster size)
 */
void loadCluster(FATXPartition* partition, int clusterId, unsigned char* clusterData) {
  u_int64_t clusterAddress;
  u_int64_t readSize;

  // work out the address of the cluster
  clusterAddress = (u_int64_t)partition->cluster1Address + ((clusterId - 1) * (u_int64_t)partition->clusterSize);

  // Now, load it
  lseek64(partition->sourceFd, clusterAddress, SEEK_SET);
  readSize = read64(partition->sourceFd,
                  clusterData,
                  partition->clusterSize);
  if (readSize != partition->clusterSize) {
    error("Out of data while reading cluster %i", clusterId);
  }
}
