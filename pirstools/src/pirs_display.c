#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "pirs_display.h"

void pirs_print_table(pirs_dt_table * table)
{
    char bits[36];
    int *colsizes;
    int height;
    int i, j;

    printf("%s\n", table->name);

    if (!table->noheader)
        printf("\n");

    printf("      ");

    colsizes = malloc(sizeof(int) * table->width);

    height = ((float) table->nent / (float) table->width + .99);

    for (i = 0; i < table->width; i++) {
        int len;

        if (table->noheader)
            colsizes[i] = 8;
        else {
            len = strlen(table->header[i]);

            colsizes[i] = len < 8 ? 9 : len + 2;
        }

        for (j = 0; j < height && (j * table->width + i) < table->nent; j++) {
            switch (table->entries[j * table->width + i]->type) {
            case Bits:
                colsizes[i] = MAX(colsizes[i], 36 + 2);
                break;
            case String:{
                    int len = strlen((const char *) table->entries[j * table->width + i]->content);
                    colsizes[i] = MAX(colsizes[i], len + 2);
                    break;
                }
            case Hexa32:
            case Hexa16:
            default:
                break;
            }
        }
        if (!table->noheader)
            printf("%-*s ", colsizes[i], table->header[i]);
    }

    printf("\n");

    for (i = 0; i < height; i++) {
        printf("%4d: ", i + 1);
        for (j = 0; j < table->width && (i * table->width + j) < table->nent; j++) {
            switch (table->entries[i * table->width + j]->type) {
            case Hexa32:
                printf("%-*.08lx ", colsizes[j], table->entries[i * table->width + j]->content);
                break;
            case Hexa16:
                printf("%-*.04lx ", colsizes[j], table->entries[i * table->width + j]->content);
                break;
            case Bits:
                sprint_bits(bits, table->entries[i * table->width + j]->content);
                printf("%-*s ", colsizes[j], bits);
                break;
            case String:
                printf("%-*s ", colsizes[j], (char *) table->entries[i * table->width + j]->content);
                break;
            default:
                printf("Unknown table entry type: %d\n", table->entries[i * table->width + j]->type);
                abort();
            }
        }
        printf("\n");
    }

    printf("\n");

    free(colsizes);
}

void pirs_print_object(pirs_object * object)
{
    switch (object->type) {
    case Table:
        pirs_print_table(object->table);
        break;
    case Line:
        break;
    default:
        printf("Unknown object type: %d\n", object->type);
        abort();
    }
}

void pirs_print_objects(pirs_object ** objects)
{
    int i;

    for (i = 0; objects[i]; i++)
        pirs_print_object(objects[i]);
}
