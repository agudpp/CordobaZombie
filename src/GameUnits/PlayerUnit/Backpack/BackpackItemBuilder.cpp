/*
 * BackpackItemBuilder.cpp
 *
 *  Created on: 23/07/2012
 *      Author: agustin
 *
 */
#include "BackpackItemBuilder.h"


////////////////////////////////////////////////////////////////////////////////
namespace BackpackItemBuilder {
BackpackItemPtr
createBackpackItem(Weapon* w)
{
    BackpackItemPtr result(new BackpackItem(BackpackDef::ItemType::WEAPON));
    result->userDef = w;
    return result;
}

BackpackItemPtr
createBackpackItem(Bomb* b)
{
    BackpackItemPtr result(new BackpackItem(BackpackDef::ItemType::BOMB));
    result->userDef = b;
    return result;
}

BackpackItemPtr
createBackpackItem(GameItem* i)
{
    BackpackItemPtr result(new BackpackItem(BackpackDef::ItemType::ITEM));
    result->userDef = i;
    return result;
}

}
