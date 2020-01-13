#ifndef __trishul_p_h
#define	__trishul_p_h
/**********************************************************************************************************************/
#include "classMethod.h"
/**********************************************************************************************************************/
#ifdef JIT3
	typedef void ValueType;
#else
	typedef jvalue ValueType;
#endif
/**********************************************************************************************************************/

extern uint polymerIteration;

#define POLYMER_ENGINE_USE_SECURITY_MANAGER
//#define POLYMER_ENGINE_USE_ACCESS_CONTROL_CONTEXT
//#define POLYMER_ENGINE_USE_PROTECTION_DOMAIN

#if !defined (POLYMER_ENGINE_USE_SECURITY_MANAGER) 			&& \
	!defined (POLYMER_ENGINE_USE_ACCESS_CONTROL_CONTEXT) 	&& \
	!defined (POLYMER_ENGINE_USE_PROTECTION_DOMAIN)
	#error No engine-engine engine defined
#endif

/**
 * Invoked when a engine is initialized.
 * @param engine The engine. This need not be a trishul_p engine, as the caller does not know how to detect such
 *				 a engine. If it is not a trishul_p engine, no action is taken.
 */
Hjava_lang_Object *polymerInitializeEngineObject (Hjava_lang_Object *initiator,
												  Hjava_lang_Object *engine, Hjava_lang_Object *enginePolicyURL);

Hjava_lang_Object *loadEnginePolicyFromURL (const char *url);

/**
 * Checks if the supplied method matches any of the installed filters.
 * If so, it's polymerState field is updated.
 */
bool polymerCheckFilters (Method *method);

/**
 * Handle type for trishul_p.Order.
 */
typedef struct {} PolymerOrder;

/**
 * Invoked by the engine before it is about to invoke a method. As an optimization, this should only be done
 * for methods that match the filters; this can be tested using polymerCheckFilters.
 *
 * @param order receives the order returned by the engine. If this is not null, polymerEngineAfter must
 *        be invoked after the original method returns.
 * @return True if the method should be invoked.
 */
bool polymerEngineBefore (const Method *method,
						  ValueType *arguments, jvalue *retval,
						  const PolymerOrder **order);

/**
 * Invoked by the engine after it has invoked a method for which polymerEngineBefore returned an order.
 */
void polymerEngineAfter (const Method *method, jvalue *retval,
						 Hjava_lang_Object *x, const PolymerOrder *order);

/**
 * Invoked when a program is terminating, to allow trishul_p to handle clean-up.
 */
void polymerTerminating	();

/**********************************************************************************************************************/
#endif /* __trishul_p_h */
