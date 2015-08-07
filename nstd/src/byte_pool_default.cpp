#include "nstd/byte_pool_default.hpp"
#include "nstd/tls.hpp"

namespace nstd {

    byte_pool_global g_byte_pool_global;

    NSTD_TLS_DECL byte_pool* p_tls_byte_pool = &g_byte_pool_global;

    byte_pool& get_default_byte_pool()
    {
        return *p_tls_byte_pool;
    }

} // namespace nstd
