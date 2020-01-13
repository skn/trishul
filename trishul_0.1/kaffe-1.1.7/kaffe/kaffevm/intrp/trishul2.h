#ifndef __intrp_trishul_h__
#define __intrp_trishul_h__

typedef struct NATIVETAINTINFO
{
	slots	*args;
	slots	*retval;
} NativeTaintInfo_t;

extern NativeTaintInfo_t *currentTaintInfo;

#define nativeRetValSetTaint(_retType,_taint)	(currentTaintInfo->retval->taint = (_taint))
#define nativeRetValSubTaint(_retType,_taint)	(taintSub1 (currentTaintInfo->retval->taint, (_taint)))
#define nativeRetValGetTaint(_retType)			(currentTaintInfo->retval->taint)
#define nativeParamGetTaint(ndx) 				(currentTaintInfo->args[ndx].taint)
#define nativeContextGetTaint()					(THREAD_DATA()->contextTaintStack)


void trishulSetFallbackContext (taint_t context);
void trishulAddFallbackContext (taint_t context);
taint_t trishulGetFallbackContext ();

#define trishul_before_invoke_method(method)
#define trishul_after_invoke_method(method)

#endif /* __intrp_trishul_h__ */
