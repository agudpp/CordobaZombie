/*
 * MainStateID.h
 *
 *  Created on: Oct 29, 2013
 *      Author: agustin
 */

#ifndef MAINSTATEID_H_
#define MAINSTATEID_H_


namespace cz {

// @brief All the ID's should be consecutive and should be between [0, Count)
//

enum MainStateID {
    InfoState = 0,
    IntroState,
    MenuState,
    PrePlayingState,
    PlayingState,
    ExitState,

    Count,
};

}


#endif /* MAINSTATEID_H_ */
