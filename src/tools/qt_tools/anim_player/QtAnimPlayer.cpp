/*
 * QtAnimPlayer.cpp
 *
 *  Created on: Dec 29, 2013
 *      Author: agustin
 */

#include "QtAnimPlayer.h"

#include <QFileDialog>
#include <QFileInfo>

#include <input/InputHelper.h>
#include <input/InputMouse.h>

#include <os_utils/OSHelper.h>



// Help stuff
//
namespace {

}




namespace tool {

////////////////////////////////////////////////////////////////////////////////
bool
QtAnimPlayer::loadMesh(const QString& meshName)
{
    ASSERT(ogreWidget());
    ASSERT(ogreWidget()->ogreData().sceneManager);
    Ogre::SceneManager* sceneMngr = ogreWidget()->ogreData().sceneManager;

    // remove the current mesh if we have
    if (mNode) {
        // reset the node
        mNode->detachAllObjects();
        mNode->setPosition(Ogre::Vector3::ZERO);
        mNode->setScale(Ogre::Vector3(1,1,1));
        mNode->resetOrientation();
    } else {
        // create the node
        mNode = sceneMngr->getRootSceneNode()->createChildSceneNode();
    }

    // remove the entity
    if (mEntity) {
        sceneMngr->destroyEntity(mEntity);
        mEntity = 0;
    }

    // load the new one
    try {
        mEntity = sceneMngr->createEntity(meshName.toStdString());
    } catch(...){
        return false;
    }

    mNode->attachObject(mEntity);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::frameUpdate(float fp)
{

//    handleCameraInput();
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::systemsReady(void)
{
    ASSERT(ogreWidget());
    ASSERT(ogreWidget()->ogreData().sceneManager);
    ASSERT(ogreWidget()->ogreData().camera);
    ASSERT(ogreWidget()->ogreData().viewport);
    ASSERT(ogreWidget()->ogreData().renderWindow);

    // load the GlobalResources.cfg file
    char* envPath = 0;
    if (!core::OSHelper::getEnvVar("CZ01_RC_PATH", envPath)) {
        QTDEBUG_WARNING("You haven't set the environment variable CZ01_RC_PATH. This"
            " can cause some problems or errors during the execution of the tool\n");
    } else {
        std::string path = envPath;
        core::OSHelper::addEndPathVar(path);
        path.append("GlobalResources.cfg");
        if (!loadResourceFile(QString(path.c_str()))) {
            QTDEBUG_WARNING("We couldn't load the GlobalResources.cfg file from " <<
                            path << " this can cause some problems during the "
                                "execution of this app.\n");
        }
    }


    OgreWidget::OgreData& data = ogreWidget()->ogreData();
    mOrbitCamera = new OrbitCamera(data.camera,
                                   data.sceneManager,
                                   data.frameTime,
                                   false);

}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::keyPressEvent(QKeyEvent* event)
{
    // we will move the camera and the sliders too using ASDW
    switch (event->key()) {
    case Qt::Key::Key_A:
        ui.xRotSlider->setValue(ui.xRotSlider->value() + 1);
        break;
    case Qt::Key::Key_S:
        ui.yRotSlider->setValue(ui.yRotSlider->value() + 1);
        break;
    case Qt::Key::Key_D:
        ui.xRotSlider->setValue(ui.xRotSlider->value() - 1);
        break;
    case Qt::Key::Key_W:
        ui.yRotSlider->setValue(ui.yRotSlider->value() - 1);
        break;
    }
    event->accept();
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::keyReleaseEvent(QKeyEvent* event)
{

    event->ignore();
}
////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::mousePressEvent(QMouseEvent* event)
{
    event->ignore();
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::mouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
}

////////////////////////////////////////////////////////////////////////////////
QtAnimPlayer::QtAnimPlayer(rrh::ResourceHandler* rh,
                           const std::string& plugins,
                           const std::string& ogre,
                           const std::string& resource) :
    QtOgreAppBase(rh, plugins, ogre, resource)
,   mLastPathLoaded(".")
,   mNode(0)
,   mEntity(0)
,   mOrbitCamera(0)
{
    ui.setupUi(this);

    // connect signals and slots
    connect(ui.loadBtn, SIGNAL(clicked(bool)), this, SLOT(onLoadClicked(bool)));
    connect(ui.xRotSlider, SIGNAL(valueChanged(int)), this, SLOT(xRotSliderChanged(int)));
    connect(ui.yRotSlider, SIGNAL(valueChanged(int)), this, SLOT(yRotSliderChanged(int)));

    // add the ogre widget to the layout
    QWidget* ogrew = ogreWidget();
    ogrew->setSizePolicy(QSizePolicy::Policy::Expanding,
                         QSizePolicy::Policy::Expanding);
    ui.horizontalLayout->addWidget(ogreWidget());

}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::handleCameraInput()
{
    if (!mOrbitCamera) {
        // nothing to do
        return;
    }

    ASSERT(ogreWidget());

    ///////////////////////////////////////////////////////////////////////////
    // CAMERA
    //  float lCoeff = 200.0f * Common::GlobalObjects::lastTimeFrame;
    Ogre::Vector3 mTranslationVec = Ogre::Vector3::ZERO;
    Ogre::Real zoom = mOrbitCamera->zoom();

    // HERE WE DEFINE THE KEYS USED TO MOVE THE CAMERA, WE WILL USE THE
    // ARROWS TO MOVE THE CAMERA
    // NOTE: we are getting the cameraPosition and modifying the position
    // without using translation, this is because we want to move always
    // in the same axis whatever be the direction of the camera.

/*
    // MOUSE
    const OIS::MouseState& lMouseState = data.OISMouse->getMouseState();

    if(data.inputHelper.isKeyPressed(input::KeyCode::KC_LEFT) ||
        data.inputHelper.isKeyPressed(input::KeyCode::KC_A)) {
        mTranslationVec.x -= 1.0f;
    }
    if(data.inputHelper.isKeyPressed(input::KeyCode::KC_RIGHT) ||
        data.inputHelper.isKeyPressed(input::KeyCode::KC_D)) {
        mTranslationVec.x += 1.0f;
    }
    if(data.inputHelper.isKeyPressed(input::KeyCode::KC_UP) ||
        data.inputHelper.isKeyPressed(input::KeyCode::KC_W)) {
        mTranslationVec.z -= 1.0f;
    }
    if(data.inputHelper.isKeyPressed(input::KeyCode::KC_DOWN) ||
        data.inputHelper.isKeyPressed(input::KeyCode::KC_S)) {
        mTranslationVec.z += 1.0f;
    }

    if(mTranslationVec != Ogre::Vector3::ZERO) {
        mOrbitCamera->moveCamera(mTranslationVec);
    }
    if(zoom != mOrbitCamera->zoom()) {
        mOrbitCamera->setZoom(zoom);
    }

    const float lMouseZ = input::Mouse::relZ();
    float scrollZoom = mOrbitCamera->zoom();
    if (lMouseZ > 0.0f) {
        scrollZoom += 5.f;
    } else if (lMouseZ < 0.0f) {
        scrollZoom -= 5.f;
    }
    if(scrollZoom != mOrbitCamera->zoom()) {
        mOrbitCamera->setZoom(scrollZoom);
    }

    // check tracking camera
    static int lastX = 0, lastY = 0;
    const float lMouseX = float(input::Mouse::relX());
    const float lMouseY = float(input::Mouse::relY());
    if (data.inputHelper.isMousePressed(input::MouseButtonID::MB_Right)) {
        const float factor = -0.01 * 1.5f;
        mOrbitCamera->rotateCamera(Ogre::Radian(lMouseX * factor),
                                    Ogre::Radian(lMouseY * factor));
    }

    // check for the type of camera we want to use
    if (data.inputHelper.isKeyPressed(input::KeyCode::KC_1)) {
        mOrbitCamera->setCameraType(OrbitCamera::CameraType::FreeFly);
    } else if (data.inputHelper.isKeyPressed(input::KeyCode::KC_2)) {
        mOrbitCamera->setCameraType(OrbitCamera::CameraType::Orbit);
    }*/

}

////////////////////////////////////////////////////////////////////////////////
QtAnimPlayer::~QtAnimPlayer()
{
    delete mOrbitCamera;
}



////////////////////////////////////////////////////////////////////////////////
// Slots

void
QtAnimPlayer::onLoadClicked(bool)
{
    // try to load the .mesh file
    const QString fileName = QFileDialog::getOpenFileName(this,
                                                          tr("Open Ogre Mesh"),
                                                          mLastPathLoaded,
                                                          tr("OgreMesh files (*.mesh)"));
    if (fileName.isEmpty()) {
        // do nothing
        return;
    }

    // add the file path to the resource system before load the file
    loadLocationFromFile(fileName);

    // now load the file and put it in the scene
    QFileInfo finfo(fileName);
    mLastPathLoaded = finfo.path();
    const QString meshName = finfo.fileName();

    if (!loadMesh(meshName)) {
        QTDEBUG_WARNING("The file " << meshName.toStdString() <<
                        " couldn't be loaded.\n");
        return;
    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::yRotSliderChanged(int val)
{
    if (!mOrbitCamera) {
        return;
    }
    Ogre::Degree degree(val);
    mOrbitCamera->setYawAngle(degree);
}

void
QtAnimPlayer::xRotSliderChanged(int val)
{
    if (!mOrbitCamera) {
        return;
    }
    Ogre::Degree degree(val);
    mOrbitCamera->setPitchAngle(degree);
}
}
