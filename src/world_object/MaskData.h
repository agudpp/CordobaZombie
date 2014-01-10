/*
 * MaskData.h
 *
 *  Created on: Jan 8, 2014
 *      Author: agustin
 */

#ifndef MASKDATA_H_
#define MASKDATA_H_

namespace w_o {

struct MaskData
{
    unsigned int groupMask;
    unsigned int filterMask;

    MaskData() : groupMask(~0), filterMask(~0)
    {}
};

} /* namespace w_o */
#endif /* MASKDATA_H_ */
