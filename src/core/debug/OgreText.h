/* Extracted from http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Simple+text
 *
 * OgreText.h
 *
 *  Created on: Jun 9, 2013
 *      Author: agustin
 */

#ifndef OGRETEXT_H_
#define OGRETEXT_H_

#include <OgreTextAreaOverlayElement.h>
#include <OgreStringConverter.h>
#include <OgreOverlayManager.h>
#include <OgreOverlayContainer.h>

namespace core {

class OgreText
{
public:
    OgreText()
    {
        olm = Ogre::OverlayManager::getSingletonPtr();
        if (init == 0) {
            panel = static_cast<Ogre::OverlayContainer*>(olm->createOverlayElement(
                "Panel", "GUI"));
            panel->setMetricsMode(Ogre::GMM_PIXELS);
            panel->setPosition(0, 0);
            panel->setDimensions(1.0f, 1.0f);
            overlay = olm->create("GUI_OVERLAY");
            overlay->add2D(panel);
        }
        ++(this->init);
        szElement = "element_" + Ogre::StringConverter::toString(init);
        overlay = olm->getByName("GUI_OVERLAY");
        panel = static_cast<Ogre::OverlayContainer*>(olm->getOverlayElement("GUI"));
        textArea =
            static_cast<Ogre::TextAreaOverlayElement*>(olm->createOverlayElement(
                "TextArea", szElement));
        panel->addChild(textArea);
        overlay->show();
        setText("");
    }
    ~OgreText()
    {
        szElement = "element_" + Ogre::StringConverter::toString(init);
        olm->destroyOverlayElement(szElement);
        --(this->init);
        if (init == 0) {
            olm->destroyOverlayElement("GUI");
            olm->destroy("GUI_OVERLAY");
        }
    }
    void
    setText(const char *szString, const Ogre::Real charHeight = 0.03f)
    {
        textArea->setCaption(szString);
        textArea->setDimensions(1.0f, 1.0f);
        textArea->setMetricsMode(Ogre::GMM_RELATIVE);
        textArea->setFontName("BasicFont");
        textArea->setCharHeight(charHeight);
    }
    void
    setText(const Ogre::String& szString, const Ogre::Real charHeight = 0.03f) // now You can use Ogre::String as text
    {
        textArea->setCaption(szString);
        textArea->setDimensions(1.0f, 1.0f);
        textArea->setMetricsMode(Ogre::GMM_RELATIVE);
        textArea->setFontName("BasicFont");
        textArea->setCharHeight(charHeight);
    }
    void
    setPos(float x, float y)
    {
        textArea->setPosition(x, y);
    }
    void
    setColor(float R, float G, float B, float I)
    {
        textArea->setColour(Ogre::ColourValue(R, G, B, I));
    }
    void
    setColor(const Ogre::ColourValue& color)
    {
        textArea->setColour(color);
    }
private:
    Ogre::OverlayManager *olm;
    Ogre::OverlayContainer *panel;
    Ogre::Overlay *overlay;
    Ogre::TextAreaOverlayElement *textArea;
    static int init;
    Ogre::String szElement;
};

} /* namespace core */
#endif /* OGRETEXT_H_ */
