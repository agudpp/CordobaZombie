/*
 * MainMenuState.h
 *
 *  Created on: 03/09/2012
 *      Author: agustin
 *
 */

#ifndef MAINMENUSTATE_H_
#define MAINMENUSTATE_H_

#include <set>
#include <vector>
#include <string>

#include "IMainState.h"
#include "MainStateMachineDefs.h"

#include "MainMenuState/IState.h"

#include "OverlayEffectManager.h"
#include "MenuManager.h"
#include "GeneralTypedefs.h"
#include "DebugUtil.h"
#include "VideoRange.h"
#include "FfmpegVideoPlayerAPI.h"
#include "OverlayEffectManager.h"
#include "XMLHelper.h"



class MainMenuState : public IMainState {

	typedef std::vector<mm_states::IState *>	StatesVector;
	typedef std::vector<mm_states::VideoRange>	VideoRangeVec;

	// internal video states
	enum VideoState {
		Entering = 0,
		Updating,
	};


	// The xml filename to be used to load all the information.
	static const char *CONFIG_FILENAME;

	// Ugly internal exception handling
	struct ExceptionInfo {
		ExceptionInfo(const std::string &i, int ec) :
			info(i),
			errCode(ec)
		{
		}

		std::string info;
		int			errCode;
	};

	// Calback receiver (this will advise the MainMenuState if some state
	// send some event
	class CallbackReceiber :
		public GenericFunctor2<void, mm_states::IState *, mm_states::Event>
	{
	public:
		CallbackReceiber(MainMenuState &s);
		void operator()(mm_states::IState *s, mm_states::Event e);

	private:
		MainMenuState &mMainMenu;
	};

public:
	MainMenuState();
	virtual ~MainMenuState();

    /**
     * Function used to get the resources files used by the state.
     * The list returned is the list of the resources used by and only by this
     * state.
     */
    virtual void getResources(IMainState::ResourcesInfoVec &resourcesList,
                              const MainMachineInfo &info) const;

	/**
	 * Entering the state with additional info
	 */
	virtual void enter(const MainMachineInfo &info);

	/**
	 * Update the state... This function will be called once.
	 * @param	info	The structure used to pass throw the states. If we want
	 * 					to fill it with some information then we can do it.
	 * @return	event	The event that was happend.
	 */
	virtual MainMachineEvent update(MainMachineInfo &info);

	/**
	 * Function called when the state is not "the actual" anymore
	 */
	virtual void exit(void);

private:
	friend class CallbackReceiber;

	/**
	 * Bridge function used to receive the events from the IStates
	 */
	void newStateEvent(mm_states::IState *state, mm_states::Event e);

	/**
	 * Function used to load an configure all the structure data used by the
	 * MainMenuState. This functions could throw an exceptionInfo object if
	 * they fails
	 */
	void configureMenuManager(void);
	void configureOvEffectManager(void);
	void configureSoundManager(void);
	void configureVideoAPI(void);


	/**
	 * Auxiliar function used to get a IState from a name.
	 */
	inline mm_states::IState *getStateByName(const Ogre::String &name) const;

	/**
	 * Function used to get the next state given the actual state and some event
	 * (this is the transition "function" implemented in this class).
	 * If we add other states we have to change this function to support the
	 * new transitions
	 * @param	e		The event occurred
	 * @param	ge		This is a "GlobalEvent" used to differentiate the cases
	 * 					where the next state is "Exit" or "PlayGame" where there
	 * 					are no other "next" state but is not an error
	 * @return	state	The next state to be executed (or 0 if there are not
	 * 					next event)
	 */
	mm_states::IState *nextState(mm_states::Event e,
			MainMachineEvent &ge);

	/**
	 * Auxiliary function to establish the new state / remove the old one
	 */
	void configureNewState(mm_states::IState *newState);

	/**
	 * Auxiliary function used to get the VideoState given the actual video
	 * position and a VideoRangeVec
	 * @param	range	The video range configuration of the state
	 * @return	state	The resulting state (given the video actual position)
	 */
	VideoState getVideoState(void);

	/**
	 * Main function logic.
	 */
	void updateStateMachine(void);


private:

	StatesVector					mStates;
	mm_states::IState 				*mActualState;
	mm_states::IState 				*mLastState;
	mm_states::Event 				mLastEvent;
	bool							mBeforeUpdateCalled;
	VideoRangeVec					mEnteringRanges;
	OvEff::OverlayEffectManager		mOvEffManager;
	mm::VideoPlayerAPI					*mVideoPlayerAPI;
	CallbackReceiber                mCbReceiver;
	XMLHelper                       mXmlHelper;
	std::set<Ogre::String>			mSoundsFilenames;
};




////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
inline mm_states::IState *MainMenuState::getStateByName(const Ogre::String &name) const
{
	const int size = mStates.size();
	for(int i = 0; i < size; ++i){
		if(mStates[i] && mStates[i]->name() == name) return mStates[i];
	}
	return 0;
}



#endif /* MAINMENUSTATE_H_ */
