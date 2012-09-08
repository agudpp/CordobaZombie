
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

	/*
	 * Queue a single slide into the slide player.
	 */
	int queueSlide ();

	/*
	 * Queue a set of slides into the slide player.
	 */
	int queueSlides();

	/*
	 * Move to next slide.
	 */
	int next();

	/*
	 * Move to previous slide.
	 */
	int prev();

	/*
	 * Move to the i'th slide.
	 */
	int seek(int index);

	/*
	 * Plays the slides one after the other, showing each of them during
	 * 'tlapse' seconds.
	 */
	int playSlides(float tlapse);

private:

	std::vector<> 			mSlideVec; 	//Vector of slides to show
	std::vector<>::iterator mSlide;		//Points to the actually showed slide

};



#endif // SLIDE_PLAYER_H_
