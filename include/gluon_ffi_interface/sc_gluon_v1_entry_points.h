// SuperCollider foreign function interface.
// Author Jordan Henderson - JordanHendersonMusic 2025
#pragma once
#include "sc_gluon_v1_types.h"

#ifdef __cplusplus
#    define SC_GLUON_EXTERN extern "C"
#endif

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64)
#    define SC_GLUON_EXPORT SC_GLUON_EXTERN __declspec(dllexport)
#else
#    define SC_GLUON_EXPORT SC_GLUON_EXTERN __attribute__((__visibility__("default")))
#endif

// Required
SC_GLUON_EXPORT uint32_t sc_gluon_version();

// Required
SC_GLUON_EXPORT uint8_t sc_gluon_load_library(sc_gluon_param_v1_t* in_params, uint32_t num_in_params,
                                              sc_gluon_do_callback_v1_f do_callback,
                                              sc_gluon_release_callback_object_v1_f release_callback,
                                              sc_gluon_function_declarations_v1_t** const out_decls,
                                              uint32_t* out_decls_size, sc_gluon_library_data_v1_t* out_library_data);


// Optional
SC_GLUON_EXPORT void sc_gluon_post_load_library(sc_gluon_library_data_v1_t,
                                                struct sc_gluon_function_declarations_v1_t* decls_to_be_freed,
                                                uint32_t decls_size);


// Optional
SC_GLUON_EXPORT void sc_gluon_unload_library(sc_gluon_library_data_v1_t);
