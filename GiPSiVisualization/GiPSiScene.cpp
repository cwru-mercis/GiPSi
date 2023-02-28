/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Scene Implementation (GiPSiScene.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen, Nathan Brown.
*/

/*
===============================================================================
	Headers
===============================================================================
*/

#include <GL/glut.h>		// NOTE: Ideally GiPSiScene would only use gl.h, glut.h should only be used in GlutVisualizationEngine
#include <stdio.h>
#include <timing.h>

#include "GiPSiException.h"
#include "GiPSiScene.h"
#include "XMLNodeList.h"

/*
===============================================================================
	Frames per second class
===============================================================================
*/

FramesPerSecond::FramesPerSecond(void)
{
	str[0] = ' ';
	str[1] = '\0';
	count  = 0;
	time   = 0;
	flag   = false;
}

void FramesPerSecond::Display(int width, int height)
{
	int topOffset	= 20;
	int	sideOffset	= 90;

	this->DrawString(str, float(width-sideOffset), float(height-topOffset), width, height);
}

void FramesPerSecond::DrawString(char *str, float x, float y, int width, int height)
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, width, 0.0, height, -1.0, 1.0);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();

			glColor3f(1.0, 1.0, 1.0);
			glRasterPos2f(x, y);
			void *font = GLUT_BITMAP_HELVETICA_18;
			
			for(int i = 0; str[i]; i++)
				glutBitmapCharacter(font, str[i]);

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}

void FramesPerSecond::Begin(void)
{
	start_timer(0);
	flag=true;
}

void FramesPerSecond::End(void)
{
	if (flag==true) {
		time += get_timer(0);
		count++;
		sprintf(str, "fps: %.2f ", fhz(time/count));
	}
	else {
		sprintf(str, "fps: 0.0 ");
	}
}

/*
===============================================================================
	Scene
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param sceneNode XML project "Scene" node for initialization.
 * @param nObject The number of objects in the visualization engine.
 * @param object Array of nObject objects.
 * @param nTexture The number of textures in the visualization engine.
 * @param texture Array of nTexture textures.
 * @param nShader The number of shaders in the visualization engine.
 * @param shader Array of nShader shaders.
 */
