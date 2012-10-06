/*
 * StatesFactory.cpp
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#include "StatesFactory.h"

#include "DebugUtil.h"

#include "CreditsState.h"
#include "HistoryState.h"
#include "MainState.h"
#include "ConfigState.h"

namespace mm_states {

bool StatesFactory::buildStates(std::vector<IState *> &result)
{
    result.clear();
    result.push_back(new MainState);
//    ASSERT(false && "descomentar las lineas de abajo");
//    result.push_back(new ConfigState);
    result.push_back(new CreditsState);
//    result.push_back(new HistoryState);

    return true;
}


}
