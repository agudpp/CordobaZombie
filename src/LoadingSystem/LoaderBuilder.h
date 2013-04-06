/*
 * LoaderBuilder.h
 *
 *  Created on: 28/10/2012
 *      Author: agustin
 *
 */

#ifndef LOADERBUILDER_H_
#define LOADERBUILDER_H_

#include <vector>

class Loader;

class LoaderBuilder {
public:
    /**
     * Build all the loaders
     */
    static void getLoaders(std::vector<Loader *> &result);
};

#endif /* LOADERBUILDER_H_ */