Scene::Scene(XMLNode * sceneNode,
			 int nObject, DisplayBuffer **object,
			 int nTexture, Texture **texture,
			 int nShader, Shader **shader) :
				shader(NULL),
				texture(NULL),
				buffer(NULL),
				camera(NULL),
				light(NULL)
{
	XMLNodeList * sceneChildren = sceneNode->GetChildren();
	XMLNode * simulationObjectNamesNode = sceneChildren->GetNode("simulationObjectNames");
	XMLNodeList * simulationObjectNamesChildren = simulationObjectNamesNode->GetChildren();
	// Check if the first simulationObjectName is "ALL"
	XMLNode * simObjectName1Node = simulationObjectNamesChildren->GetNode((unsigned int)0);
	const char * simObjectName1 = simObjectName1Node->GetValue();
	if (strcmp(simObjectName1, "ALL") == 0)
	{
		delete simObjectName1;
		// Just add all of the simObject meshes
		this->nBuffer	= nObject - nTexture;
		this->buffer	= new DisplayBuffer*[nBuffer];
		int iObject = 0;
		for (int i = 0; i < nObject && iObject < nBuffer; i++)
		{
			if (object[i]->GetTextureType() == GIPSI_NO_TEXTURE)
			{
				this->buffer[iObject] = object[i];
				iObject++;
			}
		}
	}
	else
	{
		delete simObjectName1;
		// Otherwise, just add the meshes for the requested objects
		this->nBuffer	= simulationObjectNamesChildren->GetLength();
		this->buffer	= new DisplayBuffer*[this->nBuffer];
		for (int i = 0; i < this->nBuffer; i++)
		{
			XMLNode * simObjectNameNode = simulationObjectNamesChildren->GetNode(i);
			const char * simObjectName = simObjectNameNode->GetValue();
			delete simObjectNameNode;

			this->buffer[i] = NULL;
			// Look for the name in the loaded objects
			for (int j = 0; j < nObject; j++)
			{
				if (strcmp(object[j]->GetObjectName(), simObjectName) == 0)
				{
					// Once we find the name, load its display array
					this->buffer[i] = object[j];
					break;
				}
			}
			if (this->buffer[i] == NULL)
			{
				// If we haven't found the object yet, throw an exception
				char error[256];
				sprintf_s(error, 256, "SimObject %s not found.", simObjectName);
				throw new GiPSiException("Scene initialization", error);
				delete simObjectName;
				return;
			}
			delete simObjectName;
		}
	}

	this->nTexture	= nTexture;
	this->texture	= new Texture*[nTexture];
	for (int i = 0; i < this->nTexture; i++)
	{
		this->texture[i] = texture[i];
	}

	this->nShader	= nShader;
	if (this->nShader > 0)
	{
		this->shader			= new Shader*[nShader];
		for (int i = 0; i < this->nShader; i++)
		{
			this->shader[i]		= shader[i];
		}
		useFragmentPixelShaders = true;
	}
	else
	{
		this->shader			= NULL;
		useFragmentPixelShaders = false;
	}

	fps = new FramesPerSecond();

	displayCoordinateSystem = true;
	useFragmentPixelShaders = true;

	// Create cameras
	XMLNode * camerasNode			= sceneChildren->GetNode("cameras");
	XMLNodeList * camerasChildren	= camerasNode->GetChildren();
	this->nCamera					= camerasChildren->GetLength();
	this->camera					= new Camera*[this->nCamera];
	for (int i = 0; i < this->nCamera; i++)
	{
		XMLNode * cameraNode = camerasChildren->GetNode(i);		
		this->camera[i] = new Camera(cameraNode);		
		delete cameraNode;
	}
	delete camerasChildren;
	delete camerasNode;

	// Create lights
	XMLNode * lightsNode			= sceneChildren->GetNode("lights");
	XMLNodeList * lightsChildren	= lightsNode->GetChildren();
	this->nLight					= lightsChildren->GetLength();
	this->light						= new Light*[this->nLight];
	for (int i = 0; i < this->nLight; i++)
	{
		XMLNode * lightNode = lightsChildren->GetNode(i);
		this->light[i] = new Light(lightNode, this->camera, this->nCamera);
		delete lightNode;
	}
	delete lightsChildren;
	delete lightsNode;

	delete simObjectName1Node;
	delete simulationObjectNamesChildren;
	delete simulationObjectNamesNode;
	delete sceneChildren;
}

/**
 * Deselect the currently selected shader.
 */
void Scene::DeselectShader(void)
{
	if (this->useFragmentPixelShaders == true)
	{
		Shader::UseFixedPipeline();
	}
}

/**
 * Deselect all currently selected textures.
 */
void Scene::DeselectAllTextures(void)
{
	int maxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);

	for (int unit=0; unit<maxTextureUnits; unit++)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		glBindTexture(GL_TEXTURE_2D, NULL);
	}
}

/**
 * Draw 'world axes' at the specified position.
 * 
 * @param x The X position of the axes.
 * @param y The Y position of the axes.
 * @param z The Z position of the axes.
 */
