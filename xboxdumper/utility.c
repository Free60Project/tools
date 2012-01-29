#include <stdarg.h>
#include <ctype.h>
#include <stddef.h>
char makehexchar(int i) {
	return (i<=9) ? '0'+i : 'A'+(i-10);
}

int hextoint(char c) {
	c = toupper(c);
	return (c > '9' ? c - 'A' + 10 : c - '0');
}

void hexdump(const unsigned char* data, size_t length) {
	const int char_offset = 16*3 + 2;
	const int line_size = 16*3 + 16 + 3;
	char line[line_size+1];
    size_t i;
    int line_i;

	printf("OFFSET  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F | 0123456789ABCDEF\n");
	printf("--------------------------------------------------------------------------\n");

	line[char_offset - 1] = ' ';
	line[char_offset - 2] = ' ';

	for (i=0; i<length; ) {
		int bi=0;
		int ci=0;

		int start_i = i;

		for (line_i=0; i < length && line_i < 16; i++, line_i++) {
			line[bi++] = makehexchar(*data>>4);
			line[bi++] = makehexchar(*data & 0x0f);
			line[bi++] = ' ';
			line[char_offset+(ci++)]=(isprint(*data) ? *data : '.');
			data++;
		}

		while (bi<16*3) {
			line[bi++]=' ';
		}

		line[char_offset+(ci++)]='\n';
		line[char_offset+ci]=0;

		printf("%06X  %s", start_i, line);
	}
}

int char_value(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'z') return c - 'a';
	if (c >= 'A' && c <= 'Z') return c - 'A';
	return 0;
}


