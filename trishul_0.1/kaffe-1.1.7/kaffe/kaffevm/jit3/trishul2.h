#ifndef __jit3_trishul_h__
#define __jit3_trishul_h__

void nativeRetValSetTaint (char retType, taint_t taint);
void nativeRetValSubTaint (char retType, taint_t taint);
taint_t nativeRetValGetTaint (char retType);
taint_t nativeParamGetTaint (uint ndx);
taint_t nativeContextGetTaint ();

void trishulSetFallbackContext (taint_t context);
void trishulAddFallbackContext (taint_t context);
taint_t trishulGetFallbackContext ();

void trishul_before_invoke_method (Method *method);
void trishul_after_invoke_method (Method *method);

#endif /* __jit3_trishul_h__ */
