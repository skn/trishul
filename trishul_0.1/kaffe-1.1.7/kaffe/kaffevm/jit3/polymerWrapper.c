
static POLYMER_WRAPPER_TYPE POLYMER_WRAPPER_NAME (POLYMER_WRAPPER_TYPE (*func) (), const Method *method,
													uint argSize, UNUSED void *temp1, void *firstArg)
{
	asm ("push %edx ; push %ecx"); // GCC saves ebp, edi and esi. EAX is for return value

	jvalue					retval; retval.taint = 0;
	const PolymerOrder 		*order;
	Hjava_lang_Object 		*x = NULL; // TODO

	void **argPtr = &firstArg;
	void *esp;

	if (polymerEngineBefore (method, argPtr, &retval, &order))
	{
		// Invoke normal method
		// TODO: copying these arguments can be skipped if polymer data is stored above them on the stack

		asm (
			"subl %1, %%esp; movl %%esp, %0"
			: "=m" (esp) : "m" (argSize)
		);

		memcpy (esp, argPtr, argSize);

#if TRAP_POLYMER_CALL
		asm ("int3");
#endif
		POLYMER_WRAPPER_INVOKE

		asm (
			"addl %0, %%esp"
			:  : "m" (argSize)
		);
	}

	if (order)
	{
		polymerEngineAfter (method, &retval, x, order);
	}

	asm ("pop %ecx ; pop %edx");

	POLYMER_WRAPPER_RETURN
}

