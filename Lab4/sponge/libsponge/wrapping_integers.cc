#include "wrapping_integers.hh"
#include <iostream>
// Dummy implementation of a 32-bit wrapping integer

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) {
    const uint64_t MOD = 1ull << 32;
    return WrappingInt32( ( n % MOD + isn.raw_value() ) % MOD ); 
}

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) {
    const uint64_t MOD = 1ull << 32;
    uint64_t valN = 1ull * n.raw_value(), valIsn = 1ull * isn.raw_value();
    uint64_t offset = ( valN >= valIsn ) ? (valN - valIsn) : valN + MOD - valIsn;
    uint64_t val = ( checkpoint / MOD ) * MOD + offset, rv = val;
    if ( abs (int64_t ( val + MOD - checkpoint ) )  < abs( int64_t( val -  checkpoint ) ) ) rv = val + MOD;
    if ( val >= MOD && abs( int64_t( val - MOD - checkpoint ) ) < abs( int64_t( rv - checkpoint ) ) ) rv = val - MOD;
    return rv;
}
