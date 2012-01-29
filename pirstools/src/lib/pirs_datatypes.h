#ifndef PIRS_DATATYPES_H
#define PIRS_DATATYPES_H

/* This header defines an abstraction layer to the objects retrieved from the PIRS loader */

typedef enum {
	Table,
	Line
} pirs_dt_object;

typedef enum {
	Hexa32,
	Hexa16,
	Bits,
	String
} pirs_dt_field;

typedef struct {
	pirs_dt_field type;
	unsigned long content;
} pirs_dt_table_ent;

typedef struct {
	unsigned int width;
	unsigned int nent;	/* Number of entries */
	char *name;
	char **header;
	int noheader;
	pirs_dt_table_ent **entries;
} pirs_dt_table;

typedef struct {
	void *ptr;
} pirs_dt_line;

typedef struct {
	pirs_dt_object type;
	union {
		pirs_dt_table *table;
		pirs_dt_line *line;
		void *ptr;
	};
} pirs_object;


struct _pirs_chunk {
	unsigned long start;
	unsigned long end;
	struct _pirs_chunk *next;
};

typedef struct _pirs_chunk pirs_chunk;

#define DWORD unsigned long

#endif
