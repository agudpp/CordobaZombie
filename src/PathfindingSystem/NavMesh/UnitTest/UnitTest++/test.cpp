/*
 * test++.cpp
 *
 *  Created on: Apr 11, 2013
 *      Author: agustin
 */


#include <string>
#include <fstream>

#include <UnitTest++/UnitTest++.h>

#include <PathfindingSystem/AbstractGraph/Graph.h>

// Helper methods
//

// Load a graph from a file
static bool
loadGraphFromFile(const std::string &fName,
                  Graph &graph)
{

}

TEST(FailSpectacularly)
{
    CHECK(false);
}

int main()
{
    return UnitTest::RunAllTests();
}

