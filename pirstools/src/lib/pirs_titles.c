#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pirs_main.h"
#include "pirs_datatypes.h"
#include "pirs_alloc.h"
#include "pirs_titles.h"
#include "util.h"

/* *INDENT-OFF* */
static pirs_title_languages_t languages[] = {
 { P_OFF_TIT_ENGLISH,		"English"	},
 { P_OFF_TIT_UNKNOWN0,		"Unknown 0"	},
 { P_OFF_TIT_GERMAN,		"German"	},
 { P_OFF_TIT_FRENCH,		"French"	},
 { P_OFF_TIT_SPANISH,		"Spanish"	},
 { P_OFF_TIT_ITALIAN,		"Italian"	},
 { P_OFF_TIT_UNKNOWN1,		"Unknown 1"	},
 { P_OFF_TIT_UNKNOWN2,		"Unknown 2"	},
 { P_OFF_TIT_PORTUGUESE,	"Portuguese"	},
 { 0, 0 }
};
/* *INDENT-ON* */

pirs_object *
pirs_get_titles (pirs_t * pirs)
{
  pirs_dt_table *table;
  const char *title_utf8;
  int height;
  int i, j;

  /* Calculate the number of titles */
  height = 0;

  for (i = 0; languages[i].offset; i++)
    {
      title_utf8 = (const char *) &(pirs->buf[languages[i].offset + 2]);

      if (title_utf8[0] == '\0')
        continue;

      height++;
    }

  table = pirs_alloc_table (2, height * 2, "Titles");

  table->header[0] = "Language";
  table->header[1] = "Title";

  j = 0;

  for (i = 0; languages[i].offset; i++)
    {
      char title_iso[P_SIZ_TIT];

      title_utf8 = (const char *) &(pirs->buf[languages[i].offset + 2]);

      if (title_utf8[0] == '\0')
        continue;

      utf8_to_iso (title_utf8, title_iso);

      table->entries[j++] = pirs_alloc_table_ent (String, (unsigned long) strdup (languages[i].language));
      table->entries[j++] = pirs_alloc_table_ent (String, (unsigned long) strdup (title_iso));
    }

  return pirs_alloc_object (Table, table);
}
