/*
 * ConfigAssetGUI.h
 *
 *  Created on: Jan 13, 2014
 *      Author: agustin
 */

#ifndef CONFIGASSETGUI_H_
#define CONFIGASSETGUI_H_


#include <QtGui/QCheckBox>
#include <QtGui/QLabel>

#include <asset/Asset.h>
#include <debug/DebugUtil.h>
#include <qt_tools/ogre_widget/OgreWidget.h>

#include "ui_ConfigAsset.h"

namespace tool {

class ConfigAssetGUI : public QWidget
{
    Q_OBJECT
public:
    ConfigAssetGUI(QWidget* parent);
    virtual
    ~ConfigAssetGUI();

    // @brief Set the ogre data to be used by this class
    // @param od        The ogre data to be used
    //
    inline void
    setOgreData(const OgreWidget::OgreData& od);

    // @brief Set the asset to be configured
    // @param asset     The asset to be configured and modified.
    //
    void
    setAssetToConfigure(core::Asset* asset);


signals:

    // @brief Signal emited when the save button is pressed.
    //
    void
    onSaveButtonPressedSig(void);

public slots:
    void
    onSaveButtonClicked(bool){emit onSaveButtonPressedSig();};
    void
    onMeshBtnClicked(bool);
    void
    onColl2DBtnClicked(bool);
    void
    onPhysicsBtnClicked(bool);

    void
    onGraphicRepClicked(int);
    void
    onColl2DRepClicked(int);
    void
    onPhysicRepClicked(int);

private:
    Ui::ConfigAsset ui;

    OgreWidget::OgreData mOgreData;
    core::Asset* mCurrentAsset;
};












////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
ConfigAssetGUI::setOgreData(const OgreWidget::OgreData& od)
{
    mOgreData = od;
}

}

#endif /* CONFIGASSETGUI_H_ */
