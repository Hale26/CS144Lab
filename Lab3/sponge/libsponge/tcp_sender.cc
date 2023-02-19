#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>
#include <iostream>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity) {}

uint64_t TCPSender::bytes_in_flight() const {
    return  _outgoing_bytes;
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    auto absoluteACK = unwrap( ackno, _isn, _next_seqno );
    if ( absoluteACK > _next_seqno ) return;
    for ( auto iter = _outstandingMap.begin(); iter != _outstandingMap.end(); ){
        const TCPSegment &seg = iter->second;
        if ( iter->first + seg.length_in_sequence_space() <= absoluteACK ){
            // it means send out correctly.
            _outgoing_bytes -= seg.length_in_sequence_space();
            iter = _outstandingMap.erase( iter );

            _timeCount = 0;
            _timeout = _initial_retransmission_timeout;
        }  else break;
    }
    _retransmission_count = 0;
    _last_windowSize = window_size;
    // _last_windowSize = max( window_size,static_cast<uint16_t>(_outgoing_bytes));
    fill_window();
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    _timeCount += ms_since_last_tick;
    if ( !_outstandingMap.empty() ){
        auto iter = _outstandingMap.begin();
        if ( _timeCount >= _timeout ){
            if ( _last_windowSize > 0 ) _timeout *= 2;
            _timeCount = 0;
            _segments_out.push( iter-> second );
            ++_retransmission_count;
        }
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _retransmission_count; }

void TCPSender::send_empty_segment() {
    TCPSegment segment;
    segment.header().seqno = next_seqno();
    _segments_out.push(segment);
}



void TCPSender::fill_window() {
    // calculate right now the windowsize.
    size_t currentWindowSize = _last_windowSize ? _last_windowSize : 1;
    // if ( _outgoing_bytes > currentWindowSize ) return;
    if ( currentWindowSize < _outgoing_bytes ) return;
    currentWindowSize -= _outgoing_bytes;
 
    TCPSegment newSeg;
    if ( !_synFlagSet ){
        _synFlagSet = true;
        newSeg.header().syn = true;
    }

    if ( _outstandingMap.empty() ) {
        _timeout = _initial_retransmission_timeout;
        _timeCount = 0;
    }

    while ( currentWindowSize > 0 ){
        size_t size = min( currentWindowSize, TCPConfig::MAX_PAYLOAD_SIZE );
        size -= newSeg.length_in_sequence_space();
        string data = _stream.read( size );
        cout << data.size();

        // it means no data will place into the payload
        if ( data.size() == 0 ) break;
        // set the payload 
        newSeg.payload() = Buffer(move(data));
        // in this case, the segment has not been in the MAX
        if ( newSeg.length_in_sequence_space() != TCPConfig::MAX_PAYLOAD_SIZE 
                && newSeg.length_in_sequence_space() != size ) break;
        if ( newSeg.length_in_sequence_space() == TCPConfig::MAX_PAYLOAD_SIZE 
                && _stream.eof() ) break;
        // put the segment into the _segments_out and the map.
        newSeg.header().seqno = wrap( _next_seqno, _isn );
        _segments_out.push( newSeg );
        _outstandingMap.insert({ _next_seqno, newSeg });
        _outgoing_bytes += newSeg.length_in_sequence_space();
        _next_seqno += newSeg.length_in_sequence_space();
        currentWindowSize -= newSeg.length_in_sequence_space();
        newSeg = TCPSegment();
    }


    if ( _stream.eof() && !_finFlagSet ) {
        if ( currentWindowSize > 0 ){
            _finFlagSet = true;
            newSeg.header().fin = true;
        }
    }

    if ( newSeg.length_in_sequence_space() > 0 ){
        newSeg.header().seqno = wrap( _next_seqno, _isn );
        _segments_out.push( newSeg );
        _outstandingMap.insert( { _next_seqno, newSeg} );
        _outgoing_bytes += newSeg.length_in_sequence_space();
        currentWindowSize -= newSeg.length_in_sequence_space();
        _next_seqno += newSeg.length_in_sequence_space();
    }
    cout << _outgoing_bytes << endl;
 
}

