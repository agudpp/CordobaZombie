/**
 * Module: ButtonHelper.h
 * Author: Raul
 * Date: 7 Nov 2012
 */


#include "XMLHelper.h"
#include "tinyxml.h"
#include "MenuButtonEffect.h"


namespace CbBHelper{

/**
 * Este los crea a los botoncitos. Carlox me adelanto a tu queja por los
 * comentarios de mierda que estoy haciendo y te contesto que te podes ir a
 * hacerte cu... hasta que la mona descubra quien se a ha tomado todo el vino
 * :P con todo respeto te lo digo.
 */


void
parseCbMenuButton(const TiXmlElement *xml,
		OvEff::MenuButtonEffect &button);


/**
 * Este llama varias veces al de arriba para crear los botoncitos y meterlos en
 * 'buttons'
 */
void
buildButtons(const TiXmlElement root,
             const std::vector<Ogre::String> &names,
			 std::vector<OvEff::MenuButtonEffect> &buttons);



}
