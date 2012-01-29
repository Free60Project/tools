/*
    shahunt.c - find SHA-1 blocks in files.
    Copyright (C) 2005 th0mas

    This program is free software; you can redistribute it and/or modify it under the 
    terms of the GNU General Public License as published by the Free Software Foundation; 
    either version 2 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
    PARTICULAR PURPOSE. See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along with this 
    program; if not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
    
    SHAHunt loads a set of (presumed) SHA-1 hashes from a file, and then iterates over the file trying to match
    plaintext chunks to one of the loaded hashes.  You can specify a lot of configuration in order to increase
    compatability with a variety of file types, such as file offsets to load the hashes, and offsets to start the
    iteration.  Run the program without any arguments to see some usage information.
     
    Requires libxbox from xbox-linux CVS.
    
    All settings are stored in a settings_t struct, so it would be easy to add support to save state on a break signal
    and the be able to load the settings back from a file.  If a serious need arised the program could also be converted to
    a distributed client/server model to distribute the load.  Email me if you have a need for this and we can work something
    out.
*/


#include <stdio.h>
#include <string.h>
#include "sha1.h"

//typedef unsigned char uint8_t;
//typedef unsigned int uint32_t;

typedef struct {
   uint8_t hash[20]; // the SHA-1 hash
   uint32_t hashOffset; // where did the hash come from
   uint32_t fileOffset; // what do we hash in order to get this SHA-1 output
   uint32_t hashSize;  // how much data to hash
} hashEntry;

typedef struct {
   unsigned int curFileOffset;
   unsigned int maxFileOffset;
   unsigned int curHashLength;
   unsigned int minHashLength;
   unsigned int maxHashLength;
   unsigned int numHashes;
   hashEntry *hashEntries;
   unsigned int hashOffset;
   int hashRelativeOffset;  /* can be negative, to have overlapping SHA-1 hashes (used when unsure when hash entries start) */
   char *fileName;
} settings_t;


void usage(char *progname)
{
   printf("Usage: %s <args> <filename>\n"
          "<args> consist of one or more of the following:\n"
	  "-so,  --start-offset <offset>          Start our hashing iteration at\n"
	  "                                       provided offset.\n"
	  "-eo,  --end-offset <offset>            End our offset at provided offset.\n"
	  "-sl,  --start-hashlength <size>        Initial length of data to hash.\n"
	  "-el,  --end-hashlength <size>          Final length of data to hash.\n"
	  "-nh,  --num-hashes <num>               Number of hashes to load from file.\n"
	  "-ho,  --hash-offset <offset>           Location of first stored SHA-1 hash\n"
	  "                                       in the file.\n"
	  "-hro, --hash-relative-offset <offset>  Number of bytes to seek (forward or\n"
	  "                                       backward) from end of one stored hash\n"
	  "					  to start of next.  See example.\n\n\n"
	  "Example: To check the first 16 hash entries from a PIRS file, use:\n\n"
	  "  %s --start-offset 0xc000 --end-offset 0x1c000 --start-hashlength 4096\n"
	  "       --end-hashlength 4096 --hash-offset 0xb000 --hash-relative-offset 4\n\n\n", progname, progname);
}


void defaultSettings(settings_t *theSettings)
{
   theSettings->curFileOffset = 0;
   theSettings->maxFileOffset = 0x1000;
   theSettings->curHashLength = theSettings->minHashLength = theSettings->maxHashLength = 0x1000;
   theSettings->fileName = 0;
   theSettings->numHashes = 1;
   theSettings->hashOffset = 0;
   theSettings->hashRelativeOffset = 0;
   theSettings->hashEntries = 0;
}

void readSettings(settings_t *theSettings, int argc, char **argv)
{
   int i;
   for (i = 1; i < argc; i++) {
      if (!strcmp(argv[i], "--start-offset") || !strcmp(argv[i], "-so")) {
         i++;
         theSettings->curFileOffset = strtol(argv[i], 0, 0);
      } else if (!strcmp(argv[i], "--end-offset") || !strcmp(argv[i], "-eo")) {
         i++;
         theSettings->maxFileOffset = strtol(argv[i], 0, 0);      
      } else if (!strcmp(argv[i], "--start-hashlength") || !strcmp(argv[i], "-sl")) {
         i++;
         theSettings->minHashLength = strtol(argv[i], 0, 0);
      } else if (!strcmp(argv[i], "--end-hashlength") || !strcmp(argv[i], "-el")) {
         i++;
         theSettings->maxHashLength = strtol(argv[i], 0, 0);
      } else if (!strcmp(argv[i], "--num-hashes") || !strcmp(argv[i], "-nh")) {
         i++;
         theSettings->numHashes = strtol(argv[i], 0, 0);
      } else if (!strcmp(argv[i], "--hash-offset") || !strcmp(argv[i], "-ho")) {
         i++;
	 theSettings->hashOffset = strtol(argv[i], 0, 0);
      } else if (!strcmp(argv[i], "--hash-relative-offset") || !strcmp(argv[i], "-hro")) {
         i++;
         theSettings->hashRelativeOffset = strtol(argv[i], 0, 0);
      } else {
         theSettings->fileName = (char *) malloc(strlen(argv[i])+1);
	 strcpy(theSettings->fileName, argv[i]);
      }
   }
}

