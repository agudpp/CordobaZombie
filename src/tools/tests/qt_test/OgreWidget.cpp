
#include "OgreWidget.h"

#ifdef Q_WS_X11
  #include <QtGui/QX11Info>
#endif

OgreWidget::OgreWidget(QWidget *parent)
       : QWidget(parent)
       , mRoot(0)
       , mSceneManager(0)
       , mRenderWindow(0)
       , mViewport(0)
       , mCamera(0)
       , mInitialised(false)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoBackground);
    setMinimumSize(240, 240);
}

OgreWidget::~OgreWidget()
{
    mRoot->shutdown();
    if (mRenderWindow) {
        mRoot->detachRenderTarget(mRenderWindow);
        delete mRenderWindow;
        mRenderWindow = 0;
    }

    delete mRoot;
    destroy();
}

void OgreWidget::setBackgroundColour(QColor c)
{
    if (mViewport)
    {
        Ogre::ColourValue ogreColour;
        ogreColour.setAsARGB(c.rgba());
        mViewport->setBackgroundColour(ogreColour);
    }
}

QPaintEngine* OgreWidget::paintEngine() const
{
    return 0;
}

void OgreWidget::paintEvent(QPaintEvent* e)
{
   if (!mRoot)
   {
        initOgreSystem();
        return;
   }

    if (mRenderWindow && mInitialised) {
        mAnimState->addTime(mRenderWindow->getBestFPS()/10000);
        mRoot->_fireFrameStarted();
        mRoot->_fireFrameRenderingQueued();
        mRenderWindow->update();
        mRoot->_fireFrameEnded();
    }
    e->accept();
}

void OgreWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    if (e->isAccepted())
    {
        const QSize& newSize = e->size();

        if (mRenderWindow)
        {
             mRenderWindow->resize(newSize.width(), newSize.height());
             mRenderWindow->windowMovedOrResized();
        }
        if (mCamera)
        {
            using Ogre::Real;

            Real aspectRatio = Real(newSize.width()) / Real(newSize.height());
            mCamera->setAspectRatio(aspectRatio);
        }
    }
}


void OgreWidget::moveEvent(QMoveEvent* e)
{
     QWidget::moveEvent(e);

     if (e->isAccepted() && mRenderWindow)
     {
          mRenderWindow->windowMovedOrResized();
          update();
     }
}

void OgreWidget::initOgreSystem()
{

     mRoot = new Ogre::Root();

     Ogre::RenderSystem* rs = mRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
     assert(rs && "RenderSystem is NULL");

     mRoot->setRenderSystem(rs);
     mRoot->initialise(false);

     // Load resource paths from config file
     Ogre::ConfigFile cf;
     cf.load("resources_qt.cfg");

     // Go through all sections & settings in the file
     Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

     Ogre::String secName, typeName, archName;
     while (seci.hasMoreElements())
     {
          secName = seci.peekNextKey();
          Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
          Ogre::ConfigFile::SettingsMultiMap::iterator i;
          for (i = settings->begin(); i != settings->end(); ++i)
          {
              typeName = i->first;
              archName = i->second;
              Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                        archName, typeName, secName);
          }
     }

     mSceneManager = mRoot->createSceneManager(Ogre::ST_GENERIC);

     Ogre::NameValuePairList viewConfig;
     Ogre::String widgetHandle;

     viewConfig["externalWindowHandle"] =
  #ifdef Q_WS_WIN
       Ogre::StringConverter::toString((size_t)((HWND)winId()));
  #else
       Ogre::StringConverter::toString((unsigned long)winId());
  #endif
     mRenderWindow = mRoot->createRenderWindow("Ogre Rendering Window", width(), height(),
                        false, &viewConfig);


     Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
     mRenderWindow->setVisible(true);
     mRenderWindow->setActive(true);
     mRenderWindow->setAutoUpdated(false);

     mCamera = mSceneManager->createCamera("VisCam");
     mCamera->setPosition(Ogre::Vector3(0, 0, 500));
     mCamera->lookAt(Ogre::Vector3(0, 0, -300));
     mCamera->setNearClipDistance(5);

     mViewport = mRenderWindow->addViewport(mCamera);
     mViewport->setBackgroundColour(Ogre::ColourValue::Blue);
     mCamera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));

     Ogre::Entity *ent = mSceneManager->createEntity("Robot", "robot.mesh");
     Ogre::SceneNode *node = mSceneManager->getRootSceneNode()->createChildSceneNode("RobotNode");
     node->attachObject(ent);

     Ogre::SceneNode* headNode = mSceneManager->getRootSceneNode()->createChildSceneNode();
     Ogre::Entity* entity = mSceneManager->createEntity("ogrehead" , "ogrehead.mesh");
     headNode->attachObject(entity);

     mAnimState = ent->getAnimationState("Walk");
     mAnimState->setEnabled(true);
     mAnimState->setLoop(true);

     mInitialised = true;

     startTimer(5);
}

void OgreWidget::timerEvent(QTimerEvent *evt)
{
    Q_UNUSED(evt);
    update();
}
