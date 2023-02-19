Lab 3 Writeup
=============

My name: [shine_hale]

This lab took me about [7] hours to do. I [did not] attend the lab session.

Program Structure and Design of the TCPSender:
```c++
  private:
    //! our initial sequence number, the number for our SYN.
    WrappingInt32 _isn;

    //! outbound queue of segments that the TCPSender wants sent
    std::queue<TCPSegment> _segments_out{};

    //! retransmission timer for the connection
    unsigned int _initial_retransmission_timeout;

    //! outgoing stream of bytes that have not yet been sent
    ByteStream _stream;

    //! the (absolute) sequence number for the next byte to be sent
    uint64_t _next_seqno{0};


    //addtional definition of the elements in the TCPsender.
    uint16_t _last_windowSize{1};
    uint64_t _outgoing_bytes{0};
    int _timeCount{0};
    int _timeout{0};
    int _retransmission_count{0};

    std::map<uint64_t,TCPSegment> _outstandingMap{};

    bool _synFlagSet{false};
    bool _finFlagSet{false};
```

Implementation Challenges:

Remaining Bugs:
I have find that the test file has the bug, when the TCPSegment was filled with
the data, and it reaches the MAX_CAPACITY. while it reaches the EOF of the Byte
stream ,the fin flag will be set at the same time but it should not be that, for
it will expire the capacity. and the test file will think it is right, but not.
