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

#include <OgreConfigDialog.h>


// Forward declarations
namespace Ogre {
    class Root;
}
namespace Ui {
	class CbaZombieConfigDialog;
}
class QWidget;

namespace engine
{

class CbaZombieConfigDialog : public Ogre::ConfigDialog
{
public:
    CbaZombieConfigDialog();
    virtual ~CbaZombieConfigDialog();

    /**
     * @brief Display window for choosing system settings
     * @returns true if the user clicked 'Ok', false otherwise
     * @remarks DEPRECATED, erase
     */
    bool
    showConfigDialog(Ogre::Root* root);

    /**
     * @brief Display window for choosing system settings
     * @returns 0 on success, !0 on error
     */
    int
    show(Ogre::Root* root);

private:
    /**
     * @brief Identify the system settings to show in the config dialog
     */
    void
    systemRecon(Ogre::Root* root);

private:
    QWidget* mWidget;
    Ui::CbaZombieConfigDialog* mTemplateUI;
};

}  // namespace engine


#endif /* CBAZOMBIECOFIGDIALOG_H_ */
