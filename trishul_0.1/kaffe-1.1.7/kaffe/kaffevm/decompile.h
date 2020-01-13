#ifndef __decompile_h
#define __decompile_h
/**********************************************************************************************************************/
#include "config-int.h"
#include "classMethod.h"
/**********************************************************************************************************************/

const char *instruction_name (const unsigned char *ptr);

int instruction_length (const unsigned char *ptr, const unsigned char *end);

/**********************************************************************************************************************/

typedef enum
{
	STORE_LOCAL_VARIABLE,
	STORE_STACK,
	STORE_FIELD_STATIC,
	STORE_FIELD_OBJECT,
} StorageEntryType;

typedef struct StorageEntry
{
	StorageEntryType	type;

	union
	{
		/* type == STORE_LOCAL_VARIABLE */
		struct
		{
			unsigned int	index;
		} local;

		/* type == STORE_FIELD_STATIC */
		struct
		{
			Field	*field;
		} field_static;

		/* STORE_FIELD_OBJECT */
		struct
		{
			Field 	*field;

			/* The index of the local variable containing the this pointer */
			unsigned int	index_this;

			/* The address at which the this pointer is put into the slot */
			unsigned int	pc_this;
		} field_object;

		/* type == STORE_FIELD_STACK */
		struct
		{
			int	depth;
		} stack;

	} param;

	struct StorageEntry	*next;
} StorageEntry;

typedef uint32_t StorageQuickLocals;

typedef struct StorageRecord
{
	StorageEntry	*first,
					*last;

	/* Quick access to the first local variables; one bit per variable */
	StorageQuickLocals		quick_locals;

	bool					has_throw;
} StorageRecord;

/**********************************************************************************************************************/
#define STORAGE_IS_EMPTY(sr)	(!(sr)->first && !(sr)->quick_locals)
/**********************************************************************************************************************/

struct _jmethodID;
void analyzeBlockStores (errorInfo *einfo, struct _jmethodID *meth, unsigned int pc_start, unsigned int pc_end, StorageRecord *store);

Hjava_lang_Class *detectExceptionType (errorInfo *einfo, struct _jmethodID *meth, unsigned int pc_start, unsigned int pc_end);

StorageEntry *StorageEntryNewFieldStatic (Field *field);
StorageEntry *StorageEntryNewFieldObject (Field *field, unsigned int index_this, unsigned int pc_this);
StorageEntry *StorageEntryNewStack  (int depth);
StorageEntry *StorageEntryNewLocal  (int index);

void storageRecordAddEntry (StorageRecord *store, StorageEntry *entry);
void storageRecordCopyRecord (StorageRecord *store, const StorageRecord *add);

/**********************************************************************************************************************/
#endif /* __decompile_h */