void printSettings(settings_t *theSettings, FILE *fd)
{
   fprintf(fd, "Filename: %s\n", theSettings->fileName);
   fprintf(fd, "Current File Offset: 0x%08x\n", theSettings->curFileOffset);
   fprintf(fd, "Max File Offset: 0x%08x\n", theSettings->maxFileOffset);
   fprintf(fd, "Min Hash Length: 0x%08x\n", theSettings->minHashLength);
   fprintf(fd, "Max Hash Length: 0x%08x\n", theSettings->maxHashLength);
   fprintf(fd, "Number of Hashes: %d\n", theSettings->numHashes);
   fprintf(fd, "Hashtable Offset: 0x%08x\n", theSettings->hashOffset);
   fprintf(fd, "Hash Entry Relative Offset: 0x%08x\n", theSettings->hashRelativeOffset);
}

void printHashEntry(hashEntry *entry, FILE *fd)
{
   fprintf(fd, "Hash: ");
   int j;
   for (j=0; j<20; ++j)
           fprintf(fd, "%02X", entry->hash[j]);
   fprintf(fd, "\n");
   fprintf(fd, "Loaded From: File Offset 0x%08x\n", entry->hashOffset);
   if (entry->fileOffset && entry->hashSize) {
      fprintf(fd, "Hash is of %d bytes starting from 0x%08x.\n", entry->hashSize, entry->fileOffset);
   }
}
      
   
int main(int argc, char **argv)
{
   SHA1Context sha;
   uint8_t *buf;
   uint8_t res[20];
   int i, j, k;
   FILE *fp = 0;
   settings_t theSettings;
   
   if (argc == 1) {
      usage(argv[0]);
      exit(1);
   }   
   
   defaultSettings(&theSettings);
   
   readSettings(&theSettings, argc, argv);
   printSettings(&theSettings, stdout);
   
   theSettings.hashEntries = (hashEntry *) malloc(sizeof(hashEntry) * theSettings.numHashes);
   
   memset(theSettings.hashEntries, 0x00, sizeof(hashEntry)*theSettings.numHashes);
   
   fp = fopen(theSettings.fileName, "rb");
   
   fseek(fp, theSettings.hashOffset, SEEK_SET);
   
   for (i = 0; i < theSettings.numHashes; i++) {
      //initHashEntry(&theSettings.hashEntries[i]);
      fread(theSettings.hashEntries[i].hash, sizeof(uint8_t), 20, fp);
      theSettings.hashEntries[i].hashOffset = ftell(fp);
      fseek(fp, theSettings.hashRelativeOffset, SEEK_CUR);
      printHashEntry(&theSettings.hashEntries[i], stdout);
   }
   
   
   buf = (uint8_t *) malloc(sizeof(uint8_t) * theSettings.maxHashLength);
   
   while (theSettings.curFileOffset < theSettings.maxFileOffset) {
      fseek(fp, theSettings.curFileOffset, SEEK_SET);
      fread(buf, sizeof(uint8_t), theSettings.maxHashLength, fp);
      
      // yeah okay so I should have called it "ts", wanna fight about it?
      for (theSettings.curHashLength = theSettings.minHashLength; theSettings.curHashLength <= theSettings.maxHashLength; theSettings.curHashLength++) {
         SHA1Reset(&sha);
	 SHA1Input(&sha, buf, theSettings.curHashLength);
	 SHA1Result(&sha, res);
	 for (i = 0; i < theSettings.numHashes; i++) {
	    k = 1;
	    for (j = 0; j < 20; j++) {
	       if (res[j] != theSettings.hashEntries[i].hash[j]) {
	          k = 0;
	       }
	    }
	    if (k) {
	       printf("-------------------------------------\nMatch Found!\nHash Entry #%d\n", i);
	       theSettings.hashEntries[i].fileOffset = theSettings.curFileOffset;
	       theSettings.hashEntries[i].hashSize = theSettings.curHashLength;
	       printHashEntry(&theSettings.hashEntries[i], stdout);
	       theSettings.curFileOffset += theSettings.curHashLength-1;
	    }
	 }
      }
      theSettings.curFileOffset++;
   }
}
