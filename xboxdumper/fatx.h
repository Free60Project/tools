// Definitions for FATX on-disk structures
// (c) 2001 Andrew de Quincey

#ifndef FATX_H
#define FATX_H

// FATX partition magic
#define FATX_PARTITION_MAGIC 0x58544146	//	"FATX"

typedef struct tagFATX_PARTITION_HDR {
	u_int32_t	fph_magic;
	u_int32_t	fph_volume_id;
	u_int32_t	fph_cluster_nsects;
	u_int32_t	fph_num_fat_tables;
} FATX_PARTITION_HDR;

// Size of FATX partition header
#define FATX_PARTITION_HEADERSIZE	0x1000


// FATX chain table block size
#define FATX_CHAINTABLE_BLOCKSIZE 4096

// ID of the root FAT cluster
#define FATX_ROOT_FAT_CLUSTER 1



typedef struct tagFATX_DIR_ENT {
	unsigned char	fde_name_len;
	unsigned char	fde_attributes;
	unsigned char	fde_name[42];
	u_int32_t	fde_first_cluster;
	u_int32_t	fde_file_size_bytes;
	u_int32_t	fde_file_mod_datetime;
	u_int32_t	fde_file_create_datetime;
	u_int32_t	fde_file_access_datetime;
} FATX_DIR_ENT;

// Size of FATX directory entries
#define FATX_DIRECTORYENTRY_SIZE 0x40

// File attribute: read only
#define FATX_FILEATTR_READONLY 0x01

// File attribute: hidden
#define FATX_FILEATTR_HIDDEN 0x02

// File attribute: system
#define FATX_FILEATTR_SYSTEM 0x04

// File attribute: archive
#define FATX_FILEATTR_ARCHIVE 0x20

// Directory entry flag indicating entry is a sub-directory
#define FATX_FILEATTR_DIRECTORY 0x10

// max filename size
#define FATX_FILENAME_MAX 42

// This structure describes a FATX partition
typedef struct {
  // The source file
  fd64_t sourceFd;

  // The starting byte of the partition
  u_int64_t partitionStart;

  // The size of the partition in bytes
  u_int64_t partitionSize;

  // The cluster size of the partition
  u_int32_t clusterSize;

  // Number of clusters in the partition
  u_int32_t clusterCount;

  // Size of entries in the cluster chain map
  u_int32_t chainMapEntrySize;

  // The cluster chain map table (which may be in words OR dwords)
  union {
    u_int16_t* words;
    u_int32_t* dwords;
  } clusterChainMap;

  // Address of cluster 1
  u_int64_t cluster1Address;

} FATXPartition;




/**
 * Open a FATX partition
 *
 * @param sourceFd File to read from
 * @param partitionOffset Offset into above file that partition starts at
 * @param partitionSize Size of partition in bytes
 */
FATXPartition* openPartition(fd64_t sourceFd,
                             u_int64_t partitionOffset,
                             u_int64_t partitionSize);


/**
 * Close a FATX partition
 */
void closePartition(FATXPartition* partition);

/**
 * Dump entire directory tree to supplied stream
 *
 * @param partition The FATX partition
 * @param outputStream Stream to output data to
 */
void dumpTree(FATXPartition* partition, fd64_t outputStream);



#endif
