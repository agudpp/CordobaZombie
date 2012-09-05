/*
 * BpItemAdditionalInfo.h
 *
 *  Created on: 05/06/2012
 *      Author: agustin
 */

#ifndef BPITEMADDITIONALINFO_H_
#define BPITEMADDITIONALINFO_H_

#include <OgreString.h>


class BpItemAdditionalInfo
{
public:
	inline BpItemAdditionalInfo(){};
	inline BpItemAdditionalInfo(const BpItemAdditionalInfo &);
	inline ~BpItemAdditionalInfo(){};

	/**
	 * Set the name of the image to shown (image = info)...
	 * TODO: probablemente querramos tener tanto un string como una imagen,
	 * en vez de tener todo en una imagen grande... por ahora mas simple
	 * dejarle el trabajo a los modeladores que son muchos xD
	 */
	inline void setMaterialName(const Ogre::String &matName);
	inline const Ogre::String &getMaterial(void) const;

private:
	Ogre::String 		mMatName;

};


inline BpItemAdditionalInfo::BpItemAdditionalInfo(const BpItemAdditionalInfo &other)
{
	mMatName = other.mMatName;
}


inline void BpItemAdditionalInfo::setMaterialName(const Ogre::String &matName)
{
	mMatName = matName;
}

inline const Ogre::String &BpItemAdditionalInfo::getMaterial(void) const
{
	return mMatName;
}


#endif /* BPITEMADDITIONALINFO_H_ */
