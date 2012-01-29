#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pirs_main.h"
#include "pirs_datatypes.h"
#include "pirs_alloc.h"
#include "pirs_desc.h"
#include "util.h"

/* *INDENT-OFF* */
static pirs_desc_languages_t languages[] = {
 { P_OFF_DESC_ENGLISH,		"English"	},
 { P_OFF_DESC_UNKNOWN0,		"Unknown 0"	},
 { P_OFF_DESC_GERMAN,		"German"	},
 { P_OFF_DESC_FRENCH,		"French"	},
 { P_OFF_DESC_SPANISH,		"Spanish"	},
 { P_OFF_DESC_ITALIAN,		"Italian"	},
 { P_OFF_DESC_UNKNOWN1,		"Unknown 1"	},
 { P_OFF_DESC_UNKNOWN2,		"Unknown 2"	},
 { P_OFF_DESC_PORTUGUESE,	"Portuguese"	},
 { 0, 0 }
};
/* *INDENT-ON* */

pirs_object *pirs_get_descriptions(pirs_t * pirs)
{
    pirs_dt_table *table;
    const char *desc_utf8;
    int height;
    int i, j;

    /* Calculate the number of descriptions */
    height = 0;

    for (i = 0; languages[i].offset; i++) {
        desc_utf8 = (const char *) &(pirs->buf[languages[i].offset + 2]);

        if (desc_utf8[0] == '\0')
            continue;

        height++;
    }

    table = pirs_alloc_table(2, height * 2, "Description");

    table->header[0] = "Language";
    table->header[1] = "Description";

    j = 0;

    for (i = 0; languages[i].offset; i++) {
        char desc_iso[P_SIZ_DESC];

        desc_utf8 = (const char *) &(pirs->buf[languages[i].offset + 2]);

        if (desc_utf8[0] == '\0')
            continue;

        utf8_to_iso(desc_utf8, desc_iso);

        table->entries[j++] = pirs_alloc_table_ent(String, (unsigned long) strdup(languages[i].language));
        table->entries[j++] = pirs_alloc_table_ent(String, (unsigned long) strdup(desc_iso));
    }

    return pirs_alloc_object(Table, table);
}
