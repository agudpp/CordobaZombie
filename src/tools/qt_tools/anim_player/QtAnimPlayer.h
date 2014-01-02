/*
 * QtAnimPlayer.h
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#ifndef QTANIMPLAYER_H_
#define QTANIMPLAYER_H_


#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreEntity.h>

#include <utils/OrbitCamera.h>

#include <qt_tools/qt_debug/QtDebug.h>
#include <qt_tools/ogre_widget/OgreWidget.h>
#include <qt_tools/ogre_widget/QtOgreAppBase.h>

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
    yRotSliderChanged(int val);
    void
    xRotSliderChanged(int val);

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

private:

    // @brief Load an entity from a file. We assume the resource location
    //        is already loaded.
    //
    bool
    loadMesh(const QString& meshName);

    // @brief Handle the camera input
    //
    void
    handleCameraInput(void);

private:
    Ui::AnimPlayer ui;
    QString mLastPathLoaded;

    Ogre::SceneNode* mNode;
    Ogre::Entity* mEntity;

    OrbitCamera* mOrbitCamera;
};

}

#endif /* QTANIMPLAYER_H_ */