void Scene::DrawCoordinateSystemAt(float x, float y, float z)
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	// Translate to position

	glPushMatrix();
	glTranslatef(x, y, z);

	// Draw the three axis

	glColor3f(1,1,1);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(2.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 2.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 2.0);
	glEnd();

	// Add a cone at the end of each axis

	glPushMatrix();
	glTranslatef(2.0f, 0.0f, 0.0f);
	glRotatef(90, 0.0f, 1.0f, 0.0f);
	glutSolidCone(0.08f, 0.5f, 10, 2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 2.0f, 0.0f);
	glRotatef(270, 1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f, 0.5f, 10, 2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 2.0f);
	glRotatef(0, 0.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f, 0.5f, 10, 2);
	glPopMatrix();

	// Add a label to each axis

	glPushMatrix();
	glTranslatef(2.4f, -0.4f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.2, 0.2, 0.0);
		glVertex3f(0.0, 0.2, 0.0);
		glVertex3f(0.2, 0.0, 0.0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-0.4f, 2.4f, 0.0f);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.2, 0.0);
		glVertex3f(0.1, 0.1, 0.0);
		glVertex3f(0.2, 0.2, 0.0);
		glVertex3f(0.1, 0.1, 0.0);
		glVertex3f(0.1, 0.0, 0.0);
		glVertex3f(0.1, 0.1, 0.0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, -0.4f, 2.4f);
	glBegin(GL_LINES);
		glVertex3f(0.0, 0.2, 0.0);
		glVertex3f(0.0, 0.2, 0.2);
		glVertex3f(0.0, 0.2, 0.0);
		glVertex3f(0.0, 0.0, 0.2);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.2);
	glEnd();
	glPopMatrix();

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
}

/**
 * Draw the indexed mesh.
 * 
 * @param n The index of the mesh to be drawn.
 */
void Scene::DrawMesh(int n)
{
	static DisplayArray *mesh;

	this->buffer[n]->ConditionalDequeue();
	mesh = this->buffer[n]->GetReadArray();

	if (mesh != NULL &&
		(mesh->dA_size > 0 || mesh->iA_size > 0)) // Not sure if this should be an "&&"
	{
		switch(mesh->header.polyMode)
		{
			case GIPSI_POLYGON_OUTLINE:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
			case GIPSI_POLYGON_FILL:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				switch(mesh->header.shadeMode)
				{
					case GIPSI_SHADE_FLAT:
						glShadeModel(GL_FLAT);
						break;
					case GIPSI_SHADE_SMOOTH:
						glShadeModel(GL_SMOOTH);
						break;
				}
				break;
		}

		// We can use interleaved arrays
		switch(mesh->header.dataType)
		{
			case 0x00:
				glInterleavedArrays(GL_V3F, 0, mesh->dispArray);
				break;
			case 0x01:
				glInterleavedArrays(GL_C3F_V3F, 0, mesh->dispArray);
				break;
			case 0x02:
				error_display(-1, "GUI: Unsupported data type!\n");
				break;
			case 0x04:
				glInterleavedArrays(GL_N3F_V3F, 0, mesh->dispArray);
				break;
			case 0x05:
				error_display(-1, "GUI: Unsupported data type!\n");
				break;
			case 0x06:
				glInterleavedArrays(GL_C4F_N3F_V3F, 0, mesh->dispArray);
				break;
			case 0x08:
				glInterleavedArrays(GL_T2F_V3F, 0, mesh->dispArray);
				break;
			case 0x09:
				glInterleavedArrays(GL_T2F_C3F_V3F, 0, mesh->dispArray);
				break;
			case 0x0a:
				error_display(-1, "GUI: Unsupported data type!\n");
				break;
			case 0x0c:
				glInterleavedArrays(GL_T2F_N3F_V3F, 0, mesh->dispArray);
				break;
			case 0x0d:
				error_display(-1, "GUI: Unsupported data type!\n");
				break;
			case 0x0e:
				glInterleavedArrays(GL_T2F_C4F_N3F_V3F, 0, mesh->dispArray);
				break;
			case 0x1c:
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_NORMAL_ARRAY);			// normal is specified
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);	// texture coords are speficied

				int stride = (2 + 3 + 3 + 3) * sizeof(float);

				int cnt = 0;
				glTexCoordPointer(2, GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 2;
				glNormalPointer(GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 3;
				glVertexPointer(3, GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 3;

				GLuint p = this->shader[this->activeShaderID]->GetCurrentProgram();
				GLint loc = glGetAttribLocation(p, "vTangent");
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, stride, mesh->dispArray + cnt);
				cnt+=3;
				break;
			}
			case 0x1d:
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);			// color (RGB) is specified
				glEnableClientState(GL_NORMAL_ARRAY);			// normal is specified
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);	// texture coords are speficied

				int stride = (2 + 3 + 3 + 3 + 3) * sizeof(float);

				int cnt = 0;
				glTexCoordPointer(2, GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 2;
				glColorPointer(3, GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 3;
				glNormalPointer(GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 3;
				glVertexPointer(3, GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 3;

				GLuint p = this->shader[this->activeShaderID]->GetCurrentProgram();
				GLint loc = glGetAttribLocation(p, "vTangent");
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, stride, mesh->dispArray + cnt);
				cnt+=3;
				break;
			}
			case 0x1e:
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glEnableClientState(GL_COLOR_ARRAY);			// color (RGBA) is specified
				glEnableClientState(GL_NORMAL_ARRAY);			// normal is specified
				glEnableClientState(GL_TEXTURE_COORD_ARRAY);	// texture coords are speficied

				int stride = (2 + 4 + 3 + 3 + 3) * sizeof(float);

				int cnt = 0;
				glTexCoordPointer(2, GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 2;
				glColorPointer(4, GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 4;
				glNormalPointer(GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 3;
				glVertexPointer(3, GL_FLOAT, stride, mesh->dispArray + cnt);
				cnt += 3;

				GLuint p = this->shader[this->activeShaderID]->GetCurrentProgram();
				GLint loc = glGetAttribLocation(p, "vTangent");
				glEnableVertexAttribArray(loc);
				glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, stride, mesh->dispArray + cnt);
				cnt+=3;
				break;
			}
			default:
			{
				int len = strlen("Unrecognized data type found ().") + 6;
				char * err = new char[len];
				char val[6];
				itoa(mesh->header.dataType, val, 16);
				sprintf_s(err, len, "Unrecognized data type found (%s).", val);

				throw new GiPSiException("Scene.DrawMesh", err);
				delete err;
				return;
			}
		}

		switch(mesh->header.objType) {
			case GIPSI_DRAW_POINT:
				glDrawElements(GL_POINTS, mesh->iA_size, GL_UNSIGNED_INT, mesh->indexArray);
				break;
			case GIPSI_DRAW_LINE:
				glDrawElements(GL_LINES, mesh->iA_size, GL_UNSIGNED_INT, mesh->indexArray);
				break;
			case GIPSI_DRAW_TRIANGLE:
				glDrawElements(GL_TRIANGLES, mesh->iA_size, GL_UNSIGNED_INT, mesh->indexArray);
				break;
			case GIPSI_DRAW_QUAD:
				glDrawElements(GL_QUADS, mesh->iA_size, GL_UNSIGNED_INT, mesh->indexArray);
				break;
			case GIPSI_DRAW_POLYGON:
				glDrawElements(GL_POLYGON, mesh->iA_size, GL_UNSIGNED_INT, mesh->indexArray);
				break;
		}
	}
}

