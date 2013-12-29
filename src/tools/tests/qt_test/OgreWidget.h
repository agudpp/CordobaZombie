#ifndef OGRE_WIDGET_H
#define OGRE_WIDGET_H


#include <OGRE/Ogre.h>
#include <QtGui>
#include <QVBoxLayout>
#include <QWidget>

#ifdef Q_WS_X11
    #include <QtGui/QX11Info>
#endif

class OgreWidget : public QWidget
{
    Q_OBJECT;

    protected:
      Ogre::Root *mRoot;
      Ogre::RenderWindow *mRenderWindow;
      Ogre::Camera *mCamera;
      Ogre::Viewport *mViewport;
      Ogre::SceneManager *mSceneManager;
      bool initialised;
      Ogre::AnimationState* mAnimState;
      bool mInitialised;

    public:
      OgreWidget(QWidget *parent=0);
      ~OgreWidget();
      void setBackgroundColour(QColor c);
      void initOgreSystem();

    protected:
      virtual void timerEvent(QTimerEvent *evt);
      QPaintEngine* paintEngine() const;
      virtual void paintEvent(QPaintEvent* e);
      virtual void resizeEvent(QResizeEvent* e);
      virtual void moveEvent(QMoveEvent* e);
};

#endif
