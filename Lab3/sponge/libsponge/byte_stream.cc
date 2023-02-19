#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

using namespace std;

ByteStream::ByteStream(const size_t capacity)  { _capacity = capacity; }

size_t ByteStream::write(const string &data) {
    size_t lenReal = ( data.length() > remaining_capacity() ) ? 
                        remaining_capacity() : data.length();
    for ( size_t i = 0; i < lenReal; ++i ) _buffer.push_back( data[i] );
    _byteWritten += lenReal;
    return lenReal;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    size_t lenReal = (len > _buffer.size()) ? _buffer.size() : len;
    string rv;
    rv.assign( _buffer.begin(), _buffer.begin() + lenReal );
    return rv;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t lenReal = (len > _buffer.size()) ? _buffer.size() : len;
    _byteRead += lenReal;
    while ( lenReal-- ) _buffer.pop_front();
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string rv = peek_output( len );
    pop_output( len );
    return rv;
}

void ByteStream::end_input() {
    _endInputFlag = true;
}

bool ByteStream::input_ended() const {
    return _endInputFlag;
}

size_t ByteStream::buffer_size() const {
   return _buffer.size();
}

bool ByteStream::buffer_empty() const {
    return _buffer.size() == 0 ; 
}

bool ByteStream::eof() const {
    return input_ended() && buffer_empty() ; 
}

size_t ByteStream::bytes_written() const {
    return _byteWritten; 
}

size_t ByteStream::bytes_read() const {
    return _byteRead; 
}

size_t ByteStream::remaining_capacity() const {
    return _capacity - _buffer.size(); 
}
