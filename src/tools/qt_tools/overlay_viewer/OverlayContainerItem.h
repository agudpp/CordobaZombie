/*
 * OverlayContainerItem.h
 *
 *  Created on: Jan 6, 2014
 *      Author: agustin
 */

#ifndef OVERLAYCONTAINERITEM_H_
#define OVERLAYCONTAINERITEM_H_


#include <QtGui/QCheckBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

#include <OgreOverlayManager.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayContainer.h>
#include <OgrePanelOverlayElement.h>
#include <OgreOverlay.h>

#include <qt_tools/qt_debug/QtDebug.h>

#include "ui_OverlayContainerItem.h"

namespace tool {

class OverlayContainerItem : public QWidget
{
    Q_OBJECT

public:
    OverlayContainerItem(Ogre::OverlayContainer* container, QWidget* parent = 0);
    virtual
    ~OverlayContainerItem();

protected slots:

    // local slots
    //
    void
    onShowHideClicked(int);
    void
    onChangeAtlasClicked(bool);

private:
    Ui::OverlayContainerItem ui;
    Ogre::OverlayContainer* mContainer;
};

} /* namespace tool */
#endif /* OVERLAYCONTAINERITEM_H_ */
