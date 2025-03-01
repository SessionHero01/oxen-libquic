#include "ip.hpp"

#include "internal.hpp"

namespace oxen::quic
{
    ipv4::ipv4(const std::string& str)
    {
        in_addr sin;
        parse_addr(sin, str);
        addr = oxenc::big_to_host(sin.s_addr);
    }

    std::string ipv4::to_string() const
    {
        char buf[INET_ADDRSTRLEN] = {};
        uint32_t net = oxenc::host_to_big(addr);
        inet_ntop(AF_INET, &net, buf, sizeof(buf));

        return "{}"_format(buf);
    }

    std::string ipv4_net::to_string() const
    {
        return "{}/{}"_format(base.to_string(), mask);
    }

    in6_addr ipv6::to_in6() const
    {
        in6_addr ret;

        oxenc::write_host_as_big(hi, &ret.s6_addr[0]);
        oxenc::write_host_as_big(lo, &ret.s6_addr[8]);

        return ret;
    }

    ipv6::ipv6(const std::string& str)
    {
        in6_addr sin6;
        parse_addr(sin6, str);

        hi = oxenc::load_big_to_host<uint64_t>(&sin6.s6_addr[0]);
        lo = oxenc::load_big_to_host<uint64_t>(&sin6.s6_addr[8]);
    }

    std::string ipv6::to_string() const
    {
        char buf[INET6_ADDRSTRLEN] = {};

        std::array<uint8_t, 16> addr;

        oxenc::write_host_as_big(hi, &addr[0]);
        oxenc::write_host_as_big(lo, &addr[8]);

        inet_ntop(AF_INET6, &addr, buf, sizeof(buf));

        return "{}"_format(buf);
    }

    const std::string ipv6_net::to_string() const
    {
        return "{}/{}"_format(base.to_string(), mask);
    }
}  //  namespace oxen::quic
