#include "../../../../multimedia/video/VideoPlayer.h"
#include "../../../../multimedia/video/VideoBuffer.h"




class OgreIndepVideoTest : public mm::VideoBuffer
{

public:

    enum error{
        EXIT = 0,
        OK
    };

	static const char* VIDEO1_PATH;
	static const char* VIDEO2_PATH;
public:

	/*
	 *
	 */
	OgreIndepVideoTest (void);

	/*
	 *
	 */
	virtual
	~OgreIndepVideoTest (void);

	/*
	 * Place data from 'source' into the buffer. 'offset' is the starting point
	 * in the buffer counted as number of bytes. 'size' indicates how much bytes
	 * should we write into the buffer. 'format' identifies the image data
	 * format as it is in 'source'.
	 */
	int
	fillBuffer(unsigned char* source = 0,
					Format format = RGB,
					unsigned int size = 0);



	/*
	 *
	 */
	void
	load(const char *path);

	/*
	 *
	 */
	void
	saveFrame(int iFrame);


	/*
	 *
	 */
	void
	run(void);

	/*
	 *
	 */
	error
	update(double tslf);

private:

	unsigned int		mHeight;
	unsigned int		mWidth;
	unsigned char 		*mBuffer;
	mm::VideoPlayer		mVP;

};

