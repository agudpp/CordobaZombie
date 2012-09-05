
/*
 * Module: 	Slide Player
 * Author: 	Raul
 * Date:	5 Sep 2012
 */


#ifndef SLIDE_PLAYER_H_
#define SLIDE_PLAYER_H_

#include <vector>



class SlidePlayer{

public:

	enum{
		SP_END_REACHED = -2,
		SP_ERROR = -1,
		SP_OK = 0,
	};

public:

	int queue_slide ();

	int queue_slides();

	int next();

	int prev();

	int seek(int index);

	int play_slides();

private:

	std::vector<> 			mSlideVec;
	std::vector<>::iterator mSlide;

};



#endif // SLIDE_PLAYER_H_
