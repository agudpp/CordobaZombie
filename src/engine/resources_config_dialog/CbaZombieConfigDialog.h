/*
 * CbaZombieConfigDialog.h
 *
 *   Created on: Dec 4, 2013
 *      Company: CordobaZombie
 *       Author: Carlos E. Budde
 *      License: GPLv3
 */
#ifndef CBAZOMBIECOFIGDIALOG_H_
#define CBAZOMBIECOFIGDIALOG_H_

#include <map>
#include <string>
#include <QWidget>
#include <QString>
#include <OgreConfigOptionMap.h>
#include <EngineConfiguration.h>

// Forward declarations
namespace Ogre {
    class Root;
    class RenderSystem;
}
namespace Ui {
	class CbaZombieConfigDialog;
}
class EngineConfiguration;
class QComboBox;


namespace engine
{

class CbaZombieConfigDialog : public QWidget
{

    Q_OBJECT

public:
    // Engine Configuration default attribute names (defined in source)
    static const char EC_MODULE_OGRE[30];
    static const char EC_OGRE_OGRECFG[30];
    static const char EC_OGRE_PLUGINSCFG[30];
    static const char EC_MODULE_OPENAL[30];
    static const char EC_OPENAL_DEVICECFG[30];

    // Configurable parameters (must reflect the slot/ui buttons)
    typedef enum {
        RENDER_SYSTEM =-1,
        COLOR_DEPTH   = 0,
        DISPLAY_FREQ  = 1,
        ANTI_ALISAING = 2,
        VERT_SYNC     = 3,
        GAMMA_CORR    = 4,
        DISPLAY_RES   = 5,
        SOUND_DEV     = 6,
        COUNT
    } ConfigFieldCode;

    // ConfigField format: < enabled? , value >
    typedef std::pair<bool,std::string> ConfigField;

public:
    CbaZombieConfigDialog(QWidget* parent=0);
    virtual ~CbaZombieConfigDialog();

    /**
     * @brief Recognize engine configuration options, and fill UI accordingly.
     * @remarks The "engineCfg" parameter should have an open config file.
     * @returns true if initialization was correct | false otherwise
     */
    bool
    init(const char *engineConfigFilename);

    /**
     * @brief Display UI.
     */
    inline void
    show();

private:
    /**
     * @brief Restore saved engine configuration (aka "system settings") if any
     * @returns true if previous options could be restored | false otherwise
     * @remarks Ogre config files must have been previously set
     */
    bool
    restoreOgreConfig();

    /**
     * @brief Restore saved engine configuration (aka "system settings") if any
     * @returns true if previous options could be restored | false otherwise
     * @remarks OpenAL config files must have been previously set
     */
    bool
    restoreOpenALConfig();

    /**
     * @brief Identify available graphic renderers, and show them on UI.
     * @returns true if at least one renderer was found | false otherwise
     */
    bool
    reconSystemRenderers();

    /**
     * @brief Identify all renderer available options, and show them on UI.
     */
    void
    reconRendererOptions(Ogre::RenderSystem& rSys);

    /**
     * @brief Identify renderer options for given field, and show them on UI.
     */
    void
    fillRendererOptions(const Ogre::ConfigOption& opt);

    /**
     * @brief Disable given field from UI.
     */
    void
    disableRendererOptions(ConfigFieldCode field);

    /**
     * @brief Set passed options in UI for specified QComboBox field
     * @remarks Previous content of matching QComboBox gets erased
     */
    void
    fillUIComboBox(QComboBox* field,
                   const Ogre::StringVector& values,
                   const char* current = "");

    /**
     * @brief Disable from UI specified QComboBox field
     * @remarks Disabled yet visible and "fixed" text can be given for showing.
     */
    void
    disableUIComboBox(QComboBox* field,
                      const char* fixed = "");

private slots:
    // @brief Set current render system name
    void setRenderSystem(const QString& rs);

    // @brief Set "value" for given ConfigField
    inline void setOgreConfigOption(ConfigFieldCode code, const QString& value);
    // @brief Set value for ConfigField::COLOR_DEPTH
    inline void setColorDepth(const QString&);
    // @brief Set value for ConfigField::DISPLAY_FREQ
    inline void setDisplayFrequency(const QString&);
    // @brief Set value for ConfigField::ANTI_ALISAING
    inline void setAntiAliasing(const QString&);
    // @brief Set value for ConfigField::VERT_SYNC
    inline void setVerticalSync(const QString&);
    // @brief Set value for ConfigField::GAMMA_CORR
    inline void setGammaCorrection(const QString&);
    // @brief Set value for ConfigField::DISPLAY_RES
    inline void setDisplayResolution(const QString&);

    // @brief Set current sound device name
    inline void setSoundDevice(const QString& sd);

    // @brief Save current engine configuration (aka "system settings").
    void saveConfig();

private:
    Ogre::Root* mOgreRoot;
    EngineConfiguration mEngineConfig;
    Ui::CbaZombieConfigDialog* mTemplateUI;
    // Configuration members (must map to ConfigField members)
    static const std::map<std::string, ConfigFieldCode> sOgreConfigField;
    std::map<ConfigFieldCode, ConfigField> mOgreConfigFieldValue;
    std::string mSoundDevice;
};



///////////////////////////////////////////////////////////////////////////////
inline void CbaZombieConfigDialog::show() { QWidget::show(); }

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setOgreConfigOption(
    CbaZombieConfigDialog::ConfigFieldCode code,
    const QString& value)
{
    if(value.size()>0)
        mOgreConfigFieldValue[code].second = value.toStdString();
}

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setColorDepth(const QString& val)
{
    if(val.size()>0)
        mOgreConfigFieldValue[ConfigFieldCode::COLOR_DEPTH].second =
            val.toStdString();
}

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setDisplayFrequency(const QString& val)
{
    if(val.size()>0)
        mOgreConfigFieldValue[ConfigFieldCode::DISPLAY_FREQ].second =
            val.toStdString();
}

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setAntiAliasing(const QString& val)
{
    if(val.size()>0)
        mOgreConfigFieldValue[ConfigFieldCode::ANTI_ALISAING].second =
            val.toStdString();
}

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setVerticalSync(const QString& val)
{
    if(val.size()>0)
        mOgreConfigFieldValue[ConfigFieldCode::VERT_SYNC].second =
            val.toStdString();
}

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setGammaCorrection(const QString& val)
{
    if(val.size()>0)
        mOgreConfigFieldValue[ConfigFieldCode::GAMMA_CORR].second =
            val.toStdString();
}

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setDisplayResolution(const QString& val)
{
    if(val.size()>0)
        mOgreConfigFieldValue[ConfigFieldCode::DISPLAY_RES].second =
            val.toStdString();
}


///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setSoundDevice(const QString& sd)
{ if (sd.size()>0) mSoundDevice = sd.toStdString(); }


}  // namespace engine



#endif /* CBAZOMBIECOFIGDIALOG_H_ */
