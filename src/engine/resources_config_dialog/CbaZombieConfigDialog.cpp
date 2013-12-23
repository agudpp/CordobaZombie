/*
 * CbaZombieConfigDialog.cpp
 *
 *   Created on: Dec 4, 2013
 *      Company: CordobaZombie
 *       Author: Carlos E. Budde
 *      License: GPLv3
 */

#include <QWidget>
#include <QPainter>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QStyleOption>

#include <OgreRoot.h>
#include <OgreString.h>
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreConfigOptionMap.h>
#include <OgreRenderSystem.h>

#include <AL/al.h>
#include <AL/alc.h>

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
            SIGNAL(currentIndexChanged(const QString&)),
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
    bool parseOK(false);
    // Read EngineConfiguration file
    if (!engineConfigFilename) {
        QMessageBox::critical(0, "Error",
                     "Null engine configuration filename passed.");
        return false;
    }
    parseOK = mEngineConfig.load(engineConfigFilename);
    if (!parseOK) {
        QMessageBox::critical(0, "Error",
                     "Failed to load engine configuration file.");
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
    bool parseOK(false);
    std::string ogreCfgFilename;
    std::string ogrePluginsFilename;
    Ogre::ConfigFile ogreCfg;

    // Retrieve ogre configuration filenames from EngineConfiguration file
    parseOK = mEngineConfig.getValue(EC_MODULE_OGRE, EC_OGRE_OGRECFG,
                                      ogreCfgFilename);
    if (!parseOK) {
        QMessageBox::critical(0, "Error",
                     "Failed to get Ogre configuration filename.");
        return false;
    }
    parseOK = mEngineConfig.getValue(EC_MODULE_OGRE, EC_OGRE_PLUGINSCFG,
                                      ogrePluginsFilename);
    if (!parseOK) {
        QMessageBox::critical(0, "Error",
                     "Failed to get Ogre plugins configuration filename.");
        return false;
    }

    // Check files exist, and try to make Ogre parse them
    if (ogreCfgFilename.empty() || ogrePluginsFilename.empty()) {
        QMessageBox::critical(0, "Error",
                              "Ogre config files have not yet been set.");
        return false;
    }
    try {
        ogreCfg.load(ogreCfgFilename, "\t:=", false);
    } catch (Ogre::Exception& e) {
        if (e.getNumber() == Ogre::Exception::ERR_FILE_NOT_FOUND) {
            QMessageBox::critical(0, "Error", QString("Ogre config file \"") +
                                  ogreCfgFilename.c_str() + "\" not found");
            return false;
        } else {
            throw(e);
        }
    }

    // Try to read parsed info into our structure
    parseOK = false;
    Ogre::ConfigFile::SectionIterator iSection = ogreCfg.getSectionIterator();
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
    int pos = 0, MAX_POS = 1024;
    const ALCchar* device  = 0;
    const ALCchar* devices = 0;
    Ogre::StringVector devNames;

    // Make OpenAL identify all available sound devices.
    if (!alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT")) {
        QMessageBox::critical(0, "Error", "OpenAL can't retrieve sound "
                             "devices information");
        return false;
    }
    devNames.clear();
    devNames.reserve(10);
    devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
    if (devices == 0) {
        QMessageBox::critical(0, "Error", "OpenAL didn't find any sound device");
        return false;
    }

    // Parse OpenAL output: "devices" contains the device names,
    // separated by NULL and terminated by two consecutive NULLs.
    pos = 0;
    do {
        device = &(devices[pos]);
        if (*device != '\0') {
            devNames.push_back(device);
            pos += strlen(device)+1;
        }
    } while (*device != '\0' && pos < MAX_POS);

    if (pos >= MAX_POS) {
        QMessageBox::information(0, "Warning", "There may be some sound "
                                 "devices which we could not find.");
    }

    // Check if there was some previously chosen sound device
    mEngineConfig.getValue(EC_MODULE_OPENAL, EC_OPENAL_DEVICECFG, mSoundDevice);
    // Load found values in UI
    fillUIComboBox(mTemplateUI->soundDevice, devNames, mSoundDevice.c_str());

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
        disableUIComboBox(mTemplateUI->vertSync, "No");
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
    if (std::strlen(current) > 0) {
        int index = field->findText(current, Qt::MatchExactly);
        if (index >= 0)
            field->setCurrentIndex(index);
    } else {
        field->setCurrentIndex(0);
    }
    field->setEnabled(true);
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
CbaZombieConfigDialog::refreshFrequencyValues(const QString& newRes)
{
    // Inform the renderer of the new resolution chosen
    Ogre::RenderSystem* rs = mOgreRoot->getRenderSystemByName(
        mTemplateUI->renderSystem->currentText().toStdString());
    if (!rs)
        return;  // Bad render system, nothing can be done
    rs->setConfigOption("Video Mode", newRes.toStdString());
    // Check which frequencies are available
    Ogre::ConfigOptionMap opts = rs->getConfigOptions();
    auto freq = opts.find("Display Frequency");
    if (freq == opts.end()) {
        // Field not found??? Disable in UI
        ConfigFieldCode code = sOgreConfigField.at(freq->first);
        mOgreConfigFieldValue[code] = std::make_pair(false, "");
        disableRendererOptions(code);
    } else {
        // Refresh UI with available frequencies for this resolution
        fillRendererOptions(freq->second);
    }
}


///////////////////////////////////////////////////////////////////////////////
void
CbaZombieConfigDialog::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
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
    // Register chosen sound device in the engine config file
    if (!mEngineConfig.setValue(EC_MODULE_OPENAL,
                                EC_OPENAL_DEVICECFG,
                                mSoundDevice)) {
        QMessageBox::critical(0, "Error", "Failed to save sound device change");
    }
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
    rs->setConfigOption("Full Screen", "Yes");  // Force fullscreen mode
    Ogre::String err = rs->validateConfigOptions();
    if (!err.empty()) {
        QMessageBox::critical(0, "Error", "Bad graphics renderer settings, "
                              "discarding configuration changes.");
        close();
    }
    // Save Ogre configuration and exit
    mOgreRoot->setRenderSystem(rs);
    mOgreRoot->saveConfig();
    close();
}

}

