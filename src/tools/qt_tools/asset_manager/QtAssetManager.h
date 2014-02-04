/*
 * QtAssetManager.h
 *
 *  Created on: Jan 13, 2014
 *      Author: agustin
 */

#ifndef QTASSETMANAGER_H_
#define QTASSETMANAGER_H_


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

#include <asset/Asset.h>

#include "ConfigAssetGUI.h"
#include "ui_AssetManager.h"

namespace tool {

class QtAssetManager : public QtOgreAppBase
{
    Q_OBJECT
public:
    QtAssetManager(rrh::ResourceHandler* rh,
                   const std::string& plugins = "plugins.cfg",
                   const std::string& ogre = "ogre.cfg",
                   const std::string& resource = "resources.cfg");
    virtual
    ~QtAssetManager();



protected slots:
    // inherited slots
    //

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

    // @brief Keypress / mouse events slots received from the OgreWidget.
    //        By default all the events are ignored so the parent class can
    //        catch this events.
    //
    virtual void
    OgreWidgetKeyPressEvent(QKeyEvent* event);
    virtual void
    OgreWidgetKeyReleaseEvent(QKeyEvent* event);
    virtual void
    OgreWidgetMousePressEvent(QMouseEvent* event);
    virtual void
    OgreWidgetMouseReleaseEvent(QMouseEvent* event);
    virtual void
    OgreWidgetMouseMoveEvent(QMouseEvent* event);
    virtual void
    OgreWidgetWheelEvent(QWheelEvent * event);

    // @brief When we click on load asset or create an asset buttons
    //
    void
    onCreateAssetClicked(bool);
    void
    onLoadAssetClicked(bool);

protected:

    // QWidget events.
    //

    // @brief For keypressing | mouse events
    //
    virtual void
    keyPressEvent(QKeyEvent* event);
//    virtual void
//    keyReleaseEvent(QKeyEvent* event);
//    virtual void
//    mousePressEvent(QMouseEvent* event);
//    virtual void
//    mouseReleaseEvent(QMouseEvent* event);
//    virtual void
//    mouseMoveEvent(QMouseEvent* event);

    // @brief This method will be used to show the current asset and all its data
    //        in the ogre windows. If we already have something in the past being
    //        rendered on the ogre window then we will remove it and show the
    //        current one (mCurrentAsset).
    //
    void
    updateCurrentAsset(void);

private:
    Ui::AssetManager ui;

    OrbitCamera* mOrbitCamera;
    core::Primitive* m3DAxis;
    core::Asset mCurrentAsset;

    QPoint mLastMousePoint;
    QString mLastPathLoaded;
    ConfigAssetGUI mConfigWindow;

};

}

#endif /* QTASSETMANAGER_H_ */
