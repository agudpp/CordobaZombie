/*
 * This is the BillboardManager builder
 * BMBuilder.h
 *
 *  Created on: Apr 7, 2013
 *      Author: agustin
 */

#ifndef BMBUILDER_H_
#define BMBUILDER_H_


// Forward declarations
//
class TiXmlElement;

namespace billboard {
class BillboardManager;
}

namespace billboard {

class BMBuilder
{
public:
    /**
     * @brief Method that parse an xml and configure the given BillboardManager.
     * @param xml       The xml to parse the information (read the sample.xml)
     * @param manager   The BillboardManager instance to be configured with this
     *                  info. Note that all the old information of the manager
     *                  will be discarded
     * @returns         true on succcess | false otherwise
     */
    static bool
    configure(const TiXmlElement *xml, BillboardManager &manager);
};

} /* namespace billboard */
#endif /* BMBUILDER_H_ */
