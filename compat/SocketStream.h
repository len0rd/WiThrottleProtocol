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

    /**
     * Flush the TX buffer: send all buffered bytes in a single send() call.
     * Called automatically when a newline is buffered or the buffer is full.
     */
    void flush() override;

private:
    int _sockfd;
    int _peeked; // -1 = no peeked byte

    // TX coalescing buffer — accumulates small writes and flushes on '\n'
    // or when full, to avoid one send() per character/fragment.
    static constexpr size_t TX_BUF_SIZE = 512;
    uint8_t                 _txbuf[TX_BUF_SIZE];
    size_t                  _txlen;
};
