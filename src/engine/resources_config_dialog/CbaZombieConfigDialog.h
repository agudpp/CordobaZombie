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
#include <QWidget>

namespace Ogre {
    class Root;
}

namespace Ui {
	class CbaZombieConfigDialog;
}

namespace core
{

class CbaZombieConfigDialog : public Ogre::ConfigDialog
{
public:
    CbaZombieConfigDialog();
    virtual ~CbaZombieConfigDialog();

    /**
     * @brief Display window for choosing system settings
     * @returns true if the user clicked 'Ok', false otherwise
     */
    bool
    showConfigDialog(Ogre::Root* root);

    /**
     * @brief Display window for choosing system settings
     * @returns true if the user clicked 'Ok', false otherwise
     */
    bool
    showConfigDialog(int argc, char** argv);

    /**
     * @brief Display window for choosing system settings
     * @returns true if the user clicked 'Ok', false otherwise
     */
    bool
    showConfigDialog(void);

private:
    QWidget* mWidget;
    Ui::CbaZombieConfigDialog* mTemplateUI;
};

}  // namespace core


#endif /* CBAZOMBIECOFIGDIALOG_H_ */
