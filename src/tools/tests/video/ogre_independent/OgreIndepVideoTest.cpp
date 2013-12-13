#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include "OgreIndepVideoTest.h"


const char* OgreIndepVideoTest::VIDEO1_PATH =
    "/home/raul/Dropbox/cordobazombie0.1/game/videos/5seg2.ogg";
	//"/home/raul/Downloads/LeapYear2010720pBluRayx264-HUBRIS/ly2010-hubris.mkv";
const char* OgreIndepVideoTest::VIDEO2_PATH =
    "/home/raul/Dropbox/cordobazombie0.1/game/videos/5seg2.ogg";


OgreIndepVideoTest::OgreIndepVideoTest(void):
	mBuffer(0),
	mVP(this),
	mHeight(0),
	mWidth(0)
{
	return;
}

void
OgreIndepVideoTest::load(const char *path)
{
	mVP.load(path);
	mVP.getSizes(mHeight,mWidth);
	mBuffer = (unsigned char*) realloc( mBuffer,
	                   sizeof(unsigned char)*mHeight*mWidth*3);
	return;
}



OgreIndepVideoTest::~OgreIndepVideoTest(void)
{
    free(mBuffer);
}


int
OgreIndepVideoTest::fillBuffer(	unsigned char* source,
								Format format,
								unsigned int size)
{
	assert(sizeof(unsigned char)*mHeight*mWidth*3 >= size);
	memcpy(mBuffer, source, size);
}


void
OgreIndepVideoTest::run(void)
{


    load(VIDEO1_PATH);
    mVP.play();
    time_t start = time(0);
    time_t end = time(0);
    double tslf = 0.;
    while(error::OK == update(tslf)){
        end = time(0);
        tslf = difftime(end, start);
        start = end;
    }

    mVP.unload();
    load(VIDEO2_PATH);
    mVP.play();
    start = time(0);
    end = time(0);
    tslf = 0.;
    while(error::OK == update(tslf)){
        double t = 0.;
        mVP.get_playing_time_in_secs(t);
        if( t >= 1.){
            mVP.seek_time_stamp(2.);
        }
        end = time(0);
        tslf = difftime(end, start);
        //std::cout << tslf << std::endl;
        start = end;
    }

    mVP.unload();

    return;
}

OgreIndepVideoTest::error
OgreIndepVideoTest::update(double tslf)
{
    static int frame = 0;

    if(mVP.update(tslf)==mm::VideoPlayer::VIDEO_OK){
        if(frame < 105){
            frame++;
            if(frame > 100){
                saveFrame(frame);
            }
        }
        return OK;
    }else{
        return EXIT;
    }
}


void OgreIndepVideoTest::saveFrame(int iFrame)
{
    FILE *pFile;
    char szFilename[32];
    int  y;

    // Open file
    sprintf(szFilename, "./Frames/frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;

    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", mWidth, mHeight);

    // Write pixel data
    for(y=0; y<mHeight; y++)
        fwrite(mBuffer+y*mWidth*3, 1, mWidth*3, pFile);

    // Close file
    fclose(pFile);
}


int
main(void)
{

    OgreIndepVideoTest test = OgreIndepVideoTest();
    test.run();

    return 0;
}


