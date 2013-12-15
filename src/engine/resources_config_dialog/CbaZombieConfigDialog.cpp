/*
 * CbaZombieConfigDialog.cpp
 *
 *   Created on: Dec 4, 2013
 *      Company: CordobaZombie
 *       Author: Carlos E. Budde
 *      License: GPLv3
 */


#include <OgreRoot.h>
#include <QtWidgets/QtWidgets>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>

#include <debug/DebugUtil.h>
#include "CbaZombieConfigDialog.h"
#include "ui_CbaZombieConfigDialog.h"  // UI file with buttons template



namespace engine
{

///////////////////////////////////////////////////////////////////////////////
CbaZombieConfigDialog::CbaZombieConfigDialog(QWidget* parent) :
    QWidget(parent)
,   mOpenALdev(0)
,   mOpenALctx(0)
,   mRenderSystem("")
,   mDisplayFreq(0)
,   mAntiAliasing(0)
,   mVertSync(0)
,   mGammaCorrection(false)
,   mDisplayRes(std::pair<int,int>(640,480))
,   mSoundDevice("")
{
    mOgreRoot = new Ogre::Root();
    // Connect with UI
    mTemplateUI = new Ui::CbaZombieConfigDialog();
    mTemplateUI->setupUi(this);
    connect(mTemplateUI->renderSystem,
            SIGNAL(currentIndexChanged(const std::string&)),
            this,
            SLOT(setRenderSystem()));
}

///////////////////////////////////////////////////////////////////////////////
CbaZombieConfigDialog::~CbaZombieConfigDialog()
{
    if (mTemplateUI)
        delete mTemplateUI;
    if (mOgreRoot)
        delete mOgreRoot;
}



bool
CbaZombieConfigDialog::showConfigDialog(Ogre::Root* root)
{
//    assert(root);
//    root->restoreConfig(); // Restore previous configuration, if any
//
//    /* TODO: probably we'll need to customize this inherited method,
//     *        specifically to implement our own "CreateWindow" */
//    bool isOk = display(); // Show config dialog
//
//    if (isOk)
//        root->saveConfig(); // If chosen parameters are OK, save them

    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
CbaZombieConfigDialog::restoreConfig()
{
    debugERROR("TODO");  // TODO
    return true;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::setRenderSystem(const std::string& rs)
{
    debugERROR("TODO");  // TODO
    return;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::setDisplayFrequency(int freq)
{
    debugERROR("TODO");  // TODO
    return;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::setAntiAliasing(int aa)
{
    debugERROR("TODO");  // TODO
    return;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::setVerticalSync(int vsync)
{
    debugERROR("TODO");  // TODO
    return;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::setGammaCorrection(bool gc)
{
    debugERROR("TODO");  // TODO
    return;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::setDisplayResolution(const std::pair<int,int>& dr)
{
    debugERROR("TODO");  // TODO
    return;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::setSoundDevice(const std::string& sd)
{
    debugERROR("TODO");  // TODO
    return;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::systemRecon()
{
//    root->getAvailableRenderers();
}


///////////////////////////////////////////////////////////////////////////////
bool
CbaZombieConfigDialog::saveConfig()
{
    debugERROR("TODO");  // TODO
    return true;
}

}

