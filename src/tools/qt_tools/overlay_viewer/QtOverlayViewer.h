/*
 * QtOverlayViewer.h
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#ifndef QtOverlayViewer_H_
#define QtOverlayViewer_H_

#include <set>
#include <map>

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
#include <cursor/MouseCursor.h>

#include "OverlayWidgetItem.h"

#include "ui_OverlayViewer.h"

namespace tool {

class QtOverlayViewer : public QtOgreAppBase
{
    Q_OBJECT
public:
    QtOverlayViewer(rrh::ResourceHandler* rh = 0,
                 const std::string& plugins = "plugins.cfg",
                 const std::string& ogre = "ogre.cfg",
                 const std::string& resource = "resources.cfg");
    virtual
    ~QtOverlayViewer();


protected slots:

    // local slots
    //
    void
    onLoadRscClicked(bool);


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

    // @brief Resize event
    //
    virtual void
    OgreWidgetResizeEvent(QResizeEvent* event);

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

    // Helper methods
    //

    // @brief List all the overlays we have in the OverlayManager of ogre.
    //        This method will return a vector of Overlay* with all these overlays.
    // @param overlays      The resulting list of overlays
    //
    void
    getOvleraysLoaded(std::vector<Ogre::Overlay*>& overlays);

    // @brief Destroy all the current overlay data except for a list of overalys
    //        we don't want to destroy.
    //
    void
    destroyOverlayData(const std::set<Ogre::Overlay*>& except);

    // @brief Update all the list of current overlays in the Widget.
    //
    void
    updateAllOverlays(void);

private:
    Ui::OverlayViewer ui;
    QString mLastPathLoaded;

    std::map<Ogre::Overlay*, OverlayWidgetItem*> mOverlayMap;
    std::vector<OverlayWidgetItem*> mOverlayWidgets;
    QWidget* mScrollWidget;
    QVBoxLayout* mScrollAreaLayout;
    ui::MouseCursor mCursor;
};

}

#endif /* QtOverlayViewer_H_ */
