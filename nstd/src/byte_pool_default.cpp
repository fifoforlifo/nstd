#include "nstd/byte_pool_default.hpp"

namespace nstd {

    byte_pool_global g_byte_pool_global;

    byte_pool& get_default_byte_pool()
    {
        return g_byte_pool_global;
    }

} // namespace nstd
