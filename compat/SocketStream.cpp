#include "SocketStream.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

static const char* TAG = "SocketStream";

SocketStream::SocketStream() : _sockfd(-1), _peeked(-1) {}

SocketStream::~SocketStream()
{
    disconnect();
}

bool SocketStream::connect(const char* host, uint16_t port)
{
    struct addrinfo hints{};
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%u", (unsigned) port);

    struct addrinfo* res = nullptr;
    int              err = getaddrinfo(host, port_str, &hints, &res);
    if (err != 0 || res == nullptr)
    {
        ESP_LOGE(TAG, "getaddrinfo('%s') failed: %d", host, err);
        return false;
    }

    _sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (_sockfd < 0)
    {
        ESP_LOGE(TAG, "socket() failed: errno %d", errno);
        freeaddrinfo(res);
        return false;
    }

    // Disable Nagle — WiThrottle sends many small commands
    int tcp_nodelay = 1;
    setsockopt(_sockfd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof(tcp_nodelay));

    if (::connect(_sockfd, res->ai_addr, res->ai_addrlen) != 0)
    {
        ESP_LOGE(TAG, "connect() to %s:%u failed: errno %d", host, (unsigned) port, errno);
        ::close(_sockfd);
        _sockfd = -1;
        freeaddrinfo(res);
        return false;
    }
    freeaddrinfo(res);

    // Switch to non-blocking after the connection is established
    int flags = fcntl(_sockfd, F_GETFL, 0);
    fcntl(_sockfd, F_SETFL, flags | O_NONBLOCK);

    ESP_LOGI(TAG, "Connected to %s:%u (fd=%d)", host, (unsigned) port, _sockfd);
    return true;
}

void SocketStream::disconnect()
{
    if (_sockfd >= 0)
    {
        shutdown(_sockfd, SHUT_RDWR);
        ::close(_sockfd);
        _sockfd = -1;
    }
    _peeked = -1;
}

bool SocketStream::connected() const
{
    return _sockfd >= 0;
}

int SocketStream::available()
{
    if (_sockfd < 0)
        return 0;
    if (_peeked >= 0)
        return 1;

    int count = 0;
    if (ioctl(_sockfd, FIONREAD, &count) < 0)
    {
        // ioctl failure usually means the socket is broken
        disconnect();
        return 0;
    }

    if (count == 0)
    {
        // Probe for graceful remote close (recv returns 0) without consuming data
        uint8_t probe;
        int     n = recv(_sockfd, &probe, 1, MSG_PEEK | MSG_DONTWAIT);
        if (n == 0)
        {
            ESP_LOGI(TAG, "Remote end closed connection");
            disconnect();
        }
        // n == -1 with EAGAIN/EWOULDBLOCK is normal (no data yet)
    }
    return count;
}

int SocketStream::read()
{
    if (_sockfd < 0)
        return -1;

    if (_peeked >= 0)
    {
        int b   = _peeked;
        _peeked = -1;
        return b;
    }

    uint8_t b;
    int     n = recv(_sockfd, &b, 1, 0);
    if (n == 1)
        return (int) b;
    if (n == 0)
    {
        disconnect();
        return -1;
    } // graceful close
    // EAGAIN / EWOULDBLOCK — no data despite available() saying there was some
    return -1;
}

int SocketStream::peek()
{
    if (_peeked < 0 && available() > 0)
    {
        uint8_t b;
        int     n = recv(_sockfd, &b, 1, MSG_DONTWAIT);
        if (n == 1)
            _peeked = (int) b;
        else if (n == 0)
            disconnect();
    }
    return _peeked;
}

size_t SocketStream::write(uint8_t c)
{
    return write(&c, 1);
}

size_t SocketStream::write(const uint8_t* buf, size_t len)
{
    if (_sockfd < 0 || len == 0)
        return 0;
    int n = send(_sockfd, buf, len, 0);
    return (n > 0) ? (size_t) n : 0;
}
