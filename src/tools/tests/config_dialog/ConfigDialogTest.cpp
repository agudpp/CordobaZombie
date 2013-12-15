/*
 * ConfigDialogTest.cpp
 *
 *  Created on: Dec 15, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#include <resources_config_dialog/CbaZombieConfigDialog.h>
#include <tests/config_dialog/ConfigDialogTest.h>
#include <global_data/GlobalData.h>


namespace tests {

ConfigDialogTest::ConfigDialogTest() :
    core::AppTester(cz::GlobalData::lastTimeFrame)
,   mConfigDialog(0)
{
    // TODO Auto-generated constructor stub

}


ConfigDialogTest::~ConfigDialogTest()
{
    // TODO Auto-generated destructor stub
}


void
ConfigDialogTest::loadAditionalData()
{
    if (!mConfigDialog) {
        mConfigDialog = new engine::CbaZombieConfigDialog();
    }
}

int
ConfigDialogTest::run()
{
    ASSERT(mConfigDialog);
    ASSERT(mRoot);
    return mConfigDialog->show(mRoot);
}


} /* namespace tests */
