#ifndef XEX_DATATYPES_H
#define XEX_DATATYPES_H

/* This header defines an abstraction layer to the objects retrieved from the XEX loader */

typedef enum {
	Table,
	Line
} xex_dt_object;

typedef enum {
	Hexa32,
	Bits,
	String
} xex_dt_field;

typedef struct {
	xex_dt_field type;
	unsigned long content;
} xex_dt_table_ent;

typedef struct {
	unsigned int width;
	unsigned int nent;	/* Number of entries */
	char *name;
	char **header;
	int noheader;
	xex_dt_table_ent **entries;
} xex_dt_table;

typedef struct {
	void *ptr;
} xex_dt_line;

typedef struct {
	xex_dt_object type;
	union {
		xex_dt_table *table;
		xex_dt_line *line;
		void *ptr;
	};
} xex_object;

xex_dt_table *xex_alloc_table (unsigned int width, unsigned int height, char *name);
xex_object *xex_alloc_object (xex_dt_object type, void *ptr);
xex_dt_table_ent *xex_alloc_table_ent (xex_dt_field type, unsigned long content);

#endif
