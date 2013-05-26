/*
 * GerenalTypedefs.h
 *
 *  Created on: 19/06/2012
 *      Author: agustin
 */

#ifndef GERENALTYPEDEFS_H_
#define GERENALTYPEDEFS_H_


// Generic functor
template<typename _retT = void, typename _argT = void>
struct GenericFunctor {
	virtual _retT operator()(_argT) = 0;
};
// for example we can use something like this:
// typedef GenericFunctor<int,int>	IntIntFunctor; (were we will return int and
// receive as arguments an int too.

template<typename _retT = void, typename _argT1 = void, typename _argT2 = void>
struct GenericFunctor2 {
	virtual _retT operator()(_argT1, _argT2) = 0;
};
#endif /* GERENALTYPEDEFS_H_ */
