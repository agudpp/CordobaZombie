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

#include <AL/al.h>
#include <AL/alc.h>
#include <OgreConfigOptionMap.h>

#include <debug/DebugUtil.h>
#include <EngineConfiguration.h>

// Forward declarations
namespace Ogre {
    class Root;
    class RenderSystem;
}
namespace Ui {
	class CbaZombieConfigDialog;
}
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

    // Configurable parameters
    typedef enum {
        RENDER_SYSTEM = 0,
        COLOR_DEPTH,
        DISPLAY_FREQ,
        ANTI_ALISAING,
        VERT_SYNC,
        GAMMA_CORR,
        DISPLAY_RES,
        SOUND_DEV,

        COUNT,
        INVALID
    } ConfigField;  // @remarks ConfigField must reflect the slot buttons

public:
    CbaZombieConfigDialog(QWidget* parent=0);
    virtual ~CbaZombieConfigDialog();

    /**
     * @brief Display window for choosing system settings
     * @returns true if the user clicked 'Ok' | false otherwise
     * TODO ERASE, DEPRECATED.
     */
    bool
    showConfigDialog(Ogre::Root* root);

    /**
     * @brief Recognize engine configuration options, and fill UI accordingly.
     * @remarks The "engineCfg" parameter should have an open config file.
     * @returns true if initialization was correct | false otherwise
     */
    bool
    init(const EngineConfiguration& engineCfg);

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
     */
    void
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
    reconRendererOptions(const Ogre::ConfigOption& field);

    /**
     * @brief Set passed options for specified QComboBox field
     * @remarks Previous content of matching QComboBox gets erased
     */
    void
    fillUIComboBox(QComboBox* field,
                   const Ogre::StringVector& values,
                   const char* current = "");

private slots:
    // @brief Set current ConfigField::RENDER_SYSTEM
    void setRenderSystem(const QString& rs);
    // @brief Set current ConfigField::COLOR_DEPTH
    inline void setColorDepth(const QString& cdepth);
    // @brief Set current ConfigField::DISPLAY_FREQ
    inline void setDisplayFrequency(const QString& freq);
    // @brief Set current ConfigField::ANTI_ALISAING
    inline void setAntiAliasing(const QString& aa);
    // @brief Set current ConfigField::VERT_SYNC
    inline void setVerticalSync(const QString& vsync);
    // @brief Set current ConfigField::GAMMA_CORR
    inline void setGammaCorrection(const QString& gc);
    // @brief Set current ConfigField::DISPLAY_RES
    inline void setDisplayResolution(const QString& dr);
    // @brief Set current ConfigField::SOUND_DEV
    inline void setSoundDevice(const QString& sd);

    /**
     * @brief Save current engine configuration (aka "system settings").
     */
    bool
    saveConfig();

private:
    Ui::CbaZombieConfigDialog* mTemplateUI;
    // Engines references and config filenames
    Ogre::Root* mOgreRoot;
    ALCdevice*  mOpenALdev;
    ALCcontext* mOpenALctx;
    std::string mOgreCfgFile;
    std::string mOgrePluginsCfgFile;
    std::string mOpenALCfgFile;
    // Configuration members (must map to ConfigField members)
    static const std::map<std::string,ConfigField> mOgreConfigField;
    std::string mRenderSystem;
    int mColorDepth;
    int mDisplayFreq;
    int mAntiAliasing;
    int mVertSync;
    bool mGammaCorrection;
    std::pair<int,int> mDisplayRes;
    std::string mSoundDevice;
};



///////////////////////////////////////////////////////////////////////////////
inline void CbaZombieConfigDialog::show() { QWidget::show(); }

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setColorDepth(const QString& cdepth)
{ ASSERT(cdepth.size()>0); mColorDepth = cdepth.toInt(); }

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setDisplayFrequency(const QString& freq)
{ ASSERT(freq.size()>0); mDisplayFreq = freq.toInt(); }

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setAntiAliasing(const QString& aa)
{ ASSERT(aa.size()>0); mAntiAliasing = aa.toInt(); }

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setVerticalSync(const QString& vsync)
{ ASSERT(vsync.size()>0); mVertSync = vsync.toInt(); }

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setGammaCorrection(const QString& gc)
{ ASSERT(gc.size()>0); mGammaCorrection = gc.toInt(); }

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setDisplayResolution(const QString& dr)
{
    ASSERT(dr.size()>0);
    QStringList tmp = dr.split("x");  // Expects "640x480" format
    ASSERT(tmp.size()==2);
    mDisplayRes.first  = tmp[0].toInt();
    mDisplayRes.second = tmp[1].toInt();
}

///////////////////////////////////////////////////////////////////////////////
inline void
CbaZombieConfigDialog::setSoundDevice(const QString& sd)
{ mSoundDevice = sd.toStdString(); }


}  // namespace engine



#endif /* CBAZOMBIECOFIGDIALOG_H_ */
