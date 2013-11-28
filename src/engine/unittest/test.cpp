/*
 * test.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include <algorithm>
#include <vector>
#include <set>
#include <string>

#include <UnitTest++/UnitTest++.h>

#include <debug/DebugUtil.h>
#include <types/StackVector.h>
#include <os_utils/OSHelper.h>
#include <Engine.h>
#include <EngineConfiguration.h>


// Return the dropbox resource path
static std::string
getDropboxPath(void)
{
    char* ENV;
    if (!core::OSHelper::getEnvVar("CZ01_RC_PATH", ENV)) {
        return "";
    }
    std::string result = ENV;
    core::OSHelper::addEndPathVar(result);
    return result;
}


// Test the engine config
TEST(EngineConfigurationTest)
{
    std::string dropboxPath = getDropboxPath();
    CHECK(!dropboxPath.empty());

    dropboxPath  = dropboxPath + "test/unittest/engine/EngineConfig.xml";

    engine::EngineConfiguration config;
    CHECK(config.load(dropboxPath));

    // get some values and verify them
    std::string strVal;
    int intVal;
    float floatVal;

    CHECK(config.getValue("OgreSystemLoader", "resources", strVal));
    CHECK_EQUAL("resources.cfg", strVal);
    CHECK(config.getValue("OgreSystemLoader", "plugins", strVal));
    CHECK_EQUAL("plugins.cfg", strVal);
    CHECK(config.getValue("OgreSystemLoader", "ogreLogLevel", strVal));
    CHECK_EQUAL("High", strVal);

    CHECK(config.getValue("InputSystemLoader", "disableInputGrabbing", intVal));
    CHECK_EQUAL(1, intVal);

    // try to get something that is not there
    CHECK(!config.getValue("InputSystemLoader23112", "disableInputGrabbing", intVal));
    CHECK(!config.getValue("InputSystemLoader", "disableInputGrabbing21231", intVal));
}

TEST(EngineLoadingSystem)
{
    debugERROR("TODO\n");
    std::string dropboxPath = getDropboxPath();
    CHECK(!dropboxPath.empty());

    dropboxPath  = dropboxPath + "test/unittest/engine/EngineConfig.xml";


    engine::Engine engine;
    CHECK(engine.loadAll(dropboxPath));
}



int
main(void)
{
    return UnitTest::RunAllTests();
}



