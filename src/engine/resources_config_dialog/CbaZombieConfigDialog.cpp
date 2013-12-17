/*
 * CbaZombieConfigDialog.cpp
 *
 *   Created on: Dec 4, 2013
 *      Company: CordobaZombie
 *       Author: Carlos E. Budde
 *      License: GPLv3
 */


#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>

#include <OgreRoot.h>
#include <OgreString.h>
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreConfigOptionMap.h>
#include <OgreRenderSystem.h>

#include <cstring>
//#include <debug/DebugUtil.h>
#include "CbaZombieConfigDialog.h"
#include "ui_CbaZombieConfigDialog.h"  // UI file with buttons template


namespace engine
{


const char CbaZombieConfigDialog::EC_MODULE_OGRE[30]      = "OgreSystemLoader";
const char CbaZombieConfigDialog::EC_OGRE_OGRECFG[30]     = "ogre";
const char CbaZombieConfigDialog::EC_OGRE_PLUGINSCFG[30]  = "plugins";
const char CbaZombieConfigDialog::EC_MODULE_OPENAL[30]    = "OpenALLoader";
const char CbaZombieConfigDialog::EC_OPENAL_DEVICECFG[30] = "device";

// Following must reflect ***EXACTLY*** the field names used by Ogre
const std::map<std::string, CbaZombieConfigDialog::ConfigField>
CbaZombieConfigDialog::mOgreConfigField = {
    {"Color Depth", CbaZombieConfigDialog::ConfigField::COLOR_DEPTH},
    {"Display Frequency", CbaZombieConfigDialog::ConfigField::DISPLAY_FREQ},
    {"FSAA", CbaZombieConfigDialog::ConfigField::ANTI_ALISAING},
    {"VSync", CbaZombieConfigDialog::ConfigField::VERT_SYNC},
    {"sRGB Gamma Conversion", CbaZombieConfigDialog::ConfigField::GAMMA_CORR},
    {"Video Mode", CbaZombieConfigDialog::ConfigField::DISPLAY_RES},
};


///////////////////////////////////////////////////////////////////////////////
CbaZombieConfigDialog::CbaZombieConfigDialog(QWidget* parent) :
    QWidget(parent)
,   mOpenALdev(0)
,   mOpenALctx(0)
,   mOgreCfgFile("")
,   mOgrePluginsCfgFile("")
,   mOpenALCfgFile("")
,   mRenderSystem("")
,   mColorDepth(0)
,   mDisplayFreq(0)
,   mAntiAliasing(0)
,   mVertSync(0)
,   mGammaCorrection(false)
,   mDisplayRes(std::pair<int,int>(0,0))
,   mSoundDevice("")
{
    // Define internal structures
    mOgreRoot = new Ogre::Root();
    mTemplateUI = new Ui::CbaZombieConfigDialog();
    // Connect with UI
    mTemplateUI->setupUi(this);
    connect(mTemplateUI->renderSystem, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(setRenderSystem(const QString&)));
    connect(mTemplateUI->colorDepth, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setColorDepth(const QString &)));
    connect(mTemplateUI->displayFreq, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(setDisplayFrequency(const QString &)));
    connect(mTemplateUI->antiAliasing, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(setAntiAliasing(const QString &)));
    connect(mTemplateUI->vertSync, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(setVerticalSync(const QString &)));
    connect(mTemplateUI->gammaCorrection, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(setGammaCorrection(const QString &)));
    connect(mTemplateUI->displayRes, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(setDisplayResolution(const QString &)));
    connect(mTemplateUI->soundDevice, SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(setSoundDevice(const QString &)));
    connect(mTemplateUI->saveAndExit, SIGNAL(clicked()),
            this, SLOT(saveConfig()));
    connect(mTemplateUI->discardAndExit, SIGNAL(clicked()),
            this, SLOT(close()));
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
CbaZombieConfigDialog::init(const EngineConfiguration& engineCfg)
{
    bool parseOK(false);

    // Retrieve configuration filenames
    parseOK = engineCfg.getValue(EC_MODULE_OGRE, EC_OGRE_OGRECFG, mOgreCfgFile);
    if (!parseOK) {
        QMessageBox::critical(0, "Error",
                     "Failed to get Ogre configuration filename.");
        return false;
    }
    parseOK = engineCfg.getValue(EC_MODULE_OGRE, EC_OGRE_PLUGINSCFG, mOgrePluginsCfgFile);
    if (!parseOK) {
        QMessageBox::critical(0, "Error",
                     "Failed to get Ogre plugins configuration filename.");
        return false;
    }
//    parseOK = engineCfg.getValue(EC_MODULE_OPENAL, EC_OPENAL_DEVICECFG, mOpenALCfgFile);
//    if (!parseOK) {
//        debugWARNING("Failed to get OpenAL configuration filename.\n");
//        return false;
//    }

    // Parse config files and fill in UI
    reconSystemRenderers();
    parseOK = restoreOgreConfig();
//    parseOK = parseOK && restoreOpenALConfig();

    return parseOK;
}


///////////////////////////////////////////////////////////////////////////////
bool
CbaZombieConfigDialog::restoreOgreConfig()
{
    Ogre::ConfigFile cfg;

    // Check files exist, and try to make Ogre parse them
    if (mOgreCfgFile.empty() || mOgrePluginsCfgFile.empty()) {
        QMessageBox::critical(0, "Error",
                              "Ogre config files have not yet been set.");
        return false;
    }
    try {
        cfg.load(mOgreCfgFile);
    } catch (Ogre::Exception& e) {
        if (e.getNumber() == Ogre::Exception::ERR_FILE_NOT_FOUND) {
            QMessageBox::critical(0, "Error", QString("Ogre config file \"") +
                                  mOgreCfgFile.c_str() + "\" not found");
            return false;
        } else {
            throw(e);
        }
    }

    // Try to read parsed info into our structure
    bool parseOK(false);
    Ogre::ConfigFile::SectionIterator iSection = cfg.getSectionIterator();
    while (iSection.hasMoreElements() && !parseOK) {
        const Ogre::String& renderSystem = iSection.peekNextKey();
        const Ogre::ConfigFile::SettingsMultiMap& settings = *iSection.getNext();
        Ogre::RenderSystem* rs = mOgreRoot->getRenderSystemByName(renderSystem);
        if (!rs)
            continue;  // Unrecognised render system
        // Set this render system as current
        int index = mTemplateUI->renderSystem->findText(rs->getName().c_str(),
                                                        Qt::MatchExactly);
        ASSERT(index>=0);
        mTemplateUI->renderSystem->setCurrentIndex(index);
        // Enable corresponding options
        reconRendererOptions(*rs);

//        Ogre::ConfigFile::SettingsMultiMap::const_iterator i;
//        for (i = settings.begin() ; i != settings.end() ; i++) {
//            reconRendererOptions(*rs, i->first, i->second);
//        }
        parseOK = true;
    }

    return parseOK;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::reconRendererOptions(Ogre::RenderSystem& render)
{
    const Ogre::ConfigOptionMap opts = render.getConfigOptions();
    for (auto it = mOgreConfigField.begin() ; it != mOgreConfigField.end() ;
              it++) {
        Ogre::ConfigOptionMap::const_iterator field = opts.find(it->first);
        if (field == opts.end()) {
            printf("      -- Disabling field \"%s\"\n", it->first.c_str());
            disableRendererOptions(it->first);
        } else {
            fillRendererOptions(field->second);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::reconSystemRenderers()
{
    ASSERT(mOgreRoot);
    Ogre::RenderSystemList rendersList = mOgreRoot->getAvailableRenderers();
    if (rendersList.empty()) {
        QMessageBox::warning(0, "Warning",
                             "No available graphics render systems detected");
        return;
    }
    mTemplateUI->renderSystem->clear();
    for (int i=0 ; i < rendersList.size() ; i++) {
        if (rendersList[i])
            mTemplateUI->renderSystem->addItem(rendersList[i]->getName().c_str());
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::fillRendererOptions(const Ogre::ConfigOption& opt)
{
    auto it = mOgreConfigField.find(opt.name);
    if (it == mOgreConfigField.end()) {
        QMessageBox::warning(0, "Warning", "Unrecognized configuration field: "
                             + QString(opt.name.c_str()) + ", at 251");
        return;
    }
    switch (it->second)
    {
    case ConfigField::COLOR_DEPTH:
        fillUIComboBox(mTemplateUI->colorDepth,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigField::DISPLAY_FREQ:
        fillUIComboBox(mTemplateUI->displayFreq,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigField::ANTI_ALISAING:
        fillUIComboBox(mTemplateUI->antiAliasing,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigField::VERT_SYNC:
        fillUIComboBox(mTemplateUI->vertSync,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigField::GAMMA_CORR:
        fillUIComboBox(mTemplateUI->gammaCorrection,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigField::DISPLAY_RES:
    {   // First we need to format the resolutions, trimming the blanks.
        Ogre::StringVector resolutions(opt.possibleValues);
        for (int i=0 ; i < opt.possibleValues.size() ; i++)
            Ogre::StringUtil::trim(resolutions[i]);
        fillUIComboBox(mTemplateUI->displayRes,
                       resolutions,
                       opt.currentValue.c_str());
    }   break;

    default:
        QMessageBox::warning(0, "Warning", "Unrecognized configuration field: "
                             + QString(opt.name.c_str()) + ", at 298");
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::disableRendererOptions(const std::string& field)
{
    auto it = mOgreConfigField.find(field);
    if (it == mOgreConfigField.end()) {
        QMessageBox::warning(0, "Warning", "Unrecognized configuration field: "
                             + QString(field.c_str()) + ", at 311");
        return;
    }
    switch (it->second)
    {
    case ConfigField::COLOR_DEPTH:
        disableUIComboBox(mTemplateUI->colorDepth, "16 bit");
        break;

    case ConfigField::DISPLAY_FREQ:
        disableUIComboBox(mTemplateUI->displayFreq, "60 Hz");
        break;

    case ConfigField::ANTI_ALISAING:
        disableUIComboBox(mTemplateUI->antiAliasing, "0");
        break;

    case ConfigField::VERT_SYNC:
        disableUIComboBox(mTemplateUI->antiAliasing, "0");
        break;

    case ConfigField::GAMMA_CORR:
        disableUIComboBox(mTemplateUI->gammaCorrection, "No");
        break;

    case ConfigField::DISPLAY_RES:
        disableUIComboBox(mTemplateUI->displayRes);
        break;

    default:
        QMessageBox::warning(0, "Warning", "Unrecognized configuration field: "
                             + QString(field.c_str()) + ", at 342");
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::fillUIComboBox(QComboBox* field,
                                      const Ogre::StringVector& values,
                                      const char* current)
{
    ASSERT(field);
    field->clear();
    for (int i=0 ; i < values.size() ; i++) {
        field->addItem(values[i].c_str());
    }
    // Try to set "current" text as default, if given.
    if (strnlen(current,80) > 0) {
        int index = field->findText(current, Qt::MatchExactly);
        if (index >= 0)
            field->setCurrentIndex(index);
    } else {
        field->setCurrentIndex(0);
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::disableUIComboBox(QComboBox* field,
                                         const char* fixed)
{
    ASSERT(field);
    field->clear();
    field->addItem(fixed);
    field->setEnabled(false);
}


///////////////////////////////////////////////////////////////////////////////
bool
CbaZombieConfigDialog::restoreOpenALConfig()
{
    debugERROR("TODO\n");  // TODO
    return true;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::setRenderSystem(const QString& rs)
{
    if (rs.size()==0)
        return;
    // Get render by name
    Ogre::RenderSystem* render = mOgreRoot->getRenderSystemByName(rs.toStdString());
    if (!render) {
        QMessageBox::critical(0, "Warning", "Couldn't open \"" + rs +
                              "\" graphics render system");
    } else {
        // Set this render system as current
        int index = mTemplateUI->renderSystem->findText(rs, Qt::MatchExactly);
        ASSERT(index>=0);
        mTemplateUI->renderSystem->setCurrentIndex(index);
        // Enable corresponding options
        reconRendererOptions(*render);
    }
}


///////////////////////////////////////////////////////////////////////////////
bool
CbaZombieConfigDialog::saveConfig()
{
    debugERROR("TODO\n");  // TODO
    return true;
}

}

