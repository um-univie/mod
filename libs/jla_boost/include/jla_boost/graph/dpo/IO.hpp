#ifndef JLA_BOOST_GRAPH_DPO_IO_HPP
#define	JLA_BOOST_GRAPH_DPO_IO_HPP

#include <jla_boost/graph/dpo/Rule.hpp>

#include <iosfwd>

namespace jla_boost {
namespace GraphDPO {

std::ostream &operator<<(std::ostream &s, Membership m);

} // namespace GraphDPO
} // namespace jla_boost

#endif	/* JLA_BOOST_GRAPH_DPO_IO_HPP */

