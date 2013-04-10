/*
 * VideoRange.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef VIDEORANGE_H_
#define VIDEORANGE_H_

namespace mm_states {

struct VideoRange {
	float	start;
	float	end;

	bool operator==(const VideoRange &other){
		return start == other.start && end == other.end;
	}
};

}
#endif /* VIDEORANGE_H_ */
