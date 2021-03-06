#pragma once

#include "telnetpp/options/subnegotiationless_server.hpp"
#include "telnetpp/options/suppress_ga/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace suppress_ga {

//* =========================================================================
/// \class telnetpp::options::suppress_ga::server
/// \extends telnetpp::server_option
/// \brief An implementation of the server side of the Telnet Suppress Go-
/// Ahead option.
/// \see https://tools.ietf.org/html/rfc858
//* =========================================================================
using server = telnetpp::options::subnegotiationless_server<
    telnetpp::options::suppress_ga::detail::option
>;

}}}
