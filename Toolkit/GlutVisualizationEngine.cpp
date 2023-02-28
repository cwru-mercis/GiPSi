/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Glut Visualization Engine implementation
(GlutVisualizationEngine.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen, Suriya Natsupakpong.
*/

////	GLUTVISUALIZATIONENGINE.CPP v0.0
////
////////////////////////////////////////////////////////////////

/*
===============================================================================
	Headers
===============================================================================
*/

#include <GL/glut.h>

#include "GlutVisualizationEngine.h"
#include "logger.h"
#include "ToolkitShaderLoader.h"
#include "OpenHapticsManager.h"
#include "ToolkitUserInterface.h"

/*
===============================================================================
	Global functions used in GLut callbacks
===============================================================================
*/

GlutVisualizationEngine *GLUTVISENGINE; // Global pointer to GlutVisualizationEngine

void GlutMainWindow_Display(void)
{
	GLUTVISENGINE->OnDisplay();
}

void GlutMainWindow_Keyboard(unsigned char key, int x, int y)
{
	GLUTVISENGINE->OnKeyboard(key, x, y);
}

void GlutMainWindow_KeyboardSpec(int key, int x, int y)
{
	GLUTVISENGINE->OnKeyboardSpec(key, x, y);
}

void GlutMainWindow_MouseClick(int button, int state, int x, int y)
{
	GLUTVISENGINE->OnMouseClick(button, state, x, y);	
}

void GlutMainWindow_MouseMotion(int x, int y)
{
	GLUTVISENGINE->OnMouseMotion(x, y);
}

void GlutMainWindow_Reshape(int width, int height)
{
	GLUTVISENGINE->OnReshape(width, height);
}

/*
===============================================================================
	GlutVisualizationEngine
===============================================================================
*/

GlutVisualizationEngine::GlutVisualizationEngine(int argc, char **argv, DisplayBuffer * dbHead,
												 XMLNode * visualizationNode,
												 int x, int y, int width, int height)
												 :	VisualizationEngine(dbHead, new ToolkitShaderLoader()), currentCameraID(0)
{
	// Reference to GlutVisualizationEngine

	GLUTVISENGINE = this;

	// Initialize GLut runtime

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(x, y);
	glutInitWindowSize(width, height);

	glutCreateWindow("GiPSi Visualization Engine");

	glutDisplayFunc	(GlutMainWindow_Display);
	glutKeyboardFunc(GlutMainWindow_Keyboard);
	glutSpecialFunc	(GlutMainWindow_KeyboardSpec);
	glutMouseFunc	(GlutMainWindow_MouseClick);
	glutMotionFunc	(GlutMainWindow_MouseMotion);
	glutReshapeFunc	(GlutMainWindow_Reshape);

	this->Init(visualizationNode);
	ui = new EndoSimUserInterface();
	((EndoSimUserInterface*)ui)->Initialize();
}

void GlutVisualizationEngine::OnDisplay(void)
{
	try
	{
		for (int i = 0; i < this->nScenes; i++)
			this->allScenes[i]->Render(currentCameraID);
	}
	catch (...)
	{
		// We only throw terminal exceptions, so if we catch any exceptions, exit
		logger->Message("GlutVisualizationEngine", "A fatal exception has occurred.  Press any key to end process.\n", 0);

		int anyKey = getchar();

		exit(0);
	}
}

void GlutVisualizationEngine::OnKeyboard(unsigned char key, int x, int y)
{
	// pass key to User Interface
	((EndoSimUserInterface*)ui)->OnKeypress(key);
	
	// process key related Visualization Engine
	switch (key)
	{
		case '':	// esc
		case 'q':
			exit(0);
			break;
		case 'c':	// change camera
			currentCameraID++;
			if ( currentCameraID >= allScenes[0]->GetNumberOfCamera() )
				currentCameraID = 0;	
			printf("set view to camera %d\n",currentCameraID);
			break;
		default:
			break;
	}
}

void GlutVisualizationEngine::OnKeyboardSpec(int key, int x, int y)
{
	const float amount = 1.0;

	switch(key)
	{
		case GLUT_KEY_LEFT :
			this->allScenes[0]->SetCameraMode(currentCameraID, CameraMode_Planar);
			this->allScenes[0]->MoveCamera(currentCameraID, -amount, 0.0);
			break;
		case GLUT_KEY_RIGHT :
			this->allScenes[0]->SetCameraMode(currentCameraID, CameraMode_Planar);
			this->allScenes[0]->MoveCamera(currentCameraID, amount, 0.0);
			break;
		case GLUT_KEY_UP : 
			this->allScenes[0]->SetCameraMode(currentCameraID, CameraMode_Planar);
			this->allScenes[0]->MoveCamera(currentCameraID, 0.0, amount);
			break;
		case GLUT_KEY_DOWN : 
			this->allScenes[0]->SetCameraMode(currentCameraID, CameraMode_Planar);
			this->allScenes[0]->MoveCamera(currentCameraID, 0.0, -amount);
			break;
		default:
			break;
	}
}

void GlutVisualizationEngine::OnMouseClick(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		this->allScenes[0]->SetCameraMode(currentCameraID, CameraMode_Spherical);

		mouseX = x;
		mouseY = y;
	}

	if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
	{
		this->allScenes[0]->SetCameraMode(currentCameraID, CameraMode_Pan);

		mouseX = x;
		mouseY = y;
	}

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
	{
		this->allScenes[0]->SetCameraMode(currentCameraID, CameraMode_Zoom);

		mouseY = y;
	}
}

void GlutVisualizationEngine::OnMouseMotion(int x, int y)
{
	float speed  = 0.03;
	float deltaX = speed * (x - mouseX);
	float deltaY = speed * (mouseY - y);

	this->allScenes[0]->MoveCamera(currentCameraID, deltaX, deltaY);
	
	mouseX = x;
	mouseY = y;
}

void GlutVisualizationEngine::OnReshape(int width, int height)
{
	float fov = 45;

	glutReshapeWindow(width, height);

	this->allScenes[0]->PerspectiveProjection(currentCameraID, fov, width, height);
	
	glutPostRedisplay();
}

void GlutVisualizationEngine::Start(SimulationKernel * newSimKernel)
{
	VisualizationEngine::Start(newSimKernel);

	((EndoSimUserInterface*)ui)->setSimKernel(newSimKernel);

#ifdef GIPSI_HAPTICS_ENABLED
	// first check if there are any cameras attached to haptic interfaces
	bool need_hm= false;
	for(int i=0; i<this->allScenes[0]->GetNumberOfCamera(); i++)
	{
		if(this->allScenes[0]->GetCameraType(i) == 1) // camera type 1 = Haptics attached camera
			need_hm=true;
	}

	if (need_hm) {
		// Set-up Haptics Manager
		//  as OpenHapticsManager is a singleton, we will use the exiting one
		OpenHapticsManager *hapticsMan = OpenHapticsManager::GetHapticsManager();		
		// Go through all the cameras in scene 0, find the ones which are controlled by a haptic interface
		//  and attach them to the appropriate haptic interface
		for(int i=0; i<this->allScenes[0]->GetNumberOfCamera(); i++)
		{
			if(this->allScenes[0]->GetCameraType(i) == 1)	// camera type 1 = Haptics attached camera
			{
				unsigned int HapticID = this->allScenes[0]->GetAttachedHapticInterfaceIDOfCamera(i);
				if( HapticID != -1 )
					this->allScenes[0]->AttachHapticInterfaceToCamera(i, hapticsMan->GetHapticInterface(HapticID));
			}
		}
	}
#endif

	glutMainLoop();
}