/*
 * QtOgreAppBase.h
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#ifndef QTOGREAPPBASE_H_
#define QTOGREAPPBASE_H_

#include <QSet>
#include <QtGui/QWidget>

#include <qt_tools/qt_debug/QtDebug.h>

#include "OgreWidget.h"

namespace tool {

class QtOgreAppBase : public QWidget
{
    Q_OBJECT
public:

    // @brief Load the OgreWidget window and create it, using this base class
    //        as the parent one.
    // @param rh                The resource handler instance to be used
    // @param *configFiles      The config files for ogre
    //
    QtOgreAppBase(rrh::ResourceHandler* rh = 0,
                  const std::string& plugins = "plugins.cfg",
                  const std::string& ogre = "ogre.cfg",
                  const std::string& resource = "resources.cfg");
    virtual
    ~QtOgreAppBase();

    // @brief Return the OgreWidget instance
    //
    inline OgreWidget*
    ogreWidget(void) {return mOgreWidget;}
    inline const OgreWidget*
    ogreWidget(void) const {return mOgreWidget;}

    // @brief This method could be overloaded and used to load the inherited
    //        application
    // @return true on success | false otherwise
    //
    virtual bool
    loadApp(void) {return true;}

    // Loading resources functions helpers
    //

    // @brief Add a resource location from a filepath
    // @param filepath      The path of the resource file
    //
    void
    loadLocationFromFile(const QString& filepath);

    // @brief Add a resource file using our ResourceHandler system.
    // @param cfg       The file to be loaded (.cfg)
    // @return true on success | false otherwise
    //
    bool
    loadResourceFile(const QString& cfg);

protected slots:

    // @brief The child class should implement this if we want to receive this
    //        this event
    //
    virtual void
    frameUpdate(float fp){};

    // @brief This method will be called when we have all the systems ready
    //        since we don't know when Ogre is built (qt and ogre problems)
    //        this method will be called to advise that we have the systems ready.
    //
    virtual void
    systemsReady(void){};

protected:
    OgreWidget* mOgreWidget;
    QSet<QString> mLocations;
    rrh::ResourceHandler* mResourceHandler;

};

}

#endif /* QTOGREAPPBASE_H_ */
