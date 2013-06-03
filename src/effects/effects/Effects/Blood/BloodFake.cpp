/*
 * BloodFake.cpp
 *
 *  Created on: Apr 3, 2012
 *      Author: Emi
 */

#include "BloodFake.h"

namespace effects {

int BloodFake::numberOfInstance = 0;

std::vector<Blood *> *BloodFake::availableEffects = 0;

BloodFake::BloodFake() :
    Effects(0.05)
{

    numberOfInstance++;

    type = effectsEnum::BloodFake;

    std::stringstream sstm;
    sstm << "BloodFake" << numberOfInstance;
    name = sstm.str();

    availableEffects = EffectQueues::getInstance()->getBloodQueue();

}

BloodFake::~BloodFake()
{

}

void
BloodFake::start()
{
    std::cout << "startEffect" << std::endl;

    started = true;
    ended = false;

}

void
BloodFake::end()
{
    ended = true;
    started = false;
    mTimer = 0.0;
}

bool
BloodFake::hasEnded()
{
    return ended;
}

void
BloodFake::update()
{

}

}
