#ifndef OGRE_WIDGET_H
#define OGRE_WIDGET_H

#include <string>
#include <vector>

#include <OGRE/Ogre.h>

#include <QtGui>
#include <QVBoxLayout>
#include <QWidget>

#ifdef Q_WS_X11
#include <QtGui/QX11Info>
#endif

#include <OIS.h>

#include <ResourceHandler.h>


namespace tool {

class OgreWidget : public QWidget
{
Q_OBJECT

public:
    struct OgreData {
        Ogre::Root *root;
        Ogre::RenderWindow *renderWindow;
        Ogre::Camera *camera;
        Ogre::Viewport *viewport;
        Ogre::SceneManager *sceneManager;
        float frameTime;

        OgreData() :
            root(0)
        ,   renderWindow(0)
        ,   camera(0)
        ,   viewport(0)
        ,   sceneManager(0)
        ,   frameTime(0)
        {}
    };


public:
    OgreWidget(QWidget *parent = 0,
               rrh::ResourceHandler* rh = 0);
    ~OgreWidget();

    // @brief Set the ResourceHandler to be used
    // @param rh        The resource handler instance to be used
    //
    inline void
    setResourceHandler(rrh::ResourceHandler* rh);

    // @brief Set the background color when repainting the screen
    //
    void
    setBackgroundColour(QColor c);

    // @brief Load the Ogre system | input system and all the associated systems
    //        to be used using this window (Widget) as main
    //        widget.
    // @param plugins   The plugins file.
    // @param ogre      The ogre file to be used
    // @param resources The resources file to load
    // @return true on success | false otherwise
    //
    bool
    initSystems(const std::string& plugins = "plugins.cfg",
                const std::string& ogre = "ogre.cfg",
                const std::string& resource = "resources.cfg");

    // @brief Get all the different ogre | input data.
    //
    inline OgreData&
    ogreData(void);
    inline const OgreData&
    ogreData(void) const;


signals:
    // @brief This signal will be emitted each frame and we will pass the frame
    //        time as argument.
    //        This will be triggered before the graphical update.
    //
    void
    frameUpdate(float timeFrame);

    // @brief This signal will be emitted when ogre system and all the other
    //        system are ready to be used
    //
    void
    systemsReady(void);

protected:

    // To be able to render into the widget we need to override this methods
    //
    virtual void
    timerEvent(QTimerEvent *evt);
    QPaintEngine*
    paintEngine() const;
    virtual void
    paintEvent(QPaintEvent* e);
    virtual void
    resizeEvent(QResizeEvent* e);
    virtual void
    moveEvent(QMoveEvent* e);

private:
    // Ogre stuff
    OgreData mOgreData;
    bool mInitialised;
    Ogre::Timer mTimer;
    float mTimeStamp;

    // Qt stuff
    QColor mBackgroundColor;

    // our stuff
    rrh::ResourceHandler* mResourceHandler;
};



////////////////////////////////////////////////////////////////////////////////
// Inline stuff
//

inline void
OgreWidget::setResourceHandler(rrh::ResourceHandler* rh)
{
    mResourceHandler = rh;
}

inline OgreWidget::OgreData&
OgreWidget::ogreData(void)
{
    return mOgreData;
}
inline const OgreWidget::OgreData&
OgreWidget::ogreData(void) const
{
    return mOgreData;
}

}

#endif
