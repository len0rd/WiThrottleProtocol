#pragma once
#include "Stream.h"
#include <stdint.h>

/**
 * Arduino-compatible TCP stream backed by a lwIP BSD socket.
 * Passed to WiThrottleProtocol::connect() as the transport.
 */
class SocketStream : public Stream
{
public:
    SocketStream();
    ~SocketStream();

    /** Open a TCP connection to host:port. Returns true on success. */
    bool connect(const char* host, uint16_t port);

    /** Close the socket gracefully. */
    void disconnect();

    /** True while the socket file descriptor is valid. */
    bool connected() const;

    // --- Stream interface ---
    int    available() override;
    int    read() override;
    int    peek() override;
    size_t write(uint8_t c) override;
    size_t write(const uint8_t* buf, size_t len) override;
    void   flush() override {}

private:
    int _sockfd;
    int _peeked; // -1 = no peeked byte
};
