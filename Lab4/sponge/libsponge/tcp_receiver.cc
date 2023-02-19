#include "tcp_receiver.hh"
#include <iostream>

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.


using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
	string msg = (seg.header()).to_string();
	string data = (seg.payload()).copy(); 

	size_t posStartFlags = msg.find("urg"), posEndFlags = msg.find("TCP winsize");
	string msgFlags = msg.substr( posStartFlags, posEndFlags - posStartFlags );
	string msgSeqno = msg.substr( msg.find("seqno:") + 7, msg.find("\nTCP ackno:") - msg.find("seqno:") - 7 );
	string msgAckno = msg.substr( msg.find("ackno:") + 7, msg.find("\nTCP doff:") - msg.find("ackno:") - 7 );

	// calc the value of the flags.
	bool synFlag = ( msgFlags.substr( msgFlags.find("syn:") + 5, 1 ) == "t" );
	// bool ackFlag = ( msgFlags.substr( msgFlags.find("ack:") + 5, 1 ) == "t" );
	bool finFlag = ( msgFlags.substr( msgFlags.find("fin:") + 5, 1 ) == "t" );

	if ( !_synFlag && !synFlag ) return;
	uint32_t seqno = _to_uint32( msgSeqno );
	if ( !_synFlag && synFlag ){
		_isn = WrappingInt32( seqno );
		_synFlag = true;
		_checkPoint = 0;
	}
	_checkPoint = unwrap( WrappingInt32(seqno), _isn, _checkPoint );

	uint64_t currenIndex = _checkPoint - 1 + synFlag;
	_reassembler.push_substring( data, currenIndex, finFlag );
	
}

uint32_t TCPReceiver::_to_uint32( const string& s ){
	uint32_t rv = 0;
	for ( size_t i = 0; i < s.size(); ++i ) {
		size_t val = 0;
		if ( s[i] >= '0' && s[i] <= '9' ) val = s[i] - '0';
		if ( s[i] >= 'a' && s[i] <= 'f' ) val = s[i] - 'a' + 10;
		rv = (rv << 4) + val;
	}
	return  rv;
}

optional<WrappingInt32> TCPReceiver::ackno() const { 
	uint64_t rv = _reassembler.stream_out().bytes_written() + 1;
	if ( _synFlag ) {
		if ( _reassembler.stream_out().input_ended() ) rv++;
		return wrap( rv, _isn );
	}
	else return nullopt;
}

size_t TCPReceiver::window_size() const {
	 return _capacity - _reassembler.stream_out().buffer_size();
 }
