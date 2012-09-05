/*
 * MultiplatformTypedefs.h
 *
 *  Some useful multiplatform type definitions.
 *
 *  Created on: Jun 4, 2012
 *     Company: estrategia201Z
 *      Author: Budde, Carlos Esteban.
 */

#ifndef GLOBALTYPEDEFS_H_
#define GLOBALTYPEDEFS_H_


/**************************************
 *                                    *
 *        Generic Hash Tables         *
 *                                    *
 **************************************
 *
 * @usage
 *
 * typedef _HashTable<typename aaa, typename bbb>::HashTable HashAaaBbb;
 *
 * For instance:
 * typedef _HashTable<int, std::string>::HashTable HashIntStr;
 * or
 * typedef _HashTable<std::string, SoundBuffer*>::HashTable HashStrBuff;
 * or
 * typedef _HashTable<std::string,
 * 					  myType,
 * 					  std::tr1::hash<std::string>,  // TR1 default hash function for strings
 * 					  std::equal_to<std::string>,   // STD default string compare function
 * 					  myAlloc>						// Custom pair<std::string,myType> allocator
 * 					  ::HashTable HashStrMType;
 */
////////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(CYGWIN)

#  include <unordered_map>
template<typename _Key, typename _Value,
 	class _Hash  = std::hash<_Key>,      // hash function
 	class _Pred  = std::equal_to<_Key>,  // key compare function
 	class _Alloc = std::allocator<std::pair<const _Key, _Value> > >
struct _HashTable
{
	typedef std::unordered_map<_Key, _Value, _Hash, _Pred, _Alloc> HashTable;
};

////////////////////////////////////////////////////////////////////////////////
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)

#  ifdef __GXX_EXPERIMENTAL_CXX0X__
#    include <unordered_map>
template<typename _Key, typename _Value,
        class _Hash  = std::hash<_Key>,      // hash function
        class _Pred  = std::equal_to<_Key>,  // key compare function
        class _Alloc = std::allocator<std::pair<const _Key, _Value> > >
struct _HashTable
{
        typedef std::unordered_map<_Key, _Value, _Hash, _Pred, _Alloc> HashTable;
};
#  else
#    include <tr1/unordered_map>
#    include <hash_map>
template<typename _Key, typename _Value,
        class _Hash  = std::tr1::hash<_Key>,  // hash function
        class _Pred  = std::equal_to<_Key>,   // key compare function
        class _Alloc = std::allocator<std::pair<const _Key, _Value> > >
struct _HashTable
{
        typedef __gnu_cxx::hash_map<_Key, _Value, _Hash, _Pred, _Alloc> HashTable;
};
#  endif
////////////////////////////////////////////////////////////////////////////////
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif
/**************************************
 * @remarks
 *
 * Supposedly the new C++ standard (i.e. c++0x) defines the following syntax
 * for real template typedefs, "previously unavailable":
 *
 * template <typename _Key, typename _Value,
 *		class _Hash  = std::tr1::hash<_Key>,  // hash function
 *		class _Pred  = std::equal_to<_Key>,   // key compare function
 *		class _Alloc = std::allocator<std::pair<const _Key, _Value> > >
 * using HashTable = HASH_TABLE<_Key,_Value,_Hash,_Pred,_Alloc>;
 *
 * However, IT DOESN'T WORK !!!
 *
 * ... fucking standards ...
 *
 **************************************/



#endif /* GLOBALTYPEDEFS_H_ */
