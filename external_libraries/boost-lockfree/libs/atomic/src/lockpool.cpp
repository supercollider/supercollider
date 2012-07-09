#include <boost/atomic.hpp>

namespace boost {
namespace atomics {
namespace detail {

lockpool::lock_type lockpool::pool_[41];

}
}
}
