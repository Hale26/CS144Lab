Lab 1 Writeup
=============

My name: [shineHale]


The starting Date: [ 22. 11. 14 ]

The finishing Date: [ 22. 11. 17 ]

This lab took me about [8] hours to do. I [did not] attend the lab session.

Program Structure and Design of the StreamReassembler:
```cpp
	/**
     * case 1. when the map has no elements
     * make the cut of the maxLength and put it into the map.
     */
     

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
```



Implementation Challenges:
At first, I do not understand why there is a question about the overlaping substring.
It blocks my way to the correct answer, while I realize that it is ambiguous,the case is really generated.

Remaining Bugs:
nothing
