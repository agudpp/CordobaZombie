/*
 * OverlayWidgetItem.h
 *
 *  Created on: Jan 6, 2014
 *      Author: agustin
 */

#ifndef OVERLAYWIDGETITEM_H_
#define OVERLAYWIDGETITEM_H_

#include <vector>

#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

#include <OgreOverlayManager.h>
#include <OgreOverlayElement.h>
#include <OgreOverlay.h>

#include <qt_tools/qt_debug/QtDebug.h>

#include "OverlayContainerItem.h"

#include "ui_OverlayItem.h"

namespace tool {

class OverlayWidgetItem : public QWidget
{
    Q_OBJECT

public:
    // @brief We take a overlay to build the widget
    OverlayWidgetItem(Ogre::Overlay* overlay, QWidget* parent = 0);
    virtual
    ~OverlayWidgetItem();

    // @brief build the widget
    // @return true on success | false otherwise
    //
    bool
    build(void);

    // @brief Return the associated overlay
    //
    inline Ogre::Overlay*
    overlay(void) {return mOverlay;}

protected slots:

    // local slots
    //
    void
    onShowHideClicked(int);

private:
    Ui::OverlayItem ui;
    Ogre::Overlay* mOverlay;
    std::vector<OverlayContainerItem*> mContainers;


};

} /* namespace tool */
#endif /* OVERLAYWIDGETITEM_H_ */
