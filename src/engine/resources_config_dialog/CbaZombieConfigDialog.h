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

#include <string>
#include <QWidget>
#if defined(_WIN32) || defined(CYGWIN)
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#elif defined(linux) || defined(_linux) || defined(__linux) || defined(__linux__)
#  include <AL/al.h>
#  include <AL/alc.h>
#else
#  error "Unsupported platform. ABORTING COMPILATION."
#endif

// Forward declarations
namespace Ogre {
    class Root;
}
namespace Ui {
	class CbaZombieConfigDialog;
}


namespace engine
{

class CbaZombieConfigDialog : public QWidget
{

    Q_OBJECT

public:
    CbaZombieConfigDialog(QWidget* parent=0);
    virtual ~CbaZombieConfigDialog();

    /**
     * @brief Display window for choosing system settings
     * @returns true if the user clicked 'Ok', false otherwise
     * TODO ERASE, DEPRECATED.
     */
    bool
    showConfigDialog(Ogre::Root* root);

    /**
     * @brief Restore saved engine configuration (aka "system settings") if any
     */
    bool
    restoreConfig();

    /**
     * @brief Display window for choosing system settings.
     */
    inline void
    show();

private slots:
    void setRenderSystem(const std::string& rs);
    void setDisplayFrequency(int freq);
    void setAntiAliasing(int aa);
    void setVerticalSync(int vsync);
    void setGammaCorrection(bool gc);
    void setDisplayResolution(const std::pair<int,int>& dr);
    void setSoundDevice(const std::string& sd);

    /**
     * @brief Identify the system settings to show in the config dialog.
     */
    void
    systemRecon();

    /**
     * @brief Save current engine configuration (aka "system settings").
     */
    bool
    saveConfig();

private:
    // References to engines
    Ogre::Root* mOgreRoot;
    ALCdevice*  mOpenALdev;
    ALCcontext* mOpenALctx;
    // Configuration members
    std::string mRenderSystem;
    int mDisplayFreq;
    int mAntiAliasing;
    int mVertSync;
    bool mGammaCorrection;
    std::pair<int,int> mDisplayRes;
    std::string mSoundDevice;
    Ui::CbaZombieConfigDialog* mTemplateUI;
};

inline void
CbaZombieConfigDialog::show()
{
    QWidget::show();
}

}  // namespace engine



#endif /* CBAZOMBIECOFIGDIALOG_H_ */
