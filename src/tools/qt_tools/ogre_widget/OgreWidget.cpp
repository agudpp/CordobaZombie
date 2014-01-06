#include "OgreWidget.h"

#include <qt_tools/qt_debug/QtDebug.h>
#include <types/basics.h>


namespace tool {


////////////////////////////////////////////////////////////////////////////////
OgreWidget::OgreWidget(QWidget *parent,
                       rrh::ResourceHandler* rh) :
    QWidget(parent)
,   mInitialised(false)
,   mTimeStamp(0)
,   mBackgroundColor(0,0,0)
,   mResourceHandler(rh)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoBackground);
    setMinimumSize(240, 240);
}

////////////////////////////////////////////////////////////////////////////////
OgreWidget::~OgreWidget()
{
    if (mOgreData.root) {
        mOgreData.root->shutdown();
        if (mOgreData.renderWindow) {
            mOgreData.root->detachRenderTarget(mOgreData.renderWindow);
            delete mOgreData.renderWindow;
            mOgreData.renderWindow = 0;
        }

        delete mOgreData.root;
    }
    destroy();
}

////////////////////////////////////////////////////////////////////////////////
void
OgreWidget::setBackgroundColour(QColor c)
{
    mBackgroundColor = c;
}

////////////////////////////////////////////////////////////////////////////////
QPaintEngine*
OgreWidget::paintEngine() const
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
void
OgreWidget::paintEvent(QPaintEvent* e)
{
    if (!mInitialised) {
        if (!initSystems()) {
            QTDEBUG_CRITICAL("Error initializing the systems\n");
        }
        return;
    }
    ASSERT(mInitialised);
    ASSERT(mOgreData.renderWindow);

    const float currentTime = mTimer.getMilliseconds();
    mOgreData.frameTime = (mTimer.getMilliseconds() - mTimeStamp) * 0.001;
    mTimeStamp = currentTime;

    // emit the signal of frame update
    emit frameUpdate(mOgreData.frameTime);

    mOgreData.root->_fireFrameStarted();
    mOgreData.root->_fireFrameRenderingQueued();
    mOgreData.renderWindow->update();
    mOgreData.root->_fireFrameEnded();

    e->accept();
}

////////////////////////////////////////////////////////////////////////////////
void
OgreWidget::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);

    if (e->isAccepted()) {
        const QSize& newSize = e->size();

        if (mOgreData.renderWindow) {
            mOgreData.renderWindow->resize(newSize.width(), newSize.height());
            mOgreData.renderWindow->windowMovedOrResized();
        }
        if (mOgreData.camera) {
            using Ogre::Real;

            Real aspectRatio = Real(newSize.width()) / Real(newSize.height());
            mOgreData.camera->setAspectRatio(aspectRatio);
        }
        emit resizeSig(e);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
OgreWidget::moveEvent(QMoveEvent* e)
{
    QWidget::moveEvent(e);

    if (e->isAccepted() && mOgreData.renderWindow) {
        mOgreData.renderWindow->windowMovedOrResized();
        update();
    }
}

////////////////////////////////////////////////////////////////////////////////
void
OgreWidget::keyPressEvent(QKeyEvent* event)
{
    emit keyPressEventSig(event);
}
void
OgreWidget::keyReleaseEvent(QKeyEvent* event)
{
    emit keyReleaseEventSig(event);
}
void
OgreWidget::mousePressEvent(QMouseEvent* event)
{
    emit mousePressEventSig(event);
}
void
OgreWidget::mouseReleaseEvent(QMouseEvent* event)
{
    emit mouseReleaseEventSig(event);
}
void
OgreWidget::mouseMoveEvent(QMouseEvent* event)
{
    emit mouseMoveEventSig(event);
}

////////////////////////////////////////////////////////////////////////////////
bool
OgreWidget::initSystems(const std::string& plugins,
                        const std::string& ogre,
                        const std::string& resource)
{
    // check if the ogre system already exists
    if (mOgreData.root) {
        QTDEBUG_CRITICAL("The ogre system already exists!");
        return false;
    }

    mOgreData.root = new Ogre::Root(plugins, ogre, resource);

    Ogre::RenderSystem* rs =
        mOgreData.root->getRenderSystemByName("OpenGL Rendering Subsystem");

    if (rs == 0) {
        QTDEBUG_CRITICAL("OpenGL rendering subsystem wasn't found! We cannot do "
            "anything here... :(.\n");
        return false;
    }

    mOgreData.root->setRenderSystem(rs);
    mOgreData.root->initialise(false);


    // Load resource paths from config file
    if (!mResourceHandler) {
        QTDEBUG_WARNING("No ResourceHandler was set to load resources!\n");
    } else if (!resource.empty()){
        // load the resources
        rrh::ResourceGroup rg;
        rg.setOgreResourceFile(resource);
        if (!mResourceHandler->loadResourceGroup(rg)) {
            QTDEBUG_WARNING("Error loading resource file " << resource);
        }
    }


    mOgreData.sceneManager = mOgreData.root->createSceneManager(Ogre::ST_GENERIC);

    Ogre::NameValuePairList viewConfig;
    Ogre::String widgetHandle;

    viewConfig["externalWindowHandle"] =
#ifdef Q_WS_WIN
        Ogre::StringConverter::toString((core::size_t)((HWND)winId()));
#else
        Ogre::StringConverter::toString((unsigned long) winId());
#endif
    mOgreData.renderWindow = mOgreData.root->createRenderWindow("Ogre Rendering Window",
                                              width(),
                                              height(),
                                              false,
                                              &viewConfig);


    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    mOgreData.renderWindow->setVisible(true);
    mOgreData.renderWindow->setActive(true);
    mOgreData.renderWindow->setAutoUpdated(false);

    mOgreData.camera = mOgreData.sceneManager->createCamera("VisCam");
    mOgreData.camera->setPosition(Ogre::Vector3(0, 0, 500));
    mOgreData.camera->lookAt(Ogre::Vector3(0, 0, -300));
    mOgreData.camera->setNearClipDistance(5);

    mOgreData.viewport = mOgreData.renderWindow->addViewport(mOgreData.camera);
    mOgreData.viewport->setBackgroundColour(Ogre::ColourValue::Blue);
    if (mOgreData.root) {
        Ogre::ColourValue ogreColour;
        ogreColour.setAsARGB(mBackgroundColor.rgba());
        mOgreData.viewport->setBackgroundColour(ogreColour);
    }
    mOgreData.camera->setAspectRatio(Ogre::Real(width()) / Ogre::Real(height()));
    mInitialised = true;

    startTimer(5);

    emit systemsReady();

    return true;
}


////////////////////////////////////////////////////////////////////////////////
void
OgreWidget::timerEvent(QTimerEvent *evt)
{
    Q_UNUSED(evt);
    update();
}



}
