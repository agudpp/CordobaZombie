
/*
Each Unit:      Each Game:         Each Source:
SoundAPI  <---> SoundManager <---> SoundSource
*/

/* Supported audio formats */
typedef enum {
	SS_MP3,
	SS_OGG,
	SS_WAV
} SSformat;

/* SoundSystem error codes */
typedef enum {
	SS_NO_ERROR,
	SS_NO_MEMORY,
	SS_NO_SOURCES,		/* No free sources */
	SS_FILE_NOT_FOUND,	/* Cannot load sound, inexistent filename */
	SS_NO_BUFFER_ATTACHED,	/* Cannot play: no buffers attached to source */
	SS_INTERNAL_ERROR
} SSerror;

/* SoundBattery playback state */
typedef enum {
	SS_KEEP_PLAYING,
	SS_PAUSED,
	SS_FINISHED
} SSplayback;

/* Maximum buffer length to load into memory.
 * Audio files larger than this will use streaming. */
#define	STREAM_LEN	1<<18  // 256 KB, ~10 seconds playing

typedef std::tr1::unordered_map<std::string, ALuint> HashTable;

typedef struct {
	ALuint   name;
	SSformat format;
	uint64_t len;  // #include <cstdint>
} SoundBuffer;


/******************************************************************************/
                      /********************************************************/
class SoundManager    /********************************************************/
{
friend class SoundAPI;
struct ActiveSound {
	/* Currently playing sound (unit + source) */
	SoundAPI*    sa;
	SoundSource* ss;
};
public:
	/* Use a singleton pattern with getInstance().            *
	 * This is threadsafe on Linux if compiling with gcc/g++. *
	 * Windows might require an explicit critical section.    */
	static SoundManager& getInstance()
	{
		static SoundManager instance;
		return instance;
	}
	SSerror   loadSound (const std::string& fname);
	SSerror unloadSound (const std::string& fname);
	void update();  // Iterates over the mActiveSounds private member
protected:
	SSerror playSound  (const SoundAPI* sa,
			    const std::string* name,
			    bool  repeat=false);
	SSerror pauseSound (const SoundAPI* sa);
	SSerror stopSound  (const SoundAPI* sa);
private:
	/* getInstance() singleton pattern requires ctor,     *
	 * copytor and assign methods to be declared private. */
	SoundManager(){/*TODO*/}
	SoundManager(SoundManager const&);    // Don't implement!
	void operator=(SoundManager const&);  // Don't implement!
private:
	/* Small sounds, loaded on startup.
	 * Buffers can be shared, and remain in memory between plays. */
	HashTable<std::string, SoundBuffer> mLoadedBuffers;
	/* Playing abstraction (contains all sources) */
	QueueContainer<SoundSource> mAvailableSources;
	/* Currently playing Sounds: */
	std::vector <ActiveSound> mActiveSounds;
}


/******************************************************************************/
/* Most parts of the SoundSource class depend on the type of SoundBuffer/s    */
/* they contain. Internal structures and functions behaviour change visibly   */
/* between wav, ogg and mp3.                                                  */
                    /**********************************************************/
class SoundSource   /**********************************************************/
{
public:
	SoundSource();
	virtual ~SoundSource()=0;
	virtual SSerror play(const SoundBuffer* sb,
			     const Ogre::Vector3& 3Dpos,
			     bool  repeat=false)=0;
	virtual SSplayback update()=0;
private:
	SoundBuffer* mBuffer;
	bool	     mRepeat;
}


/******************************************************************************/
/* Every Unit in the game is allowed to have his own SoundAPI.                */
/* This SoundAPI is the only interaction between such Unit                    */
/* and the game's sound system.                                               */
                  /************************************************************/
class SoundAPI    /************************************************************/
{
friend class SoundManager;
public:
	SoundAPI();
	~SoundAPI();
	SSerror play(const std::string &fname, bool repeat=false);
	void pause();
	void stop();
protected:
	/* Get sound 3D position. */
	const Ogre::Vector3 &getPosition()
	{return mNode->getPosition();}
	/* Callback from SoundManager, when currently playing sound ends. */
	SSplayback playingEnded();
protected:
	int mActivationIndex;	
private:
	static SoundManager *mSoundManager;
	Ogre::SceneNode     *mNode;  // 3D position
#ifdef DEBUG
private:
	array<char*> viewAttachedSounds();  // For debugging
#endif
}

/*
Play sound call sequence:

Unit:	SoundAPI.play(fname);
sAPI:	SoundManager.playSound(this,fname);
sMGR:	loaded = mActiveSounds.find(sAPI);
	let loaded==false, then:
	src = mAvailableSources.pop();
	mActiveSounds.append(ActiveSound(sAPI,src));
	src.play(mLoadedBuffers[fname], sAPI.getPosition());
donde
	mActiveSounds.find(const SoundAPI& sAPI) {
		return  sAPI->mActivationIndex >= 0
			&& mActiveSounds.size() < sAPI->mActivationIndex
			&& mActiveSounds[sAPI->mActivationIndex] == sAPI;
	}
*/
