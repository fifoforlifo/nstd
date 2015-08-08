#pragma once

#if defined(_MSC_VER) && _M_IX86
#define NSTD_VECTORCALL __vectorcall
#else
#define NSTD_VECTORCALL
#endif
