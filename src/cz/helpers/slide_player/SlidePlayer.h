/*
 * SlidePlayer.h
 *
 *  Created on: Nov 29, 2013
 *      Author: raul & agustin :)
 */

#ifndef SLIDEPLAYER_H_
#define SLIDEPLAYER_H_

#include <functional>

#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgreString.h>

#include <types/StackVector.h>
#include <math/Vec2.h>
#include <math/AABB.h>
#include <overlay_effects/SlideOverlayEffect.h>


// forward
//
namespace effect {
class EffectHandler;
}

// @brief This class will be used to show slides in an specific way.
//        Note that this class completely depends of the effect handler so
//        you need to update the effect handler to be able to work with this.
//


// some defines
#define SLIDE_PLAYER_CONFIG_OVERLAY     "SlidePlayerOverlayConfig"

namespace cz {

class SlidePlayer
{
    // Internal enum to identify if we are moving forward or backward
    //
    enum MoveDir {
        NONE = 0,
        FORWARD,
        BACKWARD
    };
public:
    // Some defines
    //
    static const unsigned int MAX_SLIDES = 32;

    typedef core::StackVector<Ogre::String, MAX_SLIDES> SlidesVec;

    // define the default function
    static const ui::SlideOverlayEffect::VelFunction SIN_FUN;

    // Configuration information
    //
    struct Info {
        // the effect handler instance to process the effects
        effect::EffectHandler* effectHandler;
        // The overlay used to configure the slide player
        Ogre::String configOverlayName;
        // the slide time to be used to "slide" the slides XD
        float slideTime;
        // The slide function to move the slides. If not set then we will use the
        // default one that is some kind of sine function (velocity)
        const ui::SlideOverlayEffect::VelFunction* velFun;

        Info() :
            effectHandler(0)
        ,   configOverlayName(SLIDE_PLAYER_CONFIG_OVERLAY)
        ,   slideTime(1.f)
        ,   velFun(&SIN_FUN)
        {}
    };
public:
    SlidePlayer();
    ~SlidePlayer();

    // @brief Configure the slide player with the data we need.
    // @param info      The config info.
    // @return true on succes | false otherwise
    //
    bool
    configure(const Info& info);
    inline const Info&
    info(void) const;

    // @brief Set the slides we will use (or
    // @param slides            The list of all the slides (material names) to
    //                          be shown in the order that are given
    // @return true on succes | false otherwise
    // @note Do not delete nor remove this overlay during the use of this instance
    //
    bool
    setSlides(const SlidesVec& slides);

    // @brief Check if we have slides in this player
    // @return true if we have | false otherwise
    //
    inline bool
    hasSlides(void) const;

    // @brief Return the number of slides we have
    // @return the slides count
    //
    inline unsigned int
    slidesCount(void) const;


    ////////////////////////////////////////////////////////////////////////////
    //  Handling the slide player
    //

    // @brief Show / hide the slide player.
    //
    inline void
    setVisible(bool visible);

    // @brief Move to the next slide.
    // @return true if we have more slides | false otherwise (i.e: it was the
    //         last slide).
    //
    bool
    next(void);

    // @brief Move to the previous one
    // @return true if we have more slides to move backward | false otherwise
    //         (i.e we are in the first one).
    //
    bool
    prev(void);

    // @brief Move to the i-th slide.
    // @return true if we can | false otherwise
    //
    bool
    seek(unsigned int i);

    // @brief Reset to the initial state
    //
    void
    reset(void);

    // @brief Check if we can move right now or not to another slide (i.e this
    //        way we can check if we are currently moving the slide).
    // @return true if we can move | false otherwise
    //
    bool
    canMove(void) const;

private:

    // avoid copying
    SlidePlayer(const SlidePlayer&);
    SlidePlayer& operator=(const SlidePlayer&);

    // @brief Load the configuration from the Overlay associated
    // @return true on success | false otherwise
    //
    bool
    buildFromOverlay(const Ogre::String& overlayName);

    // @brief Configure the SlidePlayer from the already initialized overlay and
    //        the given list of slides.
    //
    void
    init(void);

    // @brief Start and configure effects
    //
    void
    configureEffects(void);

private:
    Info mInfo;
    SlidesVec mSlides;
    unsigned int mCurrentSlide;
    Ogre::Overlay* mConfigOverlay;
    Ogre::OverlayContainer* mCenterSlide;
    Ogre::OverlayContainer* mPrevSlide;
    Ogre::OverlayContainer* mNextSlide;
    Ogre::OverlayContainer* mHiddenSlide;

    ui::SlideOverlayEffect mCenterEffect;
    ui::SlideOverlayEffect mPrevEffect;
    ui::SlideOverlayEffect mNextEffect;
    ui::SlideOverlayEffect mHiddenEffect;

    core::Vector2 mCenterTL;
    core::Vector2 mPrevTL;
    core::Vector2 mNextTL;
    core::Vector2 mHiddenPrevTL;
    core::Vector2 mHiddenNextTL;

    MoveDir mMoveDir;

};














////////////////////////////////////////////////////////////////////////////////
// inline stuff
//

inline const SlidePlayer::Info&
SlidePlayer::info(void) const
{
    return mInfo;
}

inline void
SlidePlayer::setVisible(bool visible)
{
    if (mConfigOverlay) {
        if (visible) {
            mConfigOverlay->show();
        } else {
            mConfigOverlay->hide();
        }
    }
}

inline bool
SlidePlayer::hasSlides(void) const
{
    return mSlides.empty();
}

inline unsigned int
SlidePlayer::slidesCount(void) const
{
    return mSlides.size();
}

} /* namespace cz */
#endif /* SLIDEPLAYER_H_ */
