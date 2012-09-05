/*
 * BackpackItem.cpp
 *
 *  Created on: 14/05/2012
 *      Author: agustin
 */


#include "BackpackItem.h"




////////////////////////////////////////////////////////////////////////////////
void BackpackItem::rightPressed(void)
{
	debugERROR("Tenemos que mostrar el AdditionalInfo asociado a este item. "
			"Esto es correr la \"MicroApp\" para que muestre la imagen asociada\n");
	ASSERT(false);
}


////////////////////////////////////////////////////////////////////////////////
BackpackItem::BackpackItem(int t) :
mSection(t),
mUserDef(0),
mOwner(0)
{

}

////////////////////////////////////////////////////////////////////////////////
BackpackItem::~BackpackItem()
{

}

