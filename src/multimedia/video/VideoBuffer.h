/*
 * VideoBuffer.h
 *
 *  Created on: 5/10/2013
 *  Author: raul
 *
 */


#ifndef VIDEO_BUFFER_H
#define VIDEO_BUFFER_H


namespace mm{

class VideoBuffer
{
public:

	/*
	 * Buffer constructor, 'height' x 'width' define the size of the buffer in
	 * number of bytes, corresponding to 'height' rows and 'width' columns
	 * in the data matrix.
	 */
			VideoBuffer(unsigned int height, unsigned int width);

	/*
	 * Destroyer.
	 */
	virtual ~VideoBuffer(void);

	/*
	 * Place data from 'source' into the buffer. 'offset' is the starting point
	 * in the buffer counted as number of bytes. 'size' indicates how much bytes
	 * should we write into the buffer.
	 */
	int
	fillBuffer(unsigned char* source = 0,
				unsigned int offset = 0,
				unsigned int size = 0);

	/*
	 * Clear all data from buffer and reset it.
	 */
	int		emptyBuffer(void);

	/*
	 * Get read only access to 'size' bytes of data at 'offset'
	 * bytes from the beginning of the buffer.
	 */
	unsigned char *
	read(unsigned int offset, unsigned int &size) const;


	/*
	 * Return codes enumeration for this class methods.
	 */
	enum{
		ERROR = -1,
	}

private:

	unsigned char *mData;   // Byte data
	unsigned int mDataSize; // Last byte written index
	unsigned int mWidth;
	unsigned int mHeight;

};

} // namespace mm




#endif //VIDEO_BUFFER_H
