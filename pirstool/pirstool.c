//PIRS compatible Generic Xbox 360 File Archive Tool
//(c) 2005 John Kelley - http://free60.org

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include "mytypes.h"

//for MSVC8 aka VS2005
#define _CRT_SECURE_NO_DEPRECATE

//to make it easier to debug from MSVC or other IDEs
#if _DEBUG || DEBUG
#ifdef WIN32
#define pause() getch()
#else
#define pause() getkbchar()
#endif // WIN32
#else
#define pause() {}
#endif // DEBUG

#define DIR_OFFSET			0xC000
#define CONTENT_TITLE		0x0410
#define CONTENT_DESC		0x0d10
#define CONTENT_PUBLISHER	0x1610

#pragma pack(push,1)
typedef struct {
	char		name[38];
	u_int32_t	unk1, blockLen, offset;
	u_int16_t	parent;
	u_int32_t	size, unk2, unk3;
} fileEntry;
#pragma pack(pop)

void printUsage() {
	printf("Usage: pirstool [l] [e] srcfile [destfile]\n");
}

//Recursively build a filename path
void sprintFileEntry(char *out, fileEntry files[], int index) {
	if (files[index].parent != 0xFFFF)
		sprintFileEntry(out, files, bswap16(files[index].parent));
	strcat(out, "/");
	strcat(out, files[index].name);
}

int main(int argc, char *argv[]) {
	fileEntry files[64];
	wchar_t info[128];
	FILE *in;
	char magicNum[4];
	char filePath[1024];
	int i;

	//verify struct size
	if (sizeof(fileEntry) != 64) {
		printf("fileEntry struct is not packed correctly with the compiler you used. (%d vs 64 bytes)\n", sizeof(fileEntry));
		pause();
		return 1;
	}

	//check # of args
	if (argc < 3) {
		printUsage();
		pause();
		return 1;
	}

	//only support list for now
	if (argv[1][0] != 'l') {
		printUsage();
		pause();
		return 1;
	}

	//open and verify file
	in = fopen(argv[2], "rb");
	if (in == NULL) {
		printf("Error opening '%s' for reading.\n", argv[2]);
		pause();
		return 1;
	}

	//verify magic number
	fread(magicNum, 4, 1, in);
	if (strncmp(magicNum, "PIRS", 4) != 0 && strncmp(magicNum, "LIVE", 4) != 0) {
		printf("File is not in a PIRS compatible format.\n");
		fclose(in);
		pause();
		return 1;
	}

	//print out archive info
	fseek(in, CONTENT_TITLE, SEEK_SET);
	fread(info, sizeof(wchar_t), 128, in);
	if (wcslen(&info[1]) > 0)
		printf("Title: %S\n", &info[1]);
	else
		printf("Title: <none specified>\n");
	fseek(in, CONTENT_DESC, SEEK_SET);
	fread(info, sizeof(wchar_t), 128, in);
	if (wcslen(&info[1]) > 0)
		printf("Desc: %S\n", &info[1]);
	fseek(in, CONTENT_PUBLISHER, SEEK_SET);
	fread(info, sizeof(wchar_t), 128, in);
	if (wcslen(&info[1]) > 0)
		printf("Publisher: %S\n", &info[1]);
	
	//warnings
	printf("\nWARNING: currently offsets aren't calulated correctly.\nOnly file name, parent and size are 100%% correct.\nNote offsets are also one unit higher than they should be\n\n");

	//print out directory contents
	printf("%-40s %9s Offset (from 0x%08X)\n", "Filename", "Size", DIR_OFFSET);
	fseek(in, DIR_OFFSET, SEEK_SET);
	fread(files, sizeof(fileEntry), 64, in);
	for(i=0; files[i].name[0] != 0; i++) {
		if (files[i].offset != 0) {	//skip directories
			filePath[0] = 0;
			sprintFileEntry(filePath, files, i);
			printf("%-40s %9d 0x%08X\n", filePath, bswap32(files[i].size), bswap32(files[i].offset));
		}
	}
	fclose(in);
	pause();
	return 0;
}
