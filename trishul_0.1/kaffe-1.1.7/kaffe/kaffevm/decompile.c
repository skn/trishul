#include "decompile.h"
#include "classMethod.h"
#include "code-analyse.h"
#include "lookup.h"
#include "utils.h"
/**********************************************************************************************************************/

typedef struct
{
	const char 	*name;
	int 		length;
	uint		flags;
	int			stack;
} Opcode;

/* Special length values to mark variable-length instructions and other unusual instructions */
enum
{
	LENGTH_INVALID		= -1,
	LENGTH_TABLESWITCH	= -2,
	LENGTH_LOOKUPSWITCH	= -3,
	LENGTH_WIDE			= -4,

	STACK_GETFIELD	= -10000,
	STACK_PUTFIELD,
	STACK_GETSTATIC,
	STACK_PUTSTATIC,
	STACK_INVOKE_VIRTUAL,
	STACK_INVOKE_INTERFACE,
	STACK_INVOKE_SPECIAL,
	STACK_INVOKE_STATIC,
	STACK_MULTIANEWARRAY,

	/* Marker to test if the stack value is dynamic */
	STACK_DYNAMIC
};

/* Instruction flags */
enum
{
	/* An if statement which uses 1 or 2 stack values for the test */
	INS_IF_TEST_ONE_VALUE	= 0x01,
	INS_IF_TEST_TWO_VALUES	= 0x02,
	/* Mask used to test for any if-statement */
	INS_IF					= 0x03,

	INS_GOTO				= 0x04,

	/* An explicit wide instruction */
	INS_WIDE				= 0x08,

	/* Storage-related flags */
	INS_STORE_LOCAL			= 0x10 | 0x20 | 0x40 | 0x80,
	INS_STORE_LOCAL_0		= 0x10,
	INS_STORE_LOCAL_1		= 0x20,
	INS_STORE_LOCAL_2		= 0x30,
	INS_STORE_LOCAL_3		= 0x40,
	INS_STORE_LOCAL_NDX		= 0x80,

	/* Stores into static members */
	INS_STORE_STATIC		= 0x100,
	/* Stores into object members */
	INS_STORE_FIELD			= 0x200,

	/* Modifies a location on the stack */
	INS_STORE_STACK			= 0x400,

	INS_ALOAD				= 0x1000 | 0x2000 | 0x4000 | 0x8000,
	INS_ALOAD_0				= 0x1000,
	INS_ALOAD_1				= 0x2000,
	INS_ALOAD_2				= 0x3000,
	INS_ALOAD_3				= 0x4000,
	INS_ALOAD_NDX			= 0x8000,

	INS_ASTORE				= 0x10000 | 0x20000 | 0x40000 | 0x80000,
	INS_ASTORE_0			= 0x10000,
	INS_ASTORE_1			= 0x20000,
	INS_ASTORE_2			= 0x30000,
	INS_ASTORE_3			= 0x40000,
	INS_ASTORE_NDX			= 0x80000,

	/* Specific instructions, cannot be combined */
	MASK_INSTRUCTION		= 0x4FFFFFFF,
	INS_NEW					= 0x40000000,
	INS_DUP,
	INS_DUP_X1,
	INS_ATHROW,
	INS_IINC,
};

#define IS_INSTRUCTION(ins,type) (((ins)->flags & MASK_INSTRUCTION) == type)
static const Opcode OPCODES[256];

#define NO_OPCODE()				{"INVALID", -1,	0, 0}

/**********************************************************************************************************************/