/**
 * Draw a test cube.
 * 
 * @param x The X position of the cube to be drawn.
 * @param y The Y position of the cube to be drawn.
 * @param z The Z position of the cube to be drawn.
 */
void Scene::DrawTestCubeAt(float x, float y, float z)
{
	// Translate to position

	glPushMatrix();
	glTranslatef(x, y, z);

	// Draw the cube

	glutSolidCube(1.0);

	glPopMatrix();
}

/**
 * Draw a test triangle.
 * 
 * @param x The X position of the triangle to be drawn.
 * @param y The Y position of the triangle to be drawn.
 * @param z The Z position of the triangle to be drawn.
 */
void Scene::DrawTestTriangleAt(float x, float y, float z)
{
	// Translate to position

	glPushMatrix();
	glTranslatef(x, y, z);

	// Draw the triangle

	glBegin(GL_TRIANGLES);
		glTexCoord2d(0.0, 0.0); 
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);

		glTexCoord2d(1.0, 0.0);
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(2.0, 0.0, 0.0);

		glTexCoord2d(0.5, 1.0);
		glNormal3f(0.0, 0.7, 0.3);
		glVertex3f(1.2, 1.9, 0.0);
	glEnd();

	glPopMatrix();
}

/**
 * Return the number of camera.  
 */
