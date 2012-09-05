/*
 * QueueContainerObject.h
 *
 *  Created on: 02/05/2012
 *      Author: agustin
 */

#ifndef QUEUECONTAINEROBJECT_H_
#define QUEUECONTAINEROBJECT_H_

#include "QueueContainer.h"

template<typename T>
class QueueContainerObject
{
public:
	QueueContainerObject();
	virtual ~QueueContainerObject();

	/**
	 * Set the Container associated
	 */
	static void setContainer(QueueContainer<T> *cont);

protected:
	static QueueContainer<T>	*mCont;

};

#endif /* QUEUECONTAINEROBJECT_H_ */
