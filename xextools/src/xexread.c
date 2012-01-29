#include <stdio.h>
#include <libxex.h>
#include "xex_display.h"

void usage (char *program)
{
        printf ("Usage: %s <xex file>\n", program);
}

int main (int argc, char **argv)
{
	xex_t *xex;
	xex_object **objects;

	if (argc < 2) {
		usage (argv[0]);
		return 1;
	}

	xex = xex_load (argv[1]);

	/* Make sure the file is a XEX File */

	if (!xex_validate (xex)) {
		printf ("Wrong file type, not a X360 Executable file\n");
		goto cleanup;
	}

	printf ("Filename    : %s\n\n", argv[1]);

	objects = xh_get_misc_info (xex);

	xex_print_objects (objects);

	xex_object *obj = xh_read_generic_table (xex);

	xex_print_table (obj->table);

	objects = xh_info_load_generic (xex, obj->table);

	xex_print_objects (objects);

	objects = xex_read_certificate (xex);

	xex_print_objects (objects);

cleanup:
	xex_unload (xex);

	return 0;
}
