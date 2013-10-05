/*
 * VideoBuffer.cpp
 *
 *  Created on: 5/10/2013
 *  Author: raul
 *
 */

/*
 * VideoBuffer.h
 *
 *  Created on: 5/10/2013
 *  Author: raul
 *
 */

#include <malloc.h>

#include <DebugUtil.h>

#include "VideoBuffer.h"



namespace mm{


//-----------------------------------------------------------------------------

VideoBuffer::VideoBuffer(unsigned int height, unsigned int width) :
	mData(0),
	mDataSize(0),
	mWidth(0),
	mHeight(0)
{
	mWidth = width;
	mHeight = height;

	// Allocate data matrix
	mData = (unsigned char*) malloc (sizeof(unsigned char*) * height * width);

	ASSERT( mData != 0 );

	mDataSize = 0;
}


//-----------------------------------------------------------------------------

virtual
VideoBuffer::~VideoBuffer(void)
{
	// Only need to free the allocated memory 'mData'.

	// If mHeight and mWidth are not zero then we should have allocated
	// memory in mData.
	ASSERT(mHeight != 0 || mWidth != 0 || mData == 0);

	if(mHeight != 0){
		if(mWidth != 0 ){
			ASSERT(mData != 0);
			free(mData);
			mData = 0;
			}
		}
}


//-----------------------------------------------------------------------------
int
VideoBuffer::fillBuffer(unsigned char* source = 0,
			unsigned int offset = 0,
			unsigned int size = 0){

	// How many bytes to copy
	int numOfBytes = mHeight*mWidth - offset;
	if(numOfBytes < 0){
		numOfBytes = ERROR;
	}else if(size < numOfBytes){
		numOfBytes = size;
	}

	// If no error in input then write.
	if(numOfBytes != ERROR){
		memcpy(mData+offset, source, numOfBytes);
		mDataSize = offset+numOfBytes;
	}

	// Return number of copied bytes
	return numOfBytes;
}

//-----------------------------------------------------------------------------
int
VideoBuffer::emptyBuffer(void){
	// Clear data in mData;
	memset(mData,0,mHeight*mWidth);
	mDataSize = 0;
	return 0;
}

//-----------------------------------------------------------------------------
unsigned char*
VideoBuffer::read(unsigned int offset, unsigned int &size) const {

	// Number of bytes allowed to be read
	int numOfBytes = mHeight*mWidth - offset;

	if(numOfBytes < 0){
		size = ERROR;
	}else if(numOfBytes < size){
		size = numOfBytes;
	}

	if(size != ERROR){
		return mData+offset;
	}else{
		return 0;
	}
}



} // namespace mm
