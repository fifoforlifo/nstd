#pragma once

#if _MSC_VER
#define NSTD_TLS_DECL __declspec(thread)
#elif __GNUC__
#define NSTD_TLS_DECL __thread
#endif
