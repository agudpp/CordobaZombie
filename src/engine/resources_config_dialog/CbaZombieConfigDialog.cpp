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
const std::map<std::string, CbaZombieConfigDialog::ConfigFieldCode>
CbaZombieConfigDialog::sOgreConfigField = {
    {"Color Depth", CbaZombieConfigDialog::ConfigFieldCode::COLOR_DEPTH},
    {"Display Frequency", CbaZombieConfigDialog::ConfigFieldCode::DISPLAY_FREQ},
    {"FSAA", CbaZombieConfigDialog::ConfigFieldCode::ANTI_ALISAING},
    {"VSync", CbaZombieConfigDialog::ConfigFieldCode::VERT_SYNC},
    {"sRGB Gamma Conversion", CbaZombieConfigDialog::ConfigFieldCode::GAMMA_CORR},
    {"Video Mode", CbaZombieConfigDialog::ConfigFieldCode::DISPLAY_RES},
};


///////////////////////////////////////////////////////////////////////////////
CbaZombieConfigDialog::CbaZombieConfigDialog(QWidget* parent) :
    QWidget(parent)
,   mOpenALdev(0)
,   mOpenALctx(0)
,   mOgreCfgFile("")
,   mOgrePluginsCfgFile("")
,   mOpenALCfgFile("")
,   mSoundDevice("")
{
    // Define internal structures
    mOgreRoot = new Ogre::Root();
    mTemplateUI = new Ui::CbaZombieConfigDialog();
    for (int i = static_cast<int>(ConfigFieldCode::RENDER_SYSTEM+1) ;
              i < static_cast<int>(ConfigFieldCode::SOUND_DEV) ;
              i++) {
        mOgreConfigFieldValue[static_cast<ConfigFieldCode>(i)] =
            std::make_pair(false,"");
    }
    // Connect with UI
    mTemplateUI->setupUi(this);
    connect(mTemplateUI->renderSystem,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(setRenderSystem(const QString&)));
    connect(mTemplateUI->colorDepth,
            SIGNAL(currentIndexChanged(int)),
            this,
            SLOT(setColorDepth(const QString&)));
    connect(mTemplateUI->displayFreq,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(setDisplayFrequency(const QString&)));
    connect(mTemplateUI->antiAliasing,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(setAntiAliasing(const QString&)));
    connect(mTemplateUI->vertSync,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(setVerticalSync(const QString&)));
    connect(mTemplateUI->gammaCorrection,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(setGammaCorrection(const QString&)));
    connect(mTemplateUI->displayRes,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(setDisplayResolution(const QString&)));
    connect(mTemplateUI->soundDevice,
            SIGNAL(currentIndexChanged(const QString&)),
            this,
            SLOT(setSoundDevice(const QString &)));
    connect(mTemplateUI->saveAndExit,
            SIGNAL(clicked()),
            this,
            SLOT(saveConfig()));
    connect(mTemplateUI->discardAndExit,
            SIGNAL(clicked()),
            this,
            SLOT(close()));
}

///////////////////////////////////////////////////////////////////////////////
CbaZombieConfigDialog::~CbaZombieConfigDialog()
{
    if (mTemplateUI)
        delete mTemplateUI;
    if (mOgreRoot)
        delete mOgreRoot;
}


///////////////////////////////////////////////////////////////////////////////
bool
CbaZombieConfigDialog::init(const char* engineConfigFilename)
{
    EngineConfiguration engineCfg;

    if (!engineConfigFilename) {
        QMessageBox::critical(0, "Error",
                     "Null engine configuration filename passed.");
        return false;
    }

    bool parseOK = engineCfg.load(engineConfigFilename);
    if (!parseOK) {
        QMessageBox::critical(0, "Error",
                     "Failed to load engine configuration file.");
        return false;
    }

    // Retrieve Ogre and OpenAL configuration filenames
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
    parseOK = engineCfg.getValue(EC_MODULE_OPENAL, EC_OPENAL_DEVICECFG, mOpenALCfgFile);
    if (!parseOK) {
        QMessageBox::critical(0, "Error",
                     "Failed to get OpenAL configuration filename.");
        return false;
    }

    // Parse config files and fill in UI
    parseOK = reconSystemRenderers();
    if (parseOK) {
        parseOK = restoreOgreConfig();
        parseOK = parseOK && restoreOpenALConfig();
    }

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
        cfg.load(mOgreCfgFile, "\t:=", false);
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
        assert(index>=0);
        mTemplateUI->renderSystem->setCurrentIndex(index);
        // Bring back previously saved render configuration
        Ogre::ConfigFile::SettingsMultiMap::const_iterator i;
        for (i = settings.begin(); i != settings.end(); ++i) {
            rs->setConfigOption(i->first, i->second);
        }
        // Enable corresponding options and stop
        reconRendererOptions(*rs);
        parseOK = true;
    }

    return parseOK;
}


