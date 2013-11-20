/*
 * test.cpp
 *
 *  Created on: Jun 21, 2013
 *      Author: agustin
 */

#include <algorithm>
#include <vector>

#include <UnitTest++/UnitTest++.h>

#include <debug/OgreCoreCreatorHelper.h>
#include <debug/DebugUtil.h>
#include <asset/AssetLoader.h>
#include <os_utils/OSHelper.h>

using namespace core;

static core::OgreCoreCreatorHelper ogre;



TEST(CheckAssetLoader)
{
//    CHECK_ASSERT(ogre.mSceneMgr != 0);

    // try to find asset_test and asset_test2
    AssetLoader loader;
    Asset a1,a2;
    std::string path, path2;

    CHECK(loader.loadAssetFromOgreResource("asset_test.mesh","Asset", a1));
    CHECK(loader.loadAssetFromOgreResource("asset_test2.mesh","Asset", a2));

    CHECK(core::OSHelper::extractPath(a1.fullAssetPath, path));
    CHECK(core::OSHelper::extractPath(a2.fullAssetPath, path2));

    core::OSHelper::addEndPathVar(path);
    core::OSHelper::addEndPathVar(path2);
    CHECK_EQUAL(path, path2);

    // check asset 1
    CHECK_EQUAL("asset_test", a1.name);
    CHECK_EQUAL("asset_test.mesh", a1.meshName);
    CHECK_EQUAL(AssetType::ASSET_STATIC_WORLD_ELEMENT, a1.type);
    CHECK_EQUAL(AssetMaterialType::ASSET_MAT_NONE, a1.materialType);
    CHECK_EQUAL(path + "asset_test_2dcol.mesh", a1.coll2DRepFile);
    CHECK_EQUAL("", a1.coll3DRepFile);

    // check asset 2
    CHECK_EQUAL("asset_test2", a2.name);
    CHECK_EQUAL("asset_test2.mesh", a2.meshName);
    CHECK_EQUAL(AssetType::ASSET_STATIC_WORLD_FLOOR, a2.type);
    CHECK_EQUAL(AssetMaterialType::ASSET_MAT_METAL, a2.materialType);
    CHECK_EQUAL(path2 + "asset_test2_2dcol.mesh", a2.coll2DRepFile);
    CHECK_EQUAL("", a2.coll3DRepFile);
}


int
main(void)
{
    if (!ogre.build() ||
        !ogre.loadConfigFromEnvVar("CZ01_RC_PATH", "resources_unit_test.cfg")) {
        debugERROR("Error loading or configuring Ogre\n");
        return -1;
    }
    return UnitTest::RunAllTests();
}
