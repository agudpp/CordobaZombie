
#include <iostream>
#include <string>

/*******************************************************************************
 * ATTEMPT #1
 * 
#if defined _WIN32 || defined CYGWIN
#  include <unordered_map>
#  define  UNORDERED_MAP<a,b,c,d,e>  unordered_map<(a),(b),(c),(d),(e)>
#elif defined linux || defined _linux || defined __linux || defined __linux__
#  ifdef _TR1
#    include <tr1/unordered_map>
#    define  UNORDERED_MAP(a,b,c,d,e)  std::tr1::unordered_map<(a),(b),(c),(d),(e)>
#  else
#    include <map>
#    define  UNORDERED_MAP(a,b,c,d,e)  std::map<(a),(b),(d),(e)>
#  endif
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif

template <typename _Key, typename _Value,
		class _Hash,
		class _Pred  = std::equal_to<_Key>,
		class _Alloc = std::allocator<std::pair<const _Key, _Value> > >
struct _HashTable
{
	typedef UNORDERED_MAP(_Key,_Value,_Hash,_Pred,_Alloc) HashTable;
};

typedef _HashTable<std::string, int>::HashTable MyHashTable;
*/




/*******************************************************************************
 * ATTEMPT #2
 *
#if defined _WIN32 || defined CYGWIN
#  include <unordered_map>
#  define  HASH_MAP(aa,bb)  unordered_map<(aa),(bb)>
#elif defined linux || defined _linux || defined __linux || defined __linux__
#  ifdef _TR1
#    include <tr1/unordered_map>
#    define  HASH_MAP(aa,bb)  std::tr1::unordered_map<aa,bb,cc,dd,ee>
#  else
#    include <map>
#    define  HASH_MAP(aa,bb,cc,dd,ee)  std::map<aa,bb,dd,ee>
#  endif
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif

template <typename _Key, typename _Value,
		class _Hash  = std::tr1::hash<_Key>,
		class _Pred  = std::equal_to<_Key>,
		class _Alloc = std::allocator<std::pair<const _Key, _Value> > >
struct _HashTable
{
	typedef HASH_MAP<_Key,_Value,_Hash,_Pred,_Alloc> HashTable;
#  ifdef _TR1
	typedef std::tr1::unordered_map<_Key,_Value,_Hash,_Pred,_Alloc> HashTable;
#  else
	typedef std::map<_Key,_Value,_Pred,_Alloc> HashTable;
#  endif
};

typedef _HashTable<std::string, unsigned long>::HashTable hashStrULong;
*/




/*******************************************************************************
 * ATTEMPT #3
 *
#if defined _WIN32 || defined CYGWIN
#  include <unordered_map>
#  define  HASH_MAP(aa,bb)  unordered_map< aa , bb >
#elif defined linux || defined _linux || defined __linux || defined __linux__
#  ifdef _TR1
#    include <tr1/unordered_map>
#    define  HASH_MAP(aa,bb)  std::tr1::unordered_map< aa , bb >
#  else
#    include <map>
#    define  HASH_MAP(aa,bb)  std::map< aa , bb >
#  endif
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif

typedef HASH_MAP(std::string,unsigned long) hashStrULong;
*/




/*******************************************************************************
 * ATTEMPT #4
 */
////////////////////////////////////////////////////////////////////////////////
#if defined _WIN32 || defined CYGWIN

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
#elif defined linux || defined _linux || defined __linux || defined __linux__

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


typedef _HashTable<std::string, unsigned long *>::HashTable hashStrULong;


#define  N  5
std::string keys[N] = { "zombie1",
			"zombie2",
			"grito",
			"quejido",
			"explosión" };
unsigned long values[N] = { 0, 1, 2, 3, 4};

int
main (void)
{
        hashStrULong mBuffers;
	hashStrULong::const_iterator elem = mBuffers.begin();
        unsigned long i = 0;
	unsigned long *dInt = 0;
	const std::string bad_key = "<bad_key>";

#ifdef __GXX_EXPERIMENTAL_CXX0X__
	std::cout << "\nc++0x standard!\n\n";
#endif

	/* Writing... */
        for (i=0 ; i<N-1 ; i++) {
		/* ...with operator[]... */
                mBuffers[keys[i]] = &(values[i]);
        }
	/* ...with insert() */
	mBuffers.insert(std::make_pair<std::string, unsigned long *>
			(keys[i], &(values[i])));

	/* Reading */
	i = mBuffers.size();
	std::cout << "mBuffers has " << i <<  " elements.\n";
       	for (i=0 ; i<N ; i++) {
		std::cout << "mBuffers[\"" << keys[i] << "\"]\t==  "
			  << *(mBuffers[keys[i]]) << std::endl;
	}

	/* Reading inexistent element */
	try {
		std::cout << "mBuffers[\"" << bad_key << "\"]\t==  "
			  << (mBuffers[bad_key] ? *(mBuffers[bad_key]) : 0)
			  << std::endl;
		/* Deleting ("bad_key" entry should have been added) */
		std::cout << "Erasing default element added at position "
			  << mBuffers.erase(bad_key)
			  << std::endl;
        } catch (...) {
		std::cerr << "Outta' range!\n";
	}

	/* Finding by key */
	if (mBuffers.find(keys[2]) != mBuffers.end()) {
		std::cout << "mBuffers[\"" << keys[2] << "\"]\t==  "
			  << *(mBuffers.find(keys[2])->second)
			  << std::endl;
	}
	if (mBuffers.find(bad_key) != mBuffers.end()) {
		std::cout << "mBuffers[\"" << bad_key << "\"]\t==  "
			  << *(mBuffers.find(bad_key)->second)
			  << std::endl;
	} else {
		std::cout << "mBuffers.find(\"" << bad_key << "\")\t== NULL\n";
	}

	/* Dynamic memory manipulation */
	dInt = new unsigned long;
	*dInt = 100;
	mBuffers["dInt"] = dInt;
	elem = mBuffers.find("dInt");
	if (elem != mBuffers.end()) {
		delete elem->second;
		mBuffers.erase(elem->first);
	}
	elem = mBuffers.find("Surely_not_a_valid_key");
	if (elem != mBuffers.end()) {
		delete elem->second;
		mBuffers.erase(elem->first);
	}

	return 0;
}
