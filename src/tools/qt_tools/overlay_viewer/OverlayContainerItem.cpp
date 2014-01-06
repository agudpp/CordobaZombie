/*
 * OverlayContainerItem.cpp
 *
 *  Created on: Jan 6, 2014
 *      Author: agustin
 */

#include "OverlayContainerItem.h"

namespace tool {

OverlayContainerItem::OverlayContainerItem(Ogre::OverlayContainer* container,
                                           QWidget* parent) :
    QWidget(parent)
,   mContainer(container)
{
    ASSERT(container);
    ui.setupUi(this);

    // connect signals
    connect(ui.checkBox, SIGNAL(stateChanged(int)),
            this, SLOT(onShowHideClicked(int)));
    connect(ui.changeAtlasBtn, SIGNAL(clicked(bool)),
            this, SLOT(onChangeAtlasClicked(bool)));

    // set the values
    ui.nameLabel->setText(mContainer->getName().c_str());
    ui.checkBox->setChecked(mContainer->isVisible());
}

OverlayContainerItem::~OverlayContainerItem()
{
    // TODO Auto-generated destructor stub
}

void
OverlayContainerItem::onShowHideClicked(int)
{
    ASSERT(mContainer);
    if (ui.checkBox->isChecked()) {
        mContainer->show();
    } else {
        mContainer->hide();
    }
}

void
OverlayContainerItem::onChangeAtlasClicked(bool)
{
    // transform to num the atlas numbers and everything else
    const QString sepXStr = ui.sepXLineEdit->text();
    const QString sepYStr = ui.sepYLineEdit->text();
    const QString atlasIDStr = ui.atlasIDLineEdit->text();

    bool ok = false;
    const int sepX = sepXStr.toInt(&ok);
    if (!ok) {
        QTDEBUG_WARNING("Los valores del atlas deben ser numericos\n");
        return;
    }
    const int sepY = sepYStr.toInt(&ok);
    if (!ok) {
        QTDEBUG_WARNING("Los valores del atlas deben ser numericos\n");
        return;
    }
    const int atlasID = atlasIDStr.toInt(&ok);
    if (!ok) {
        QTDEBUG_WARNING("Los valores del atlas deben ser numericos\n");
        return;
    }

    // veryfy the values
    if (sepX < 0 || sepY < 0 || atlasID >= (sepX*sepY)) {
        QTDEBUG_WARNING("Los valores ingresados son incorrectos. Debemos tener"
            " al menos 1 en X y 1 en Y, y atlasID debe ser valores entre 0 inclusive"
            " y menor que (sepX x sepY)\n");
        return;
    }

    // now we will configure the atlas for this element
    const unsigned int index = sepX * sepY;
    const float uSize = 1.f / static_cast<float>(sepX);
    const float vSize = 1.f / static_cast<float>(sepY);
    unsigned int vVal = atlasID / sepX;
    unsigned int uVal = atlasID - vVal * sepX;

    Ogre::PanelOverlayElement* panel = static_cast<Ogre::PanelOverlayElement*>(mContainer);

    panel->setUV(uVal * uSize,         // u0
                 vVal * vSize,         // v0
                 (uVal+1) * uSize,     // u1
                 (vVal+1) * vSize);    // v1
}

} /* namespace tool */