int	 Scene::GetNumberOfCamera	(void)
{
	return nCamera;
}

/**
 * Return the current mode of the indexed camera.
 * 
 * @param id The index of the camera.
 */
int Scene::GetCameraMode(int id)
{
	return this->camera[id]->GetMode();
}

/**
 * Move the indexed camera by the specified amount.
 * 
 * @param id The index of the camera.
 * @param x Relative X amount to move the camera.
 * @param y Relative Y amount to move the camera.
 */
void Scene::MoveCamera(int id, float x, float y)
{
	switch(this->camera[id]->GetMode())
	{
	case CameraMode_Default:
		break;
	case CameraMode_Pan:
		this->camera[id]->Pan(x, y);
		break;
	case CameraMode_Planar:
		this->camera[id]->Planar(x, y);
		break;
	case CameraMode_Spherical:
		this->camera[id]->Spherical(x, y);
		break;
	case CameraMode_Zoom:
		this->camera[id]->Zoom(y);
		break;
	default:
		break;
	}
}

/**
 * Return Camera Type.
 *	0 : free camera
 *  1 : haptic attached camera
 *
 * @param id The index of the camera. 
 */
int	Scene::GetCameraType(int id)
{
	return this->camera[id]->getType();
}

/**
 * Return Attached Haptic Interface ID of Camera.
 * -1 : no attached Haptic Interface
 *
 * @param id The index of the camera. 
 */
unsigned int Scene::GetAttachedHapticInterfaceIDOfCamera(int id)
{
	return this->camera[id]->getAttachedHapticInterfaceID();
}

/**
 * Attach Haptic Interface to the indexed camera.
 * 
 * @param id The index of the camera.
 * @param HI HapticInterface pointer. 
 */
void Scene::AttachHapticInterfaceToCamera(int id, HapticInterface *HI)
{
	if (this->camera[id]->getType() == 1)
	{
		this->camera[id]->AttachHapticInterface(HI);
	}
}

/**
 * Display using perspective projection.
 * 
 * @param fov The horizontal view angle.
 * @param width Width of the viewport in world coordinates.
 * @param height Height of the viewport in world coordinates.
 */
void Scene::PerspectiveProjection(int cameraID, float fov, int width, int height)
{
	this->dim.width		= width;
	this->dim.height	= height;

	glViewport(0, 0, this->dim.width, this->dim.height);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(this->camera[cameraID]->PerspectiveProjection(fov, float(width)/float(height), 0.01, 1000));
}

/**
 * Render all objects in the scene.
 */
void Scene::Render(int cameraID)
{
	try
	{
		this->fps->End();
		this->fps->Begin();

		// Clear screen

	#ifdef _DEBUG

		glClearColor(0.0, 0.2, 0.0, 1.0);

	#else

		glClearColor(0.0, 0.0, 0.0, 1.0);

	#endif

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Set camera

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(this->camera[cameraID]->ViewingTransformation());

		// Update light sources
		for (int i = 0; i < this->nLight; i++)
		{
			float lightPos[4];
			// Update attached light positions
			if (this->light[i]->type == ATTACHED &&
				this->light[i]->attached)
			{
				float newPos[4];
				light[i]->attached->GetPosition(newPos[0], newPos[1], newPos[2]);
				for (int j = 0; j < 3; j++)
					light[i]->pos[j] = (Real)newPos[j];
			}

			// Enable and position opengl light sources
			lightPos[3] = 1.0;
			for (int j = 0; j < 3; j++)
				lightPos[j] = (float)this->light[i]->pos[j];
			glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		}

		// Coordinate system
		if (this->displayCoordinateSystem == true) { DrawCoordinateSystemAt(0, 0, 0); }

		// Update all textures
		for (int i = 0; i < this->nTexture; i++)
		{
			this->texture[i]->UpdateTexture();
		}

		// Display all objects
		for (int i = 0; i < this->nBuffer; i++)
		{
			for (int j = 0; j < this->buffer[i]->GetNumTextures(); j++)
			{
				this->SelectTexture(this->buffer[i]->GetTexture(j), j);
			}
			if (this->useFragmentPixelShaders == true)
			{
				if (this->buffer[i]->GetShaderParams() != NULL)
					this->SelectShader(this->buffer[i]->GetShaderParams(), 0);
			}
			this->DrawMesh(i);
		}

		// Show frames per second
		if (this->useFragmentPixelShaders == true)
		{
			this->DeselectShader();
		}
		this->DeselectAllTextures();
		this->fps->Display(this->dim.width, this->dim.height);

		// Display scene
		glutSwapBuffers();
		glutPostRedisplay();

		// Clean up opengl light sources
		for (int i = 0; i < this->nLight; i++)
			glDisable(GL_LIGHT0 + i);

		//this->fps->End();
	}
	catch (...)
	{
		throw;
		return;
	}
}

