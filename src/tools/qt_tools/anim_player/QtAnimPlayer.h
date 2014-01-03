/*
 * QtAnimPlayer.h
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#ifndef QTANIMPLAYER_H_
#define QTANIMPLAYER_H_

#include <QtGui/QCheckBox>
#include <QtGui/QLabel>

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <utils/OrbitCamera.h>

#include <qt_tools/qt_debug/QtDebug.h>
#include <qt_tools/ogre_widget/OgreWidget.h>
#include <qt_tools/ogre_widget/QtOgreAppBase.h>
#include <debug/PrimitiveDrawer.h>

#include "ui_AnimPlayer.h"

namespace tool {

class QtAnimPlayer : public QtOgreAppBase
{
    Q_OBJECT
public:
    QtAnimPlayer(rrh::ResourceHandler* rh = 0,
                 const std::string& plugins = "plugins.cfg",
                 const std::string& ogre = "ogre.cfg",
                 const std::string& resource = "resources.cfg");
    virtual
    ~QtAnimPlayer();

public slots:
    void
    onLoadClicked(bool);
    void
    onLoadResourceClicked(bool);
    void
    yRotSliderChanged(int val);
    void
    xRotSliderChanged(int val);
    void
    zoomIncClickedChanged(bool);
    void
    zoomDecClickedChanged(bool);
    void
    velControllerSliderChanged(int val);

protected slots:

    // @brief The child class should implement this if we want to receive this
    //        this event
    //
    virtual void
    frameUpdate(float fp);

    // @brief This method will be called when we have all the systems ready
    //        since we don't know when Ogre is built (qt and ogre problems)
    //        this method will be called to advise that we have the systems ready.
    //
    virtual void
    systemsReady(void);

protected:

    // QWidget events.
    //

    // @brief For keypressing | mouse events
    //
    virtual void
    keyPressEvent(QKeyEvent* event);
    virtual void
    keyReleaseEvent(QKeyEvent* event);
    virtual void
    mousePressEvent(QMouseEvent* event);
    virtual void
    mouseReleaseEvent(QMouseEvent* event);
    virtual void
    mouseMoveEvent(QMouseEvent* event);

private:

    // @brief Load all the animations from a given entity
    // @param entity    The entity we want to load all the animations
    //
    void
    loadAnimations(Ogre::Entity* entity);

    // @brief Destroy all the current loaded animations
    //
    void
    destroyAnimationStuff(void);

    // @brief Load an entity from a file. We assume the resource location
    //        is already loaded.
    //
    bool
    loadMesh(const QString& meshName);


private:

    // Internal structure to hold the animation information
    //
    struct AnimInfo {
        Ogre::AnimationState* animState;
        QLabel* label;
        QCheckBox* button;

        AnimInfo() :
            animState(0)
        ,   label(0)
        ,   button(0)
        {}

        ~AnimInfo()
        {
            delete button;
            delete label;
        }
    };

private:
    Ui::AnimPlayer ui;
    QString mLastPathLoaded;

    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;

    OrbitCamera* mOrbitCamera;
    core::Primitive* m3DAxis;

    QPoint mLastMousePoint;
    float mAnimVel;
    std::vector<AnimInfo*> mAnimInfo;
};

}

#endif /* QTANIMPLAYER_H_ */