///////////////////////////////////////////////////////////////////////////////
bool
CbaZombieConfigDialog::restoreOpenALConfig()
{

    QMessageBox::information(0, "TODO!!!", "restoreOpenALConfig");
    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool
CbaZombieConfigDialog::reconSystemRenderers()
{
    if (!mOgreRoot) {
        mOgreRoot = new Ogre::Root();  // We shouldn't need to be doing this!
        if (!mOgreRoot) {
            QMessageBox::critical(0, "Error", "Internal error: couldn't "
                                 "create an Ogre::Root element.");
            return false;
        }
    }
    Ogre::RenderSystemList rendersList = mOgreRoot->getAvailableRenderers();
    if (rendersList.empty()) {
        QMessageBox::warning(0, "Warning",
                             "No available graphics render systems detected");
        return false;
    }
    mTemplateUI->renderSystem->clear();
    for (int i=0 ; i < rendersList.size() ; i++) {
        if (rendersList[i])
            mTemplateUI->renderSystem->addItem(rendersList[i]->getName().c_str());
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::reconRendererOptions(Ogre::RenderSystem& render)
{
    const Ogre::ConfigOptionMap opts = render.getConfigOptions();
    for (auto it = sOgreConfigField.begin() ; it != sOgreConfigField.end() ;
              it++) {
        Ogre::ConfigOptionMap::const_iterator field = opts.find(it->first);
        if (field == opts.end()) {
            // Register as unavailable, and disable in UI
            mOgreConfigFieldValue[it->second] = std::make_pair(false, "");
            disableRendererOptions(it->second);
        } else {
            // Register as available, and load values in UI
            mOgreConfigFieldValue[it->second] =
                std::make_pair(true, field->second.currentValue);
            fillRendererOptions(field->second);
        }
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::fillRendererOptions(const Ogre::ConfigOption& opt)
{
    auto it = sOgreConfigField.find(opt.name);
    if (it == sOgreConfigField.end()) {
        QMessageBox::warning(0, "Warning", "Unrecognized configuration field: "
                             + QString(opt.name.c_str()));
        return;
    }
    switch (it->second)
    {
    case ConfigFieldCode::COLOR_DEPTH:
        fillUIComboBox(mTemplateUI->colorDepth,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigFieldCode::DISPLAY_FREQ:
        fillUIComboBox(mTemplateUI->displayFreq,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigFieldCode::ANTI_ALISAING:
        fillUIComboBox(mTemplateUI->antiAliasing,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigFieldCode::VERT_SYNC:
        fillUIComboBox(mTemplateUI->vertSync,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigFieldCode::GAMMA_CORR:
        fillUIComboBox(mTemplateUI->gammaCorrection,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    case ConfigFieldCode::DISPLAY_RES:
        fillUIComboBox(mTemplateUI->displayRes,
                       opt.possibleValues,
                       opt.currentValue.c_str());
        break;

    default:
        QMessageBox::warning(0, "Warning", "Unrecognized configuration field: "
                             + QString(opt.name.c_str()));
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::disableRendererOptions(ConfigFieldCode field)
{
    switch (field)
    {
    case ConfigFieldCode::COLOR_DEPTH:
        disableUIComboBox(mTemplateUI->colorDepth, "16 bit");
        break;

    case ConfigFieldCode::DISPLAY_FREQ:
        disableUIComboBox(mTemplateUI->displayFreq, "60 Hz");
        break;

    case ConfigFieldCode::ANTI_ALISAING:
        disableUIComboBox(mTemplateUI->antiAliasing, "0");
        break;

    case ConfigFieldCode::VERT_SYNC:
        disableUIComboBox(mTemplateUI->antiAliasing, "0");
        break;

    case ConfigFieldCode::GAMMA_CORR:
        disableUIComboBox(mTemplateUI->gammaCorrection, "No");
        break;

    case ConfigFieldCode::DISPLAY_RES:
        disableUIComboBox(mTemplateUI->displayRes);
        break;

    default:
        QMessageBox::warning(0, "Warning", "Unrecognized configuration field: "
                             + QString(field));
        break;
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::fillUIComboBox(QComboBox* field,
                                      const Ogre::StringVector& values,
                                      const char* current)
{
    assert(field);
    field->clear();
    for (int i=0 ; i < values.size() ; i++)
        field->addItem(values[i].c_str());
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
    assert(field);
    field->clear();
    field->addItem(fixed);
    field->setEnabled(false);
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
        assert(index>=0);
        mTemplateUI->renderSystem->setCurrentIndex(index);
        // Enable corresponding options
        reconRendererOptions(*render);
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::saveConfig()
{
    // Check selected graphics renderer is valid
    Ogre::RenderSystem* rs = mOgreRoot->getRenderSystemByName(
        mTemplateUI->renderSystem->currentText().toStdString());
    if (!rs) {
        QMessageBox::critical(0, "Error", "Bad render system, "
                              "discarding configuration changes.");
        close();
    }
    // Check all options are valid for selected graphics renderer
    for (auto it = sOgreConfigField.begin() ; it != sOgreConfigField.end() ;
               it++) {
        if (!mOgreConfigFieldValue[it->second].first)
            continue;  // Field not available for this render system
        rs->setConfigOption(it->first, mOgreConfigFieldValue[it->second].second);
    }
    Ogre::String err = rs->validateConfigOptions();
    if (!err.empty()) {
        QMessageBox::critical(0, "Error", "Bad graphics renderer settings, "
                              "discarding configuration changes.");
        close();
    }
    // Save and exit
    mOgreRoot->setRenderSystem(rs);
    mOgreRoot->saveConfig();
    close();
}

}

