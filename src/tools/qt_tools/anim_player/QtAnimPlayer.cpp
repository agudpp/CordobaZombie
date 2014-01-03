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
QtAnimPlayer::loadAnimations(Ogre::Entity* entity)
{
    if (!entity) {
        QTDEBUG_CRITICAL("entity is null! This is an error, we cannot load animations\n");
        return;
    }

    // destroy all the current animations
    destroyAnimationStuff();

    // now load the entity animations
    Ogre::AnimationStateSet* anims = entity->getAllAnimationStates();
    if (anims == 0 || !anims->getAnimationStateIterator().hasMoreElements()) {
        // no animations found for this entity
        QMessageBox::information(this,
                                 tr("Animations"),
                                 "La entity " + QString(entity->getName().c_str()) +
                                    " no contiene animaciones");
        return;
    }

    // now load all the animations
    Ogre::AnimationStateIterator it = anims->getAnimationStateIterator();
    unsigned int widgetId = 1;
    while (it.hasMoreElements()) {
        Ogre::AnimationState* anim = it.getNext();
        if (anim) {
            AnimInfo* ai = new AnimInfo;
            ai->animState = anim;
            ai->label = new QLabel(anim->getAnimationName().c_str(), this);
            ai->button = new QCheckBox(this);
            ai->button->setChecked(false);

            // add this two elements into the layout
            ui.animFormLayout->setWidget(widgetId, QFormLayout::LabelRole, ai->label);
            ui.animFormLayout->setWidget(widgetId, QFormLayout::FieldRole, ai->button);
            widgetId++;
            mAnimInfo.push_back(ai);
        }

    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::destroyAnimationStuff(void)
{
    for (AnimInfo* ai : mAnimInfo) {
        delete ai;
    }
    mAnimInfo.clear();
}


////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::frameUpdate(float fp)
{
    for (AnimInfo* ai : mAnimInfo) {
        if (ai->button->isChecked()) {
            if (!ai->animState->getEnabled()) {
                ai->animState->setEnabled(true);
                ai->animState->setLoop(true);
            }
            ai->animState->addTime(fp * mAnimVel);
        } else {
            if (ai->animState->getEnabled()) {
                ai->animState->setEnabled(false);
            }
        }
    }
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
    mOrbitCamera->setZoomDist(50.f);

    // create the axis to be shown in the scene
    m3DAxis = core::PrimitiveDrawer::instance().create3DAxis(Ogre::Vector3::ZERO, 15);
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
    // zoom
    case Qt::Key::Key_Plus:
        zoomDecClickedChanged(true);
        break;
    case Qt::Key::Key_Minus:
        zoomIncClickedChanged(true);
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
    mLastMousePoint = event->globalPos();
    event->ignore();
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::mouseReleaseEvent(QMouseEvent* event)
{
    event->ignore();
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::mouseMoveEvent(QMouseEvent* event)
{
    if (!(event->buttons() & Qt::MouseButton::LeftButton)) {
        return;
    }
    // Note that this is not the right option, we need to implement this in the
    // OgreWidget since we only want to move the camera when moving the mouse
    // inside of that widget. To do that we can emmit an event with the mouse
    // information and catch that signal here. Instead of moving the camera
    // whenever we move the mouse in ALL the widget!.
    //

    const QPoint& globalPos = event->globalPos();
    // then move the camera depending where it was moved
    const float relX = mLastMousePoint.x() - globalPos.x();
    const float relY = mLastMousePoint.y() - globalPos.y();
    mLastMousePoint = globalPos;

    const float factor = -0.01 * 1.5f;
    ui.xRotSlider->setValue(ui.xRotSlider->value() + relX);
    ui.yRotSlider->setValue(ui.yRotSlider->value() + relY);
    event->accept();
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
,   m3DAxis(0)
{
    ui.setupUi(this);

    // connect signals and slots
    connect(ui.loadBtn, SIGNAL(clicked(bool)), this, SLOT(onLoadClicked(bool)));
    connect(ui.rscLoadBtn, SIGNAL(clicked(bool)), this, SLOT(onLoadResourceClicked(bool)));
    connect(ui.xRotSlider, SIGNAL(valueChanged(int)), this, SLOT(xRotSliderChanged(int)));
    connect(ui.yRotSlider, SIGNAL(valueChanged(int)), this, SLOT(yRotSliderChanged(int)));
    connect(ui.incZoomBtn, SIGNAL(clicked(bool)), this, SLOT(zoomIncClickedChanged(bool)));
    connect(ui.decZoomBtn, SIGNAL(clicked(bool)), this, SLOT(zoomDecClickedChanged(bool)));
    connect(ui.velControllerSlider, SIGNAL(valueChanged(int)), this, SLOT(velControllerSliderChanged(int)));

    // init the velocity of the animation
    velControllerSliderChanged(ui.velControllerSlider->value());

    // add the ogre widget to the layout
    QWidget* ogrew = ogreWidget();
    ogrew->setSizePolicy(QSizePolicy::Policy::Expanding,
                         QSizePolicy::Policy::Expanding);
    ui.horizontalLayout->addWidget(ogreWidget());

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
    if (mEntity) {
        destroyAnimationStuff();
        loadAnimations(mEntity);
    }
}

////////////////////////////////////////////////////////////////////////////////
void
QtAnimPlayer::onLoadResourceClicked(bool)
{
    // try to load the .mesh file
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::FileMode::AnyFile);
    dialog.setDirectory(mLastPathLoaded);
    dialog.setFilter(tr("Resources files (*.cfg)"));

    if (!dialog.exec()) {
        return;
    }
    const QStringList paths = dialog.selectedFiles();
    // we will only want the first
    if (paths.isEmpty()) {
        return;
    }

    const QString rscPath = paths.first();

    QFileInfo finfo(rscPath);
    if (finfo.isDir()) {
        // we need to add the location only
        loadLocationFromPath(rscPath);
    } else {
        // else is a cfg file
        loadResourceFile(rscPath);
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

void
QtAnimPlayer::zoomIncClickedChanged(bool)
{
    if (!mOrbitCamera) {
        return;
    }
    const float currentZoom = mOrbitCamera->zoom();
    mOrbitCamera->setZoomDist(currentZoom + 5.f);
}
void
QtAnimPlayer::zoomDecClickedChanged(bool)
{
    if (!mOrbitCamera) {
        return;
    }
    const float currentZoom = mOrbitCamera->zoom();
    mOrbitCamera->setZoomDist(currentZoom - 5.f);
}

void
QtAnimPlayer::velControllerSliderChanged(int val)
{
    mAnimVel = static_cast<float>(val) /
        static_cast<float>(ui.velControllerSlider->maximum());

    // animVel will be between [0,1]. So we can change this and setting
    // the value between [0,2].
    mAnimVel *= 2.f;
}

}