/**
 * Placeholder text.
 */
void Scene::RenderToTextureBegin(const int id)
{
	this->texture[id]->SetViewPortToSizeOf();
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * Placeholder text.
 */
void Scene::RenderToTextureEnd(const int id)
{
	this->texture[id]->OverrideWithFrameBuffer();

	glViewport(0, 0, this->dim.width, this->dim.height);
	glClearColor(0.0, 0.5, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
 * Select specified shader.
 * 
 * @param params Parameters for to use for shader.
 * @param nthPass Pass number (for multi-pass shaders).
 */
void Scene::SelectShader(ShaderParams * params, const int nthPass)
{
	if (this->useFragmentPixelShaders == true)
	{
		this->activeShaderID	= 0;

		// Obtain the ID of the chosen shader
		for (int i = 0; i < this->nShader; i++)
		{
			if (this->shader[i]->GetShaderName() == params->GetShaderName())
			{
				// We've found the shader
				// Now set activeShaderID (no idea what it does) and select the shader
				this->activeShaderID = i;
				this->shader[this->activeShaderID]->Select(nthPass);

				// And set the shader params
				this->shader[this->activeShaderID]->SetParameters(params);
				return;
			}
		}

		// We haven't found the shader (error and exit)
		throw new GiPSiException("Scene.SelectShader", "Shader not found in scene shader list.");
	}
}

/**
 * Select specified texture.
 * 
 * @param name Texture name.
 * @param unit Texture unit.
 */
void Scene::SelectTexture(const char * name, const int unit)
{
	// Obtain the ID of the chosen texture

	bool validTextureID	= false;
	int	 id				= 0;

	for (int i=0; i<this->nTexture; i++)
	{
		const char * textureName = this->texture[i]->GetObjectName();

		if (strcmp(textureName, name) == 0)
		{
			if (!validTextureID)
			{
				validTextureID	= true;
				id				= i;
			}
			else
			{
				validTextureID = false;
				break;
			}
		}
	}

	// Make sure the chosen texture unit is within the legal range, this OpenGL
	// implementation has support for 8 active textures. (4 on the lab computer.)

	bool validTextureUnit = false;
	
	int maxTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);

	if (unit >= 0 && unit < maxTextureUnits)
	{
		validTextureUnit = true;
	}

	// Select the texture

	if (validTextureID && validTextureUnit)
	{
		glActiveTexture(GL_TEXTURE0 + unit);
		this->texture[id]->Select();
	}
	else
	{
		error_exit(1002, "Dublicate texture name or invalid texture unit");
	}
}

/**
 * Set mode of indexed camera.
 * 
 * @param id Camera index.
 * @param cameraMode New mode for to place camera in.
 */
void Scene::SetCameraMode(const int id, CameraMode cameraMode)
{
	this->camera[id]->SetMode(cameraMode);
}

/**
 * Toggle display of coordinate system.
 */
void Scene::ToggleCoordinateSystem(void)
{
	this->displayCoordinateSystem = !(this->displayCoordinateSystem);
}
