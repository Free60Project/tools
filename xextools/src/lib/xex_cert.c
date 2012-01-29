#include <stdlib.h>
#include "swissknife.h"
#include "xex_alloc.h"
#include "xex_main.h"
#include "xex_cert.h"

xex_object **xex_read_certificate (xex_t *xex) {
        xex_dt_table *cert, *trailer;
        xex_object **objects;
        DWORD cert_off, cert_siz;
        int i;

        objects = malloc (sizeof (xex_object *) * 3);
        
        cert_off = get_dword (xex, X_OFF_CERT);
        cert_siz = get_dword (xex, cert_off);
        
        cert = xex_alloc_table (4, 256 / 4, "Certificate");
        cert->noheader = 1;
        
        objects[0] = xex_alloc_object (Table, cert);
        
        for (i = 0; i < 256 / 4; i++) {
                DWORD ent = get_dword (xex, cert_off + 8 + i * 4);
                cert->entries[i] = xex_alloc_table_ent (Hexa32, ent);
        }
        
        trailer = xex_alloc_table (4, (cert_siz / 4) - 4 - 256 / 4, "Certificate Trailer");
        trailer->noheader = 1;
        
        for (i = 0; i < (cert_siz / 4) - 4 - 256 / 4; i++) {
                DWORD ent = get_dword (xex, cert_off + 8 + 256 + i * 4);
                trailer->entries[i] = xex_alloc_table_ent (Hexa32, ent);
        }
        
        objects[1] = xex_alloc_object (Table, trailer);
        objects[2] = NULL;
        
        return objects;

}