static const Opcode OPCODES[256] =
{
/* 0x00 */
	{"nop", 			0, 						0, 							0},
	{"aconst_null", 	0, 						INS_STORE_STACK, 			1},
	{"iconst_m1", 		0, 						INS_STORE_STACK, 			1},
	{"iconst_0", 		0, 						INS_STORE_STACK, 			1},
	{"iconst_1", 		0, 						INS_STORE_STACK, 			1},
	{"iconst_2", 		0, 						INS_STORE_STACK, 			1},
	{"iconst_3", 		0, 						INS_STORE_STACK, 			1},
	{"iconst_4", 		0, 						INS_STORE_STACK, 			1},
	{"iconst_5", 		0, 						INS_STORE_STACK, 			1},
	{"lconst_0", 		0, 						INS_STORE_STACK, 			2},
	{"lconst_1", 		0, 						INS_STORE_STACK, 			2},
	{"fconst_0", 		0, 						INS_STORE_STACK, 			1},
	{"fconst_1", 		0, 						INS_STORE_STACK, 			1},
	{"fconst_2", 		0, 						INS_STORE_STACK, 			1},
	{"dconst_0", 		0, 						INS_STORE_STACK, 			2},
	{"dconst_1", 		0, 						INS_STORE_STACK, 			2},
/* 0x10 */
	{"bipush", 			1, 						0, 							1},
	{"sipush", 			2, 						0, 							1},
	{"ldc", 			1, 						0, 							1},
	{"ldc_w", 			2, 						0, 							1},
	{"ldc2_w", 			2, 						0, 							2},
	{"iload", 			1, 						INS_STORE_STACK, 			1},
	{"lload", 			1, 						INS_STORE_STACK, 			2},
	{"fload", 			1, 						INS_STORE_STACK, 			1},
	{"dload", 			1, 						INS_STORE_STACK, 			2},
	{"aload", 			1, 						INS_ALOAD_NDX | INS_STORE_STACK, 			1},
	{"iload_0", 		0, 						INS_STORE_STACK, 			1},
	{"iload_1", 		0, 						INS_STORE_STACK, 			1},
	{"iload_2", 		0, 						INS_STORE_STACK, 			1},
	{"iload_3", 		0, 						INS_STORE_STACK, 			1},
	{"lload_0", 		0, 						INS_STORE_STACK, 			2},
	{"lload_1", 		0, 						INS_STORE_STACK, 			2},
/* 0x20 */
	{"lload_2", 		0, 						INS_STORE_STACK, 			2},
	{"lload_3", 		0, 						INS_STORE_STACK, 			2},
	{"fload_0", 		0, 						INS_STORE_STACK, 			1},
	{"fload_1", 		0, 						INS_STORE_STACK, 			1},
	{"fload_2", 		0, 						INS_STORE_STACK, 			1},
	{"fload_3", 		0, 						INS_STORE_STACK, 			1},
	{"dload_0", 		0, 						INS_STORE_STACK, 			2},
	{"dload_1", 		0, 						INS_STORE_STACK, 			2},
	{"dload_2", 		0, 						INS_STORE_STACK, 			2},
	{"dload_3", 		0, 						INS_STORE_STACK, 			2},
	{"aload_0", 		0, 						INS_ALOAD_0 | INS_STORE_STACK, 			1},
	{"aload_1", 		0, 						INS_ALOAD_1 | INS_STORE_STACK, 			1},
	{"aload_2", 		0, 						INS_ALOAD_2 | INS_STORE_STACK, 			1},
	{"aload_3", 		0, 						INS_ALOAD_3 | INS_STORE_STACK, 			1},
	{"iaload", 			0, 						0, 							-1},
	{"laload", 			0, 						0, 							0},
/* 0x30 */
	{"faload", 			0, 						0, 							-1},
	{"daload", 			0, 						0, 							0},
	{"aaload", 			0, 						0, 							-1},
	{"baload", 			0, 						0, 							-1},
	{"caload", 			0, 						0, 							-1},
	{"saload", 			0, 						0, 							-1},
	{"istore", 			1, 						INS_STORE_LOCAL_NDX, 		-1},
	{"lstore", 			1, 						INS_STORE_LOCAL_NDX, 		-2},
	{"fstore", 			1, 						INS_STORE_LOCAL_NDX, 		-1},
	{"dstore", 			1, 						INS_STORE_LOCAL_NDX, 		-2},
	{"astore", 			1, 						INS_ASTORE_NDX | INS_STORE_LOCAL_NDX, 		-1},
	{"istore_0", 		0, 						INS_STORE_LOCAL_0, 			-1},
	{"istore_1", 		0, 						INS_STORE_LOCAL_1, 			-1},
	{"istore_2", 		0, 						INS_STORE_LOCAL_2, 			-1},
	{"istore_3", 		0, 						INS_STORE_LOCAL_3, 			-1},
	{"lstore_0", 		0, 						INS_STORE_LOCAL_0, 			-2},
/* 0x40 */
	{"lstore_1", 		0, 						INS_STORE_LOCAL_1, 			-2},
	{"lstore_2", 		0, 						INS_STORE_LOCAL_2, 			-2},
	{"lstore_3", 		0, 						INS_STORE_LOCAL_3, 			-2},
	{"fstore_0", 		0, 						INS_STORE_LOCAL_0, 			-1},
	{"fstore_1", 		0, 						INS_STORE_LOCAL_1, 			-1},
	{"fstore_2", 		0, 						INS_STORE_LOCAL_2, 			-1},
	{"fstore_3", 		0, 						INS_STORE_LOCAL_3, 			-1},
	{"dstore_0", 		0, 						INS_STORE_LOCAL_0, 			-2},
	{"dstore_1", 		0, 						INS_STORE_LOCAL_1, 			-2},
	{"dstore_2", 		0, 						INS_STORE_LOCAL_2, 			-2},
	{"dstore_3", 		0, 						INS_STORE_LOCAL_3, 			-2},
	{"astore_0", 		0, 						INS_ASTORE_0 | INS_STORE_LOCAL_0, 			-1},
	{"astore_1", 		0, 						INS_ASTORE_1 | INS_STORE_LOCAL_1, 			-1},
	{"astore_2", 		0, 						INS_ASTORE_2 | INS_STORE_LOCAL_2, 			-1},
	{"astore_3", 		0, 						INS_ASTORE_3 | INS_STORE_LOCAL_3, 			-1},
	{"iastore", 		0, 						0, 							-3},
/* 0x50 */
	{"lastore", 		0, 						0, 							-4},
	{"fastore", 		0, 						0, 							-3},
	{"dastore", 		0, 						0, 							-4},
	{"aastore", 		0, 						0, 							-3},
	{"bastore", 		0, 						0, 							-3},
	{"castore", 		0, 						0, 							-3},
	{"sastore", 		0, 						0, 							-3},
	{"pop", 			0, 						0, 							-1},
	{"pop2", 			0, 						0, 							-2},
	{"dup", 			0, 						INS_DUP, 					1},
	{"dup_x1", 			0, 						INS_DUP_X1, 				1},
	{"dup_x2", 			0, 						0, 							1},
	{"dup2", 			0, 						0, 							2},
	{"dup2_x1", 		0, 						0, 							2},
	{"dup2_x2", 		0, 						0, 							2},
	{"swap", 			0, 						0, 							0},
/* 0x60 */
	{"iadd",			0, 						0, 							-1},
	{"ladd",			0, 						0, 							-2},
	{"fadd",			0, 						0, 							-1},
	{"dadd",			0, 						0, 							-2},
	{"isub",			0, 						0, 							-1},
	{"lsub",			0, 						0, 							-2},
	{"fsub",			0, 						0, 							-1},
	{"dsub",			0, 						0, 							-2},
	{"imul",			0, 						0, 							-1},
	{"lmul",			0, 						0, 							-2},
	{"fmul",			0, 						0, 							-1},
	{"dmul",			0, 						0, 							-2},
	{"idiv",			0, 						0, 							-1},
	{"ldiv",			0, 						0, 							-2},
	{"fdiv",			0, 						0, 							-1},
	{"ddiv",			0, 						0, 							-2},
/* 0x70 */
	{"irem",			0, 						0, 							-1},
	{"lrem",			0, 						0, 							-2},
	{"frem",			0, 						0, 							-1},
	{"drem",			0, 						0, 							-2},
	{"ineg",			0, 						0, 							0},
	{"lneg",			0, 						0, 							0},
	{"fneg",			0, 						0, 							0},
	{"dneg",			0, 						0, 							0},
	{"ishl",			0, 						0, 							-1},
	{"lshl",			0, 						0, 							-2},
	{"ishr",			0, 						0, 							-1},
	{"lshr",			0, 						0, 							-2},
	{"iushr",			0, 						0, 							-1},
	{"lushr",			0, 						0, 							-2},
	{"iand",			0, 						0, 							-1},
	{"land",			0, 						0, 							-2},
/* 0x80 */
	{"ior",				0, 						0, 							-1},
	{"lor",				0, 						0, 							-2},
	{"ixor",			0, 						0, 							-1},
	{"lxor",			0, 						0, 							-2},
	{"iinc", 			2, 						INS_IINC,					0},
	{"i2l", 			0, 						0, 							1},
	{"i2f", 			0, 						0, 							0},
	{"i2d", 			0, 						0, 							1},
	{"l2i", 			0, 						0, 							-1},
	{"l2f", 			0, 						0, 							-1},
	{"l2d", 			0, 						0, 							0},
	{"f2i", 			0, 						0, 							0},
	{"f2l", 			0, 						0, 							1},
	{"f2d", 			0, 						0, 							1},
	{"d2i", 			0, 						0, 							-1},
	{"d2l", 			0, 						0, 							0},
/* 0x90 */
	{"d2f", 			0, 						0, 							-1},
	{"i2b", 			0, 						0, 							0},
	{"i2c", 			0, 						0, 							0},
	{"i2s", 			0, 						0, 							0},
	{"lcmp", 			0, 						0, 							-2},
	{"fcmpl", 			0, 						0, 							-1},
	{"fcmpg", 			0, 						0, 							-1},
	{"dcmpl", 			0, 						0, 							-2},
	{"dcmpg", 			0, 						0, 							-2},
	{"ifeq", 			2, 						INS_IF_TEST_ONE_VALUE, 		-1},
	{"ifne", 			2, 						INS_IF_TEST_ONE_VALUE, 		-1},
	{"iflt", 			2, 						INS_IF_TEST_ONE_VALUE, 		-1},
	{"ifge", 			2, 						INS_IF_TEST_ONE_VALUE, 		-1},
	{"ifgt", 			2, 						INS_IF_TEST_ONE_VALUE, 		-1},
	{"ifle", 			2, 						INS_IF_TEST_ONE_VALUE, 		-1},
	{"if_icmpeq", 		2, 						INS_IF_TEST_TWO_VALUES, 	-1},
/* 0xA0 */
	{"if_icmpne", 		2, 						INS_IF_TEST_TWO_VALUES, 	-1},
	{"if_icmplt", 		2, 						INS_IF_TEST_TWO_VALUES, 	-1},
	{"if_icmpge", 		2, 						INS_IF_TEST_TWO_VALUES, 	-1},
	{"if_icmpgt", 		2, 						INS_IF_TEST_TWO_VALUES, 	-1},
	{"if_icmple", 		2, 						INS_IF_TEST_TWO_VALUES, 	-1},
	{"if_acmpeq", 		2, 						INS_IF_TEST_TWO_VALUES, 	-1},
	{"if_acmpne", 		2, 						INS_IF_TEST_TWO_VALUES, 	-1},
	{"goto", 			2, 						INS_GOTO, 					0},
	{"jsr", 			2, 						0, 							1},
	{"ret", 			1, 						0, 							-1},
	{"tableswitch", 	LENGTH_TABLESWITCH, 	0, 							-1},
	{"lookupswitch", 	LENGTH_LOOKUPSWITCH, 	0, 							-1},
	{"ireturn",			0, 						0, 							-1},
	{"lreturn", 		0, 						0, 							-2},
	{"freturn", 		0, 						0, 							-1},
	{"dreturn", 		0, 						0, 							-2},
/* 0xB0 */
	{"areturn", 		0, 						0, 							-1},
	{"return", 			0, 						0, 							0},
	{"getstatic", 		2, 						0, 							STACK_GETSTATIC},
	{"putstatic", 		2, 						INS_STORE_STATIC, 			STACK_PUTSTATIC},
	{"getfield", 		2, 						0, 							STACK_GETFIELD},
	{"putfield", 		2, 						INS_STORE_FIELD, 			STACK_PUTFIELD},
	{"invokevirtual", 	2, 						0, 							STACK_INVOKE_VIRTUAL},
	{"invokespecial", 	2, 						0, 							STACK_INVOKE_SPECIAL},
	{"invokestatic", 	2, 						0, 							STACK_INVOKE_STATIC},
	{"invokeinterface", 4, 						0, 							STACK_INVOKE_INTERFACE},
	NO_OPCODE(),
	{"new", 			2, 						INS_NEW,					1},
	{"newarray", 		1, 						INS_STORE_STACK,			0},
	{"anewarray", 		2, 						INS_STORE_STACK, 			0},
	{"arraylength", 	0, 						0, 							0},
	{"athrow", 			0, 						INS_ATHROW,					0},
/* 0xC0 */
	{"checkcast", 		2, 						0, 							0},
	{"instanceof", 		2, 						0, 							0},
	{"monitorenter", 	0, 						0, 							-1},
	{"monitorexit", 	0, 						0, 							-1},
	{"wide", 			LENGTH_WIDE, 			0, 							0},
	{"multianewarray", 	3, 						0, 							STACK_MULTIANEWARRAY},
	{"ifnull", 			2, 						INS_IF_TEST_ONE_VALUE, 		-1},
	{"ifnonnull", 		2, 						INS_IF_TEST_ONE_VALUE, 		-1},
	{"goto_w", 			4, 						INS_GOTO | INS_WIDE, 		0},
	{"jsr_w", 			4, 						INS_WIDE, 					0},

	/* No used opcodes beyond here */
	NO_OPCODE(),  		NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
/* 0xD0 */
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
/* 0xE0 */
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
/* 0xF0 */
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
	NO_OPCODE(), 		NO_OPCODE(),		NO_OPCODE(), 			NO_OPCODE(),
};

