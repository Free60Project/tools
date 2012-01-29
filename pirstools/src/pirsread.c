#include <stdio.h>
#include <libpirs.h>
#include "pirs_display.h"

int main(int argc, char **argv)
{
    pirs_t *pirs;
    pirs_object **content;

    pirs = pirs_load(argv[1]);

    if (pirs == NULL) {
        printf("Couldn't open the pirs file, exiting..\n");
        return 1;
    }

    /* Make sure the file is a PIRS File */

    if (!pirs_validate(pirs)) {
        printf("Wrong file type, not a PIRS file\n");
        goto cleanup;
    }

    printf ("Analyzing file, this may take a while..\n");

    content = pirs_get_all(pirs);

    pirs_print_objects(content);

    pirs_unload(pirs);

    return 0;

  cleanup:
    pirs_unload(pirs);

    return 0;
}
