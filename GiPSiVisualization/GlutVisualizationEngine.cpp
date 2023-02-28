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

Contributor(s): Svend Johannsen.
*/

/*
===============================================================================
	Headers
===============================================================================
*/

#include <GL/glut.h>

#include "GlutVisualizationEngine.h"

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

GlutVisualizationEngine::GlutVisualizationEngine(int argc, char **argv, SimulationKernel* simKernel,
												 int x, int y, int width, int height) : VisualizationEngine(simKernel)
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

	// Initialize OpenGL and setup the scene

	this->Init();
}

void GlutVisualizationEngine::OnDisplay(void)
{
	this->allScenes[0]->Render();
}

void GlutVisualizationEngine::OnKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '': // esc
	case 'q':
		if (this->simKernel->IsRunning()) { this->simKernel->Stop(); }
		exit(0);
		break;
	case 'r':
		if (this->simKernel->IsRunning())
		{
			this->simKernel->Stop();
		}
		else
		{
			this->simKernel->Run();
		}
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
			this->allScenes[0]->SetCameraMode(0, CameraMode_Planar);
			this->allScenes[0]->MoveCamera(0, -amount, 0.0);
			break;
		case GLUT_KEY_RIGHT :
			this->allScenes[0]->SetCameraMode(0, CameraMode_Planar);
			this->allScenes[0]->MoveCamera(0, amount, 0.0);
			break;
		case GLUT_KEY_UP : 
			this->allScenes[0]->SetCameraMode(0, CameraMode_Planar);
			this->allScenes[0]->MoveCamera(0, 0.0, amount);
			break;
		case GLUT_KEY_DOWN : 
			this->allScenes[0]->SetCameraMode(0, CameraMode_Planar);
			this->allScenes[0]->MoveCamera(0, 0.0, -amount);
			break;
		default:
			break;
	}
}

void GlutVisualizationEngine::OnMouseClick(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		this->allScenes[0]->SetCameraMode(0, CameraMode_Spherical);

		mouseX = x;
		mouseY = y;
	}

	if ((button == GLUT_MIDDLE_BUTTON) && (state == GLUT_DOWN))
	{
		this->allScenes[0]->SetCameraMode(0, CameraMode_Pan);

		mouseX = x;
		mouseY = y;
	}

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
	{
		this->allScenes[0]->SetCameraMode(0, CameraMode_Zoom);

		mouseY = y;
	}
}

void GlutVisualizationEngine::OnMouseMotion(int x, int y)
{
	float speed  = 0.03;
	float deltaX = speed * (x - mouseX);
	float deltaY = speed * (mouseY - y);

	this->allScenes[0]->MoveCamera(0, deltaX, deltaY);
	
	mouseX = x;
	mouseY = y;
}

void GlutVisualizationEngine::OnReshape(int width, int height)
{
	float fov = 45;

	glutReshapeWindow(width, height);

	this->allScenes[0]->PerspectiveProjection(fov, width, height);
	
	glutPostRedisplay();
}

void GlutVisualizationEngine::Start(void)
{
	glutMainLoop();
}