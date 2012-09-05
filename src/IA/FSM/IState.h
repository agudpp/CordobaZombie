/*
 * IState.h
 *
 *  Created on: 09/02/2012
 *      Author: agustin
 *
 */

#ifndef ISTATE_H_
#define ISTATE_H_

template<typename T>
class IState {
	static int STATE_ID;
public:
	IState():mID(STATE_ID++){};
	virtual ~IState(){};

	// returns the state id
	inline int getID(void) const {return mID;}

	// Enter the state
	virtual void enter(T) = 0;

	// exit state
	virtual void exit(T) = 0;

	// update state
	virtual void update(T) = 0;

private:
	int mID;
};

template<typename T>
int IState<T>::STATE_ID	= 0;


#endif /* ISTATE_H_ */