#undef OPCODE

/**********************************************************************************************************************/

static int read_int (const uint8 *ptr)
{
	int b1 = *ptr; ptr++;
	int b2 = *ptr; ptr++;
	int b3 = *ptr; ptr++;
	int b4 = *ptr;

	return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

/**********************************************************************************************************************/

const char *instruction_name (const uint8 *ptr)
{
	const Opcode *ins = &OPCODES[*ptr];
	return ins->name;
}

/**********************************************************************************************************************/
#define ALIGN_4(ptr)	((ptr) + ((((uint) (ptr)) % 4) ? (4 - (((uint) (ptr)) % 4)) : 0))

int instruction_length (const uint8 *ptr, const uint8 *end)
{
	const Opcode *ins = &OPCODES[*ptr];
	int length = ins->length;

	// TODO: check extra bytes are present, ie ptr doesn't overrun end
	switch (length)
	{
	case LENGTH_INVALID:
		// TODO: can we recover from this? This should probably not happen when fully implemented
		abort ();
		break;
	case LENGTH_WIDE:
		if (IS_INSTRUCTION (&OPCODES[*(ptr + 1)], INS_IINC))
		{
			return 6;
		}

		// TODO: implement
		abort ();
		break;
	case LENGTH_TABLESWITCH:
		{
			const unsigned char *startPtr = ptr;
			ptr = ALIGN_4 (ptr + 1);

			/*int def  = read_int (ptr);*/ ptr += sizeof (int); // default not needed
			int low  = read_int (ptr); ptr += sizeof (int);
			int high = read_int (ptr); ptr += sizeof (int);

			int count = high - low + 1;
			ptr += count * sizeof (int);

			length = ptr - startPtr;
		}
		break;
	case LENGTH_LOOKUPSWITCH:
		{
			const unsigned char *startPtr = ptr;
			ptr = ALIGN_4 (ptr + 1);

			/*int def  = read_int (ptr);*/ ptr += sizeof (int); // default not needed
			int count = read_int (ptr); ptr += sizeof (int);
			ptr += count * 2 * sizeof (int);

			length = ptr - startPtr;
		}
		break;
	default:
		length++; /* For opcode byte */
		if (ptr + length > end)
		{
			abort ();
		}
	}
	return length;
}

/**********************************************************************************************************************/

static int getFieldSize (const Field *field)
{
	// Convert bytes to slot count
	if (field->bsize <= 4) return 1;
	else return 2;
}

/**********************************************************************************************************************/

static int getInvokeStackChange (const callInfo *call, int idx)
{
	const char *sig;
	sig = call->signature->data;
	assert(sig[0] == '(');
	sig++;

	while (sig[0] != ')') {
		switch (sig[0]) {
		case '[':
			idx -= 1;
			while (sig[0] == '[') {
				sig++;
			}
			if (sig[0] == 'L') {
				while (sig[0] != ';') {
					sig++;
				}
			}
			sig++;
			break;
		case 'L':
			idx -= 1;
			while (sig[0] != ';') {
				sig++;
			}
			sig++;
			break;
		case 'I':
		case 'Z':
		case 'S':
		case 'B':
		case 'C':
		case 'F':
			idx -= 1;
			sig++;
			break;
		case 'J':
		case 'D':
			idx -= 2;
			sig++;
			break;
		default:
			assert("Signature character unknown" == 0);
		}
	}
	switch (call->rettype) {
	case '[':
	case 'L':
	case 'I':
	case 'Z':
	case 'S':
	case 'B':
	case 'C':
	case 'F':
		idx += 1;
		break;
	case 'J':
	case 'D':
		idx += 2;
		break;
	case 'V':
		break;
	default:
		break;
	}

	return idx;
}

/**********************************************************************************************************************/

/**
 * Any errors are signalled through einfo.
 */
static int getStackChange (errorInfo *einfo, Method *meth, const Opcode *ins, uint pc)
{
	fieldInfo 	finfo;
	callInfo 	call;


	if (ins->stack < STACK_DYNAMIC)
	{
		int stackChange;

		switch (ins->stack)
		{
		case STACK_GETSTATIC:
			if (!getField(WORD(pc+1), meth->class, true, &finfo, einfo))
			{
				return 0;
			}
			stackChange = getFieldSize (finfo.field);
			break;
		case STACK_PUTSTATIC:
			if (!getField(WORD(pc+1), meth->class, true, &finfo, einfo))
			{
				return 0;
			}
			stackChange = -getFieldSize (finfo.field);
			break;
		case STACK_GETFIELD:
			if (!getField(WORD(pc+1), meth->class, false, &finfo, einfo))
			{
				return 0;
			}
			stackChange = getFieldSize (finfo.field) - 1;
			break;
		case STACK_PUTFIELD:
			if (!getField(WORD(pc+1), meth->class, false, &finfo, einfo))
			{
				return 0;
			}
			stackChange = -(getFieldSize (finfo.field) + 1);
			break;
		case STACK_INVOKE_VIRTUAL:
			if (!getMethodSignatureClass(WORD(pc+1), meth->class, true, 0, &call, einfo) ||
				!call.method)
			{
				return 0;
			}
			stackChange = getInvokeStackChange (&call, -1);
			break;
		case STACK_INVOKE_INTERFACE:
			if (!getMethodSignatureClass(WORD(pc+1), meth->class, true, 2, &call, einfo) ||
				!call.method)
			{
				return 0;
			}
			stackChange = getInvokeStackChange (&call, -1);
			break;
		case STACK_INVOKE_SPECIAL:
			if (!getMethodSignatureClass(WORD(pc+1), meth->class, true, 1, &call, einfo) ||
				!call.method)
			{
				return 0;
			}
			stackChange = getInvokeStackChange (&call, -1);
			break;
		case STACK_INVOKE_STATIC:
			if (!getMethodSignatureClass(WORD(pc+1), meth->class, true, 0, &call, einfo) ||
				!call.method)
			{
				return 0;
			}
			stackChange = getInvokeStackChange (&call, 0);
			break;
		case STACK_MULTIANEWARRAY:
			stackChange = 1 - meth->c.bcode.code[pc + 3];
			break;
		default:
			fprintf (stderr, "UNHANDLED CASE %s:%u\n", __FILE__, __LINE__);
			abort ();
			stackChange = 0; // Prevent warning
		}

		return stackChange;
	}
	else
	{
		return ins->stack;
	}
}

/**********************************************************************************************************************/

void analyzeBlockStores (errorInfo *einfo, Method *meth, uint pc_start, uint pc_end, StorageRecord *store)
{
	uintp 				pc;
	fieldInfo 			finfo;
	Hjava_lang_Class 	*lastNewType = NULL;

	// Current stack depth
	int 		stack = 0;
	/* PCs of instructions that modify the stack at the specified stack depth (the index).
	 * Only indices below the current depth are valid.
	 */
	uint		*stack_pcs = (uint *) alloca (meth->stacksz * sizeof (uint));
	memset (stack_pcs, 0xFF, meth->stacksz * sizeof (uint));

//printf ("BLOCK: %s.%s\n", meth->class->name->data, meth->name->data);

	for (pc = pc_start; pc <= pc_end;)
	{
		const uint8 *ptr = meth->c.bcode.code + pc;

		const Opcode *ins = &OPCODES[*ptr];
		if (!stack && IS_INSTRUCTION (ins, INS_NEW))
		{
			lastNewType = getClass (WORD(pc+1), meth->class, einfo);
		}

		// TODO: the flags are a bitset, but here they're used as mutually exclusive. Sort this out.
		if (ins->flags & INS_STORE_LOCAL)
		{
			/* Store into local variable */
			uint index;

			switch (ins->flags & INS_STORE_LOCAL)
			{
			case INS_STORE_LOCAL_0:		index = 0;		break;
			case INS_STORE_LOCAL_1:		index = 1;		break;
			case INS_STORE_LOCAL_2:		index = 2;		break;
			case INS_STORE_LOCAL_3:		index = 3;		break;
			case INS_STORE_LOCAL_NDX: 	index = ptr[1]; break;
			default: index = -1; abort (); // prevent compiler warning
			}

			if (index < sizeof (store->quick_locals) * 8)
			{
				store->quick_locals |= 1 << index;
			}
			else
			{
				/* Add storage entry for local with higher index */
				storageRecordAddEntry (store, StorageEntryNewLocal (index));
			}
		}
		else if (ins->flags & INS_STORE_STATIC)
		{
			if (!getField(WORD(pc+1), meth->class, true, &finfo, einfo))
			{
				return;
			}
			storageRecordAddEntry (store, StorageEntryNewFieldStatic (finfo.field));
		}
		else if (ins->flags & INS_STORE_FIELD)
		{
			if (!getField(WORD(pc+1), meth->class, false, &finfo, einfo))
			{
				return;
			}

			/* Try to find the origin of the this pointer
			 * The value is top of stack, the this pointer is below that.
			 */
			bool failed = true;
			const Opcode *thisInstr = NULL;

			int thisIndex = stack;
			int thisField = 0;
			int thisPc   = -1;

			while (failed && thisIndex >= 2)
			{
				thisInstr = &OPCODES[meth->c.bcode.code[stack_pcs[thisIndex - 2]]];
				if (thisInstr->flags & INS_ALOAD)
				{
					failed = false;

					uint thisPcLocal = stack_pcs[thisIndex - 2];
					if (thisInstr->flags & INS_ALOAD_NDX)
					{
						thisField = meth->c.bcode.code[thisPcLocal + 1];
					}
					else
					{
						thisField = meth->c.bcode.code[thisPcLocal] - 0x2a /* aload_0 */;
					}
				}
				else if (thisInstr->flags & INS_ASTORE)
				{
					uint thisPcLocal = stack_pcs[thisIndex - 2];
					if (thisInstr->flags & INS_ASTORE_NDX)
					{
						thisField = meth->c.bcode.code[thisPcLocal + 1];
					}
					else
					{
						thisField = meth->c.bcode.code[thisPcLocal] - 0x4b /* astore_0 */;
					}

					//if (thisPc < 0 || thisPcLocal < thisPc)
					thisPc = thisPcLocal;
					break;
				}
				else if ((thisInstr->flags & INS_NEW) == INS_NEW)
				{
					break;
				}
				else if ((thisInstr->flags & INS_DUP) == INS_DUP)
				{
					thisIndex--;
				}
				else if ((OPCODES[meth->c.bcode.code[stack_pcs[thisIndex - 1]]].flags & INS_DUP_X1) == INS_DUP_X1)
				{
					thisIndex--;
				}
				else
				{
					break;
				}
				thisInstr = NULL;
			}

			if (thisPc < 0)
				thisPc = pc_start;


			if (failed)
			{
				// Not enough of the stack is known, failure
				//printf ("FAIL: %s.%s %s: %s\n", meth->class->name->data, meth->name->data, meth->parsed_sig->signature->data,
				//	thisInstr ? thisInstr->name : "");
				meth->context_failed = true;
			}
			else
			{
				storageRecordAddEntry (store, StorageEntryNewFieldObject (finfo.field, thisField, thisPc));
			}
		}
		else if (ins->flags & INS_STORE_STACK)
		{
			//printf ("STACK WRITE: %s @ %d %d %d\n", ins->name, stack, STORE_QUICK_STACK_LBOUND, STORE_QUICK_STACK_UBOUND);
			storageRecordAddEntry (store, StorageEntryNewStack (stack));
		}
		else if (IS_INSTRUCTION (ins, INS_ATHROW))
		{
			if (lastNewType
#ifdef TRISHUL_EXCEPTION_EXCLUDE_RUNTIME
				 && !isRuntimeException (lastNewType)
#endif
				)
				store->has_throw = true;
		}

		/* Calculate stack adjustment */
		einfo->type = 0;
		int stackChange = getStackChange (einfo, meth, ins, pc);
		if (einfo->type)
			return;
		if (stackChange > 0)
		{
			if (stack >= 0)
			{
				int i;
				for (i = 0; i < stackChange; i++)
				{
					stack_pcs[stack + i] = pc;
				}
			}
		}
		stack += stackChange;

		pc = pc + instruction_length (ptr, meth->c.bcode.code + meth->c.bcode.codelen);
	}
}

/**********************************************************************************************************************/
typedef struct StorageEntryAlloc
{
	int							used;
	struct StorageEntryAlloc	*next;
	StorageEntry				entries[(4096 - sizeof (int) - sizeof (void *)) / sizeof (StorageEntry)];
} StorageEntryAlloc;

static StorageEntryAlloc 	storage_entries_first,
							*storage_entries_last = &storage_entries_first;

static StorageEntry *StorageEntryNew (StorageEntryType type)
{
	/* TODO: release this memory somewhere */
	if (storage_entries_last->used ==
		sizeof (storage_entries_last->entries) / sizeof (storage_entries_last->entries[0]))
	{
		storage_entries_last->next = calloc (sizeof (StorageEntryAlloc), 1);
		RecordAllocStorageRecord (sizeof (StorageEntryAlloc));
		storage_entries_last = storage_entries_last->next;
	}

	StorageEntry *entry = &storage_entries_last->entries[storage_entries_last->used++];
	entry->type = type;
	return entry;
}

/**********************************************************************************************************************/

static StorageEntry *StorageEntryCopy (const StorageEntry *source)
{
	StorageEntry *entry = StorageEntryNew (source->type);
	memcpy (&entry->param, &source->param, sizeof (entry->param));
	return entry;
}

/**********************************************************************************************************************/

StorageEntry *StorageEntryNewFieldStatic (Field *field)
{
	StorageEntry *entry = StorageEntryNew (STORE_FIELD_STATIC);
	entry->param.field_static.field = field;
	return entry;
}

/**********************************************************************************************************************/

StorageEntry *StorageEntryNewFieldObject (Field *field, unsigned int index_this, unsigned int pc_this)
{
	StorageEntry *entry = StorageEntryNew (STORE_FIELD_OBJECT);
	entry->param.field_object.field 	 	= field;
	entry->param.field_object.index_this 	= index_this;
	entry->param.field_object.pc_this		= pc_this;
	return entry;
}

/**********************************************************************************************************************/

StorageEntry *StorageEntryNewStack (int depth)
{
	StorageEntry *entry = StorageEntryNew (STORE_STACK);
	entry->param.stack.depth = depth;
	return entry;
}

/**********************************************************************************************************************/

StorageEntry *StorageEntryNewLocal (int index)
{
	StorageEntry *entry = StorageEntryNew (STORE_LOCAL_VARIABLE);
	entry->param.local.index = index;
	return entry;
}

/**********************************************************************************************************************/

void storageRecordAddEntry (StorageRecord *store, StorageEntry *entry)
{
	if (store->last)
	{
		store->last->next = entry;
	}
	else
	{
		store->first = entry;
	}

	store->last = entry;

}

/**********************************************************************************************************************/

void storageRecordCopyRecord (StorageRecord *store, const StorageRecord *add)
{
	store->quick_locals |= add->quick_locals;
	store->has_throw	 = store->has_throw || add->has_throw;

	/* Copy the elements */
	const StorageEntry *entry;
	for (entry = add->first; entry; entry = entry->next)
	{
		storageRecordAddEntry (store, StorageEntryCopy (entry));
	}
}

/**********************************************************************************************************************/

Hjava_lang_Class *detectExceptionType (errorInfo *einfo, Method *meth, uint pc_start, uint pc_end)
{
	Hjava_lang_Class 	*type = NULL;
	int 				stack = 0;
	uint 				pc;

	for (pc = pc_start; pc <= pc_end;)
	{
		const uint8 *ptr = meth->c.bcode.code + pc;

		const Opcode *ins = &OPCODES[*ptr];
		if (!stack && IS_INSTRUCTION (ins, INS_NEW))
		{
			type = getClass (WORD(pc+1), meth->class, einfo);
		}

		einfo->type = 0;
		int stackChange = getStackChange (einfo, meth, ins, pc);
		if (einfo->type)
			return NULL;
		stack += stackChange;

		pc = pc + instruction_length (ptr, meth->c.bcode.code + meth->c.bcode.codelen);
	}

	return type;
}

/**********************************************************************************************************************/
