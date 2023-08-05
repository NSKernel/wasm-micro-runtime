#include "sgx.h"
#include "sgx_defs.h"
#include "sgx_ecp_types.h"
#include "sgx_quote_3.h"
#include "dcap_dh_def.h"

#ifndef _TDCAP_DH_H_
#define _TDCAP_DH_H_

#ifdef __cplusplus
extern "C" {
#endif

/* The order of calling SGX DH Library APIs is restricted as below */
/* As session initiator : Step.1 sgx_dh_dcap_init_session -->  Step.2 sgx_dh_dcap_initiator_proc_msg1 --> Step.3 sgx_dh_dcap_initiator_proc_msg3 */
/* As session responder :  Step.1 sgx_dh_dcap_init_session --> Step.2 sgx_dh_dcap_responder_gen_msg1 --> Step.3 sgx_dh_dcap_responder_proc_msg2*/
/* Any out of order calling will cause session establishment failure. */

/*Function name: sgx_dh_dcap_init_session
** parameter description
**@ [input] role: caller's role in dh session establishment
**@ [output] session: point to dh session structure that is used during establishment, the buffer must be in enclave address space
*/
sgx_status_t  sgx_dh_dcap_init_session(sgx_dh_session_role_t role,
                                        sgx_dh_session_t* session);

/*Function name: sgx_dh_dcap_responder_gen_msg1
** parameter description
**@ [output] msg1: point to dh message 1 buffer
**@ [input/output] dh_session: point to dh session structure that is used during establishment, and the buffer must be in enclave address space
*/
sgx_status_t  sgx_dh_dcap_responder_gen_msg1(sgx_dh_dcap_msg1_t* msg1,
                                              sgx_dh_session_t* dh_session);
/*Function name: sgx_dh_dcap_initiator_proc_msg1
** parameter description
**@ [input] msg1: point to dh message 1 buffer generated by session responder, and the buffer must be in enclave address space
**@ [output] msg2: point to dh message 2 buffer, and the buffer must be in enclave address space
**@ [input/output] dh_session: point to dh session structure that is used during establishment, and the buffer must be in enclave address space
*/
sgx_status_t  sgx_dh_dcap_initiator_proc_msg1(const sgx_dh_dcap_msg1_t* msg1,
                                               sgx_dh_dcap_msg2_t* msg2,
                                               sgx_dh_session_t* dh_session);

/*Function name: sgx_dh_dcap_responder_proc_msg2
** parameter description
**@ [input] msg2: point to dh message 2 buffer generated by session initiator, and the buffer must be in enclave address space
**@ [output] msg3: point to dh message 3 buffer generated by session responder in this function, and the buffer must be in enclave address space
**@ [input/output] dh_session: point to dh session structure that is used during establishment, and the buffer must be in enclave address space
**@ [output] aek: AEK derived from shared key. the buffer must be in enclave address space.
**@ [output] initiator_identity: identity information of initiator including isv svn, isv product id, sgx attributes, mr signer, and mr enclave. the buffer must be in enclave address space.
*/
sgx_status_t  sgx_dh_dcap_responder_proc_msg2(const sgx_dh_dcap_msg2_t* msg2,
                                               sgx_dh_dcap_msg3_t* msg3,
                                               sgx_dh_session_t* dh_session,
                                               sgx_key_128bit_t* aek);
/*Function name: sgx_dh_dcap_initiator_proc_msg3
** parameter description
**@ [input] msg3: point to dh message 3 buffer generated by session responder, and the buffer must be in enclave address space
**@ [input/output] dh_session: point to dh session structure that is used during establishment, and the buffer must be in enclave address space
**@ [output] aek: AEK derived from shared key. the buffer must be in enclave address space.
**@ [output] responder_identity: identity information of responder including isv svn, isv product id, sgx attributes, mr signer, and mr enclave. the buffer must be in enclave address space.
*/
sgx_status_t  sgx_dh_dcap_initiator_proc_msg3(const sgx_dh_dcap_msg3_t* msg3,
                                               sgx_dh_session_t* dh_session,
                                               sgx_key_128bit_t* aek,
                                               sgx_dh_session_enclave_identity_t* responder_identity);

#ifdef __cplusplus
}
#endif

#endif
