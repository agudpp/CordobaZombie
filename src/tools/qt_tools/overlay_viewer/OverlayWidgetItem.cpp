/*
 * OverlayWidgetItem.cpp
 *
 *  Created on: Jan 6, 2014
 *      Author: agustin
 */

#include "OverlayWidgetItem.h"

namespace tool {

OverlayWidgetItem::OverlayWidgetItem(Ogre::Overlay* overlay, QWidget* parent) :
    QWidget(parent)
,   mOverlay(overlay)
{
    ui.setupUi(this);

    // set the connections here
    connect(ui.checkBox, SIGNAL(stateChanged(int)),
            this, SLOT(onShowHideClicked(int)));

    // configure the overlay containers items
}

OverlayWidgetItem::~OverlayWidgetItem()
{
    for (OverlayContainerItem* i : mContainers) {
        delete i;
    }

    mContainers.clear();
}

bool
OverlayWidgetItem::build(void)
{
    if (!mOverlay || !mContainers.empty()) {
        QTDEBUG_CRITICAL("We didn't set an overlay here or we already have"
            "built the overlay widget!? wtf\n");
        return false;
    }

    // configure the current widget
    ui.overlayLabel->setText(mOverlay->getName().c_str());
    ui.checkBox->setChecked(mOverlay->isVisible());

    // build all the OverlayContainer items and add them in the layout
    //
    // now we have to create the child elements here
    Ogre::Overlay::Overlay2DElementsIterator elems = mOverlay->get2DElementsIterator();
    while (elems.hasMoreElements()) {
        Ogre::OverlayContainer* elem = elems.getNext();
        if (elem == 0) continue;

        OverlayContainerItem* item = new OverlayContainerItem(elem, this);

        // create the item in the real layout
        ui.verticalLayout->addWidget(item);

        // add this element to the list of elements
        mContainers.push_back(item);
    }

    return true;
}

void
OverlayWidgetItem::onShowHideClicked(int)
{
    if (ui.checkBox->isChecked()) {
        mOverlay->show();
    } else {
        mOverlay->hide();
    }
}

} /* namespace tool */
