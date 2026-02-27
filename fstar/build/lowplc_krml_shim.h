/**
 * lowplc_krml_shim.h — Minimal KaRaMeL runtime shims for LowPLC
 *
 * Provides the fst/snd tuple accessors and KRML_HOST macros that
 * KaRaMeL-extracted C code references. For bare-metal targets,
 * KRML_HOST_EXIT is a fault loop.
 */
#ifndef LOWPLC_KRML_SHIM_H
#define LOWPLC_KRML_SHIM_H

#include <stdint.h>
#include <stdbool.h>

/* Tuple accessors — KaRaMeL generates these for fst/snd on F* tuples.
   They are simple struct field accesses, inlined by the compiler. */
#define FStar_Pervasives_Native_fst__LowPLC_TON_ton_state_LowPLC_TON_ton_outputs(x) ((x).fst)
#define FStar_Pervasives_Native_snd__LowPLC_TON_ton_state_LowPLC_TON_ton_outputs(x) ((x).snd)

/* KRML_HOST macros — used in unreachable default branches */
#ifndef KRML_HOST_EPRINTF
  #define KRML_HOST_EPRINTF(...) ((void)0)
#endif

#ifndef KRML_HOST_EXIT
  #ifdef __arm__
    #define KRML_HOST_EXIT(x) do { for(;;); } while(0)
  #else
    #include <stdlib.h>
    #define KRML_HOST_EXIT(x) exit(x)
  #endif
#endif

#endif /* LOWPLC_KRML_SHIM_H */
