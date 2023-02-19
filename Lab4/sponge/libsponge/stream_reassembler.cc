#include "stream_reassembler.hh"

#include <string>
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`


using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {

    if ( eof ) _eofIndex = index + data.size();
    if ( eof ) _eofFlag = true;
    // make some cut of the newIndata 
    string newData = data;
    size_t newIndex = index;
    if ( newData.size() + newIndex <= _wannaIndex ) {
        newData = "";
        newIndex = _wannaIndex;
    }else if ( index < _wannaIndex ) {
        size_t pos = _wannaIndex - index;
        newData = newData.substr( pos, newData.size() - pos );
        newIndex = _wannaIndex;
    }
    /**No matter how big the newData is , the total size is fixed.
     * so whether you put it into the _output,or put it into the _innerMap,
     * it will always occupy the space. 
     * So a easy way to think is to put it into the _innerMap,
     * and then make the satisfying string into the _output.
     * at the same time, make the count and delete.
     */
    put_string_innerMap(newData, newIndex);
    make_transition( );

    // solve the eof 
    if ( empty() && _eofFlag && _wannaIndex >= _eofIndex) _output.end_input(); 
}


size_t StreamReassembler::unassembled_bytes() const { return _unorderedSize; }

bool StreamReassembler::empty() const { return !_unorderedSize; }


void StreamReassembler::put_string_innerMap( const std::string& tar, const size_t& Index ){
    string data = tar;
    size_t index = Index;
    if ( data.size() == 0 ) return;

    /**
     * case 1. when the map has no elements
     * make the cut of the maxLength and put it into the map.
     */
    if ( _innerMap.empty() ) {
        size_t maxLength = _capacity - _output.buffer_size();
        data = data.substr( 0, maxLength );
        _innerMap.insert( {index, data} );
        _unorderedSize += data.size();
        return;
    }

    //case 2. when the map has elements
    /** 
     * (1) judge the segment position in the map.
     * 
     * (2) judge whether the besides element that the iterator points  
     *     has the overlaping parts. if yes, cut the overlaping from
     *     the data, else continue;
     * 
     * (3) judge if besides have the overwritten cases, delete them
     *     from the map.
     * 
     * (4) update the _unorderedSize and put the data into the map.
     *  
     * tips: take care of the iterator invalidation as the elements 
     * in the map has been changed.
     */

    auto iterLow = _innerMap.lower_bound( index );

    if ( iterLow != _innerMap.begin() ){
        auto iter = iterLow; 
        --iter;
        size_t existIndex = iter -> first;
        string existStr = iter -> second;
        if ( existIndex + existStr.size() > index && existIndex < index ){
            size_t cutLen = existIndex + existStr.size() - index;
            cutLen = min( cutLen, data.size() );
            data = data.substr( cutLen, data.size() - cutLen );
            index = index + cutLen;
        }
    }

    auto iter = _innerMap.lower_bound( index );
    
    while ( iter != _innerMap.end() && index <= iter -> first ){
        if ( index + data.size() > iter -> first ) {
            if ( index + data.size() > iter -> first + iter -> second.size() ){
                _unorderedSize -= iter -> second.size();
                iter = _innerMap.erase( iter );
                continue;
            }else {
                size_t cutLen = index + data.size() - iter -> first;
                cutLen = min( cutLen, data.size() );
                data = data.substr( 0, data.size() - cutLen );
                break;
            }

        }
        else break;
    }
    size_t firstUnacceptableIndex = _capacity - _output.buffer_size() + _wannaIndex;

    if ( index >= firstUnacceptableIndex ) return;

    if ( data.size() > 0 ) {
        _innerMap.insert({ index, data });
        _unorderedSize += data.size();
    }

    /**
     * if over the capacity, just throw away the bytes at the end.
     */
    iter = _innerMap.end(); --iter;
    while ( _innerMap.size() > 0 ){
        if ( _capacity >= _unorderedSize + _output.buffer_size() )  break;
        if ( _unorderedSize + _output.buffer_size() - iter->second.size() <= _capacity ){
            size_t resLen = _capacity - _unorderedSize - _output.buffer_size() + iter->second.size();
            string newData = iter->second.substr( 0, resLen );
            _unorderedSize -= iter->second.size() - resLen;
            _innerMap[ iter->first ] = newData;
            break;
        }
        else {
            _unorderedSize -= iter->second.size();
            iter = _innerMap.erase( iter );
            iter--;
        }
    }
}



void StreamReassembler::make_transition(){
    for ( auto iter = _innerMap.begin(); iter != _innerMap.end();  ){
        string existStr = iter->second;
        size_t existIndex = iter->first;
        if ( existIndex == _wannaIndex ){
            _output.write( existStr );
            _wannaIndex +=  existStr.size();
            iter = _innerMap.erase( iter );
            _unorderedSize -= existStr.size();
        }else break;
    }
}




// the thought in the 11.14
    // if ( newIndex + newData.size() > maxIndex ){
    //     size_t length = maxIndex - newIndex;
    //     newData = newData.substr( 0, length );
    // }
    // if ( _innerMap.size() == 0 ) {
    //     _innerMap.insert({ newIndex, newData });
    //     return;
    // }
    // auto iter1 = _innerMap.lower_bound( newIndex );
    // if ( iter1 != _innerMap.begin() ){
    //     /**
    //      * it means the map has the item whose tail is begin the newdata.
    //      */
    //     auto iter = --iter1;
    //     if ( iter -> first < newIndex && iter->first + (iter->second).size() > newIndex ){
    //         size_t pos = (iter->second).size() + (iter->first) - newIndex;
    //         newIndex = iter->first + (iter->second).size();
    //         newData = newData.substr( pos, newData.size() - pos );
    //     }
    // }
    // auto iter2 = --(_innerMap.lower_bound( newIndex + newData.size() ));
    // if ( iter2 -> first < newIndex + newData.size() && iter2 -> first + (iter2 -> second).size() > newIndex + newData.size() ) {
    //     size_t pos = newIndex + newData.size() - (iter2 -> first);
    //     newData = newData.substr( 0, newData.size() - pos );
    // }else ++iter2;
    // --iter2;
    // if ( distance(iter2,iter1) <= 0 ) _innerMap.erase( iter1, iter2 );
    // _innerMap.insert({ newIndex, newData });


    // /**
    //  * Aim: make the value of the buffer into the _output
    //  */
    // auto iter = _innerMap.begin();
    // while ( iter != _innerMap.end() ){
    //     if ( iter -> first == _wannaIndex ) {
    //         _output.write( iter->second );
    //         _wannaIndex += (iter->second).size();
    //         _innerMap.erase(iter);
    //         iter = _innerMap.begin();
    //     }else break;
    // }
    // if ( newIndex > _eofIndex ) _output.end_input();
