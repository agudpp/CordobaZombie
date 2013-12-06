/*
 * GameLogicData.h
 *
 *  Created on: Dec 5, 2013
 *      Author: agustin
 */

#ifndef GAMELOGICDATA_H_
#define GAMELOGICDATA_H_


namespace demo_app {

struct GameLogicData {
    unsigned int goodBoxes;
    unsigned int totalGoodBoxes;
    unsigned int badBoxes;
    unsigned int totalBadBoxes;

    GameLogicData() {reset();}

    void reset(void)
    {
        goodBoxes = badBoxes = totalGoodBoxes = totalBadBoxes = 0;
    }

    bool operator==(const GameLogicData& other) const
    {
        return goodBoxes == other.goodBoxes &&
            totalBadBoxes == other.totalBadBoxes &&
            totalGoodBoxes == other.totalGoodBoxes &&
            badBoxes == other.badBoxes;
    }

    bool operator!=(const GameLogicData& other) const
    {
        return !(*this == other);
    }

    bool win(void) const
    {
        return goodBoxes >= totalGoodBoxes;
    }
    bool loose(void) const
    {
        return badBoxes >= totalBadBoxes;
    }
};

}


#endif /* GAMELOGICDATA_H_ */
