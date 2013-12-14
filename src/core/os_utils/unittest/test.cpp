/*
 * test.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: agustin
 */

#include <OgreVector3.h>

#include <algorithm>
#include <vector>
#include <cmath>
#include <set>
#include <fstream>

#include <UnitTest++/UnitTest++.h>
#include <UnitTest++/XmlTestReporter.h>

#include <os_utils/OSHelper.h>
#include <math/FloatComp.h>
#include <types/basics.h>
#include <types/StackVector.h>
#include <types/StackQueue.h>
#include <types/StackPriorityQueue.h>
#include <types/BoolCountingMask.h>

using namespace core;

typedef StackVector<int, 512> SV512;
typedef StackQueue<int, 512> SQ512;
typedef StackPriorityQueue<int, 512> SPQ512;



TEST(Check_addEndPathVar)
{
    std::string path = "p";
    OSHelper::addEndPathVar(path);
    CHECK(path.size() == 2);
    CHECK(path[1] == '\\' || path[1] == '/');

    path = "p/";
    OSHelper::addEndPathVar(path);
    CHECK(path.size() == 2);
    CHECK(path[1] == '\\' || path[1] == '/');

    path = "p\\";
    OSHelper::addEndPathVar(path);
    CHECK(path.size() == 2);
    CHECK(path[1] == '\\' || path[1] == '/');
}

TEST(Check_extractFileName)
{
    // check normal filename
    std::string path = "path/filename.ext";
    std::string fname;

    CHECK(OSHelper::extractFileName(path, fname));
    CHECK_EQUAL("filename.ext", fname);

    path = "path\\filename.ext";
    CHECK(OSHelper::extractFileName(path, fname));
    CHECK_EQUAL("filename.ext", fname);

    // check with a multiple extensions
    path = "path\\filename.ext1.ext2.ext3";
    CHECK(OSHelper::extractFileName(path, fname));
    CHECK_EQUAL("filename.ext1.ext2.ext3", fname);

    // extract filename from a filename only (without path)
    path = "filename.ext1.ext2.ext3";
    CHECK(OSHelper::extractFileName(path, fname));
    CHECK_EQUAL("filename.ext1.ext2.ext3", fname);

    // extract a filename without extension
    path = "filename";
    CHECK(OSHelper::extractFileName(path, fname));
    CHECK_EQUAL("filename", fname);
}

TEST(Check_extractFileExtension)
{
    // check normal filename
    std::string path = "filename.ext";
    std::string fname;
    std::string ext;

    CHECK(OSHelper::extractFileExtension(path, fname, ext));
    CHECK_EQUAL("filename", fname);
    CHECK_EQUAL("ext", ext);

    // check with a multiple extensions
    path = "filename.ext1.ext2.ext3";
    CHECK(OSHelper::extractFileExtension(path, fname, ext));
    CHECK_EQUAL("filename.ext1.ext2", fname);
    CHECK_EQUAL("ext3", ext);

    // extract a filename without extension
    path = "filename";
    CHECK(OSHelper::extractFileExtension(path, fname, ext));
    CHECK_EQUAL("filename", fname);
    CHECK_EQUAL("", ext);
}

TEST(Check_extractPath)
{
    std::string fullPath = "/p1/p2/p3/filename.ext";
    std::string path;

    CHECK(OSHelper::extractPath(fullPath, path));
    CHECK_EQUAL("/p1/p2/p3/", path);

    fullPath = "/p1/p2\\p3\\filename.ext";
    CHECK(OSHelper::extractPath(fullPath, path));
    CHECK_EQUAL("/p1/p2\\p3\\", path);

    fullPath = "/p1/p2\\p3\\filename";
    CHECK(OSHelper::extractPath(fullPath, path));
    CHECK_EQUAL("/p1/p2\\p3\\", path);

    fullPath = "/p1/p2\\p3\\";
    CHECK(OSHelper::extractPath(fullPath, path));
    CHECK_EQUAL("/p1/p2\\p3\\", path);

    fullPath = "/p1/p2\\p3/";
    CHECK(OSHelper::extractPath(fullPath, path));
    CHECK_EQUAL("/p1/p2\\p3/", path);
}


int
main(int c, char* args[])
{
    return UnitTest::RunAllTests();
}



