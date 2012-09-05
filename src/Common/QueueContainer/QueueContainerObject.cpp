/*
 * QueueContainerObject.cpp
 *
 *  Created on: 02/05/2012
 *      Author: agustin
 */

#include "QueueContainerObject.h"
#include "DebugUtil.h"


template<typename T>
QueueContainer<T>	*QueueContainerObject<T>::mCont = 0;

template<typename T>
QueueContainerObject<T>::QueueContainerObject()
{
	ASSERT(mCont);
}

template<typename T>
QueueContainerObject<T>::~QueueContainerObject()
{

}

/**
 * Set the Container associated
 */
template<typename T>
void QueueContainerObject<T>::setContainer(QueueContainer<T> *cont)
{
	ASSERT(mCont);
	mCont = cont;
}
