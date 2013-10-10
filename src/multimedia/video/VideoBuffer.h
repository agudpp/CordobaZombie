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
	 * Return codes enumeration for this class methods.
	 */
	enum Codes{
		ERROR = -1,
	};

	/*
	 * Frame data formats.
	 */
	enum Format{
		RGB = 0, // 1 Byte R, 1 Byte G, 1 Byte Blue
		RGBA     // 1 Byte R, 1 Byte G, 1 Byte Blue, 1 Byte Alpha
	};


//	/*
//	 * Buffer constructor, 'height' x 'width' define the size of the buffer in
//	 * number of bytes, corresponding to 'height' rows and 'width' columns
//	 * in the data matrix.
//	 */
//			VideoBuffer(void);
//
//	/*
//	 * Destroyer.
//	 */
//	virtual
//	~VideoBuffer(void);

	/*
	 * Place data from 'source' into the buffer. 'offset' is the starting point
	 * in the buffer counted as number of bytes. 'size' indicates how much bytes
	 * should we write into the buffer. 'format' identifies the image data
	 * format as it is in 'source'.
	 */
	virtual int
	fillBuffer(unsigned char* source = 0,
				Format format = RGB,
				unsigned int size = 0) = 0;



};

} // namespace mm




#endif //VIDEO_BUFFER_H
