/*
 * Gui.cpp
 *
 *  Created on: 28/05/2011
 *      Author: Emi
 */

#include "Gui.h"

Gui::Gui(OIS::Mouse * m) {
	num = 0;
	CEGUI::OgreRenderer &  myRenderer =	CEGUI::OgreRenderer::create();
	sys = CEGUI::System::getSingletonPtr();
	sys->create( myRenderer );

	myRendererPtr = & myRenderer;

	rp = static_cast<CEGUI::DefaultResourceProvider*>
		(CEGUI::System::getSingleton().getResourceProvider());

	rp->setResourceGroupDirectory("schemes", "CEGUIdatafiles/schemes/");
	rp->setResourceGroupDirectory("imagesets", "CEGUIdatafiles/imagesets/");
	rp->setResourceGroupDirectory("fonts", "CEGUIdatafiles/fonts/");
	rp->setResourceGroupDirectory("layouts", "CEGUIdatafiles/layouts/");
	rp->setResourceGroupDirectory("looknfeels", "CEGUIdatafiles/looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", "CEGUIdatafiles/lua_scripts/");

	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");


	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
	if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
	{
		parser->setProperty("SchemaDefaultResourceGroup", "schemas");
	}

	rp->setResourceGroupDirectory("schemas", "CEGUIdatafiles/xml_schemas/");


	CEGUI::FontManager::getSingleton().create( "DejaVuSans-10.font" );
	CEGUI::System::getSingleton().setDefaultFont( "DejaVuSans-10" );


	CEGUI::ImagesetManager::getSingleton().createFromImageFile("map_preview", "menu_solo.png");
	CEGUI::SchemeManager::getSingleton().create( "menu.scheme" );
	CEGUI::System::getSingleton().setDefaultMouseCursor( "menu", "puntero" );
	CEGUI::Size s = CEGUI::Size(20,25);
	CEGUI::MouseCursor::getSingleton().setExplicitRenderSize(s);

	CEGUI::System::getSingleton().setDefaultTooltip("Menu/Tooltip" );


	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	wMgrPtr = &wmgr;

	mouse = m;
	menu2 = false;
	presionado = false;

	principal  = 0;

	w = myRendererPtr->getDisplaySize().d_width;
	h = myRendererPtr->getDisplaySize().d_height;






	menuPrincipal = new MainGui(wMgrPtr, "pruebaMenu.layout", true);
}

Gui::~Gui() {


}

int Gui::load()
{

	myRoot = wMgrPtr->createWindow( "DefaultWindow", "root" );
	CEGUI::System::getSingleton().setGUISheet( myRoot );



	menuPrincipal->loadMenu();



	RegisterForEvents();



	//This is for the mouse pointer to appear in the middle of the screen everytime a window is created
	CEGUI::System::getSingleton().injectMousePosition(w/2,h/2);


	return 0;
}

void Gui::execute()
{
	myRendererPtr->setRenderingEnabled(true);
	useMouse();
	clock_t mLastTimeInjection=clock();
	CEGUI::System::getSingleton().injectTimePulse( ( clock() - mLastTimeInjection ) * 0.001f );
	mLastTimeInjection = clock();
	cout << "finExecute" << endl;

}

void Gui::unload()
{
	wMgrPtr->destroyAllWindows();
	myRendererPtr->setRenderingEnabled(false);
}

void Gui::useMouse()
{
	OIS::MouseState mState = mouse->getMouseState();
	CEGUI::System::getSingleton().injectMouseMove((float)(mState.X.rel),(float)(mState.Y.rel));


	if(mState.buttonDown(OIS::MB_Left)){
		if(!presionado){
			presionado = true;
			CEGUI::System::getSingleton().injectMouseButtonDown(CEGUI::LeftButton);
		}
	}
	else{
		if(presionado){
			presionado = false;
			CEGUI::System::getSingleton().injectMouseButtonUp(CEGUI::LeftButton);
		}
	}


}

void Gui::cargarImagen(const string &imagen, const string &nombre)
{
	CEGUI::Window *myImageWindow = CEGUI::WindowManager::getSingleton().createWindow("Menu/StaticImage", nombre);
	wMgrPtr->getWindow("Root")->addChildWindow(myImageWindow);
	myImageWindow->setPosition(CEGUI::UVector2( CEGUI::UDim( 0, 0 ), CEGUI::UDim( 0, 0 ) ) );

	cout<<"h " << h << endl;
	float proporcion = (float)h / 1920;
	cout<<"proporcion " << proporcion << endl;
	int ancho = 944*proporcion;
	cout<<"ancho " << ancho << endl;

	myImageWindow->setSize(CEGUI::UVector2(CEGUI::UDim(0,ancho),CEGUI::UDim(1,0)));
	myImageWindow->setProperty("Image",imagen);
	myImageWindow->setProperty("BackgroundEnabled", "true");
	myImageWindow->setProperty("FrameEnabled", "False");
	myImageWindow->setText("Prueba de texto dentro de la ventana");
	myImageWindow->setTooltipText("Texto de la imagen");
	myImageWindow->setAlpha(1.0);

}



void Gui::descargarImagen(const string &nombre)
{
	wMgrPtr->destroyWindow(nombre);
}


void Gui::RegisterForEvents()
{

}










