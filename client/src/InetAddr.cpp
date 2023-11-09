#include "../include/InetAddr.h"
#include <strings.h>

InetAddr::InetAddr(const string &ip, unsigned short port)
{
    ::bzero(&_addr, sizeof(struct sockaddr_in));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
    _addr.sin_addr.s_addr = inet_addr(ip.c_str());//host->network
}

InetAddr::InetAddr(const struct sockaddr_in &addr)
: _addr(addr)
{

}

InetAddr::~InetAddr()
{

}

string InetAddr::ip() const
{
    //从网络字节序转换为本机字节序
    return string(inet_ntoa(_addr.sin_addr));
}

unsigned short InetAddr::port() const
{
    //从网络字节序转换为本机字节序
    return ntohs(_addr.sin_port);
}

const struct sockaddr_in *InetAddr::getInetAddrPtr() const
{
    return &_addr;
}


