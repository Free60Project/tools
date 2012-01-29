#ifndef XH_INFO_H
#define XH_INFO_H

#include "xex_datatypes.h"

typedef enum {
        INFO_INIT = 0,
        INFO_EXEC = 1 << 0, 
        INFO_LOAD = 1 << 1, 
        INFO_UNKNOWN_0 = 1 << 2
} xh_info_t;

xex_object **xh_info_load_generic (xex_t *xex, xex_dt_table *table);
xex_object *xh_read_generic_table (xex_t *xex);

#define xh_info_execute(table, id, arg1, arg2, arg3, exec, ret) do {	\
        int i;                                                  	\
        for (i = 0; table[i].handler; i++) {                    	\
                if (table[i].id == id) {                        	\
                        ret = table[i].handler (arg1, arg2, arg3);    	\
                        exec = 1;                                	\
                }                                               	\
        }                                                       	\
} while (0)

#endif
