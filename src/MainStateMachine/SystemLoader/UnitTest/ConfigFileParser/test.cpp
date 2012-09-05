/*
 * test.cpp
 *
 *  Created on: 06/06/2012
 *      Author: agustin
 */

#include <iostream>
#include <string>

#include "DebugUtil.h"
#include "ConfigFileParser.h"



int main(int argc, char **argv)
{
	ConfigFileParser cfp;

	if(!cfp.parseFile("config.test")){
		debugRED("Error parseando el test\n");
	}
	cfp.print();


	return 0;
}
