/*
 * ConfigDialogTest.h
 *
 *  Created on: Dec 15, 2013
 *     Company: CordobaZombie
 *      Author: Budde, Carlos Esteban
 */

#ifndef CONFIGDIALOGTEST_H_
#define CONFIGDIALOGTEST_H_

#include <app_tester/AppTester.h>

// Forward declarations
namespace engine {
    class CbaZombieConfigDialog;
}

namespace tests {

class ConfigDialogTest : public core::AppTester
{
public:
    ConfigDialogTest();
    ~ConfigDialogTest();

    /**
     * @brief Load additional stuff before launching update cycle.
     */
    void
    loadAditionalData(void);

    /**
     * @brief Show the engine configuration window
     */
    int
    run(void);

private:
    engine::CbaZombieConfigDialog* mConfigDialog;
};

} // namespace tests

#endif /* CONFIGDIALOGTEST_H_ */
