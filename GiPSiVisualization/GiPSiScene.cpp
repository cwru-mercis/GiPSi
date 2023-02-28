/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Scene implementation (GiPSiScene.cpp).

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

#include <GL/glut.h>		// NOTE: Ideally GiPSiScene would only use gl.h, glut.h should only be used in GlutVisualizationEngine
#include <stdio.h>
#include <timing.h>

#include "GiPSiScene.h"

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
}

void FramesPerSecond::End(void)
{
	time += get_timer(0);
	count++;
	sprintf(str, "fps: %.2f ", fhz(time/count));
}

/*
===============================================================================
	Scene
===============================================================================
*/

Scene::Scene(int nMesh, DisplayArray **mesh,
			 int nTexture, Texture **texture,
			 int nShader, Shader **shader)
{
	this->nMesh		= nMesh;
	this->mesh		= mesh;
	this->nTexture	= nTexture;
	this->texture	= texture;
	this->nShader	= nShader;
	this->shader	= shader;

	fps = new FramesPerSecond();

	displayCoordinateSystem = true;
	useFragmentPixelShaders = true;

	this->nCamera	= 1;
	this->camera	= new Camera*[this->nCamera];
	this->camera[0] = new Camera();
}

void Scene::DeselectShader(void)
{
	if (this->useFragmentPixelShaders == true)
	{
		Shader::UseFixedPipeline();
	}
}

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

void Scene::DrawMesh(int n)
{
	static DisplayArray *mesh;

	mesh = this->mesh[n];

	if (mesh != NULL)
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

void Scene::DrawTestCubeAt(float x, float y, float z)
{
	// Translate to position

	glPushMatrix();
	glTranslatef(x, y, z);

	// Draw the cube

	glutSolidCube(1.0);

	glPopMatrix();
}

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

int Scene::GetCameraMode(int id)
{
	return this->camera[id]->GetMode();
}

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

void Scene::PerspectiveProjection(float fov, int width, int height)
{
	this->dim.width		= width;
	this->dim.height	= height;

	glViewport(0, 0, this->dim.width, this->dim.height);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(this->camera[0]->PerspectiveProjection(fov, float(width)/float(height), 0.01, 1000));
}

void Scene::Render(void)
{
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
	glLoadMatrixf(this->camera[0]->ViewingTransformation());

	// Update light sources

	float lightPos[4];
	this->camera[0]->GetPosition(lightPos[0], lightPos[1], lightPos[2]);
	lightPos[3] = 1.0;
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	// Coordinate system

	if (this->displayCoordinateSystem == true) { DrawCoordinateSystemAt(0, 0, 0); }

	// *******************************************
	// *******************************************
	//  APPLICATION SPECIFIC PART BEGINS HERE
	// *******************************************
	// *******************************************

	// Ventricle system

	this->SelectTexture(TextureName_VentricleSystemBase,		0);
	this->SelectTexture(TextureName_VentricleSystemHeightMap,	1);
	this->SelectShader(ShaderName_Tissue, 0);
	this->SetParametersTissueShader(0, 1, 0.4, 0.6, 0.3, 32.0, 0.003, 1.0, 1.0, 0.0);
		this->DrawMesh(3);
			
	// Choroid Plexus

	this->SelectTexture(TextureName_ChoroidPlexusBase,		2);
	this->SelectTexture(TextureName_ChoroidPlexusHeightMap, 3);
	this->SelectShader(ShaderName_Tissue, 0);
	this->SetParametersTissueShader(2, 3, 0.3, 0.5, 0.5, 128.0, 0.004, 5.0, 1.0, 0.0);
		this->DrawMesh(4);
		this->DrawMesh(11);

	// Anterior Septal Vein, Thalamostriate Vein and Another Vein

	this->SelectTexture(TextureName_AnteriorSeptalVeinBase,			0);
	this->SelectTexture(TextureName_AnteriorSeptalVeinHeightMap,	1);
	this->SelectShader(ShaderName_Tissue, 0);
	this->SetParametersTissueShader(0, 1, 0.5, 0.5, 0.3, 128.0, 0.004, 1.0, 1.0, 0.0);
		this->DrawMesh(5);
		this->DrawMesh(6);
		this->DrawMesh(7);
		this->DrawMesh(12);
		this->DrawMesh(13);
		this->DrawMesh(14);
	this->SetParametersTissueShader(0, 1, 0.3, 0.5, 1.0, 256.0, 0.004, 1.0, 0.2, 0.005);
		this->DrawMesh(5);
		this->DrawMesh(6);
		this->DrawMesh(7);
		this->DrawMesh(12);
		this->DrawMesh(13);
		this->DrawMesh(14);

	// Basilar Artery

	this->SelectTexture(TextureName_BasilarArteryBase,		0);
	this->SelectTexture(TextureName_BasilarArteryHeightMap,	1);
	this->SelectShader(ShaderName_Tissue, 0);
	this->SetParametersTissueShader(0, 1, 0.5, 0.5, 0.3, 128.0, 0.004, 1.0, 1.0, 0.0);
		this->DrawMesh(8);

	// Mammillary Bodies

	this->SelectTexture(TextureName_MammillarryBodiesBase,		0);
	this->SelectTexture(TextureName_MammillarryBodiesHeightMap,	1);
	this->SelectShader(ShaderName_Tissue, 0);
	this->SetParametersTissueShader(0, 1, 0.5, 0.9, 0.5, 128.0, 0.003, 2.0, 1.0, 0.0);
		this->DrawMesh(10);

	// Black cover

	this->SetParametersTissueShader(0, 1, 0.0, 0.0, 0.0, 0.0, 0.003, 0.0, 1.0, 0.0);
		this->DrawMesh(15);

	// Ventricle Floor
	// NOTE: Transparent objects should be rendered last

	this->SelectTexture(TextureName_VentricleFloorBase,			0);
	this->SelectTexture(TextureName_VentricleFloorHeightMap,	1);
	this->SelectShader(ShaderName_Tissue, 0);
	this->SetParametersTissueShader(0, 1, 0.4, 0.6, 0.3, 32.0, 0.003, 1.0, 1.0, 0.0);
		this->DrawMesh(9);

	// *******************************************
	// *******************************************
	//  APPLICATION SPECIFIC PART ENDS HERE
	// *******************************************
	// *******************************************

	// Show frames per second

	this->DeselectShader();
	this->DeselectAllTextures();
	this->fps->Display(this->dim.width, this->dim.height);

	// Display scene

	glutSwapBuffers();
	glutPostRedisplay();

	this->fps->End();
}

void Scene::RenderToTextureBegin(const int id)
{
	this->texture[id]->SetViewPortToSizeOf();
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::RenderToTextureEnd(const int id)
{
	this->texture[id]->OverrideWithFrameBuffer();

	glViewport(0, 0, this->dim.width, this->dim.height);
	glClearColor(0.0, 0.5, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene::SelectShader(const ShaderName name, const int nthPass)
{
	if (this->useFragmentPixelShaders == true)
	{
		bool validShaderID		= false;
		this->activeShaderID	= 0;

		// Obtain the ID of the chosen shader

		for (int i=0; i<this->nShader; i++)
		{
			ShaderName shaderName = this->shader[i]->GetName();

			if (shaderName == name)
			{
				if (!validShaderID)
				{
					validShaderID			= true;
					this->activeShaderID	= i;
				}
				else
				{
					validShaderID = false;
					break;
				}
			}
		}

		// Select the shader

		if (validShaderID == true)
		{
			this->shader[this->activeShaderID]->Select(nthPass);
		}
		else
		{
			error_exit(1004, "Invalid or dublicate shader ID");
		}
	}
}

void Scene::SelectTexture(const TextureName name, const int unit)
{
	// Obtain the ID of the chosen texture

	bool validTextureID	= false;
	int	 id				= 0;

	for (int i=0; i<this->nTexture; i++)
	{
		TextureName textureName = this->texture[i]->GetName();

		if (textureName == name)
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

void Scene::SetCameraMode(const int id, CameraMode cameraMode)
{
	this->camera[id]->SetMode(cameraMode);
}

void Scene::ToggleCoordinateSystem(void)
{
	this->displayCoordinateSystem = !(this->displayCoordinateSystem);
}

// *******************************************
// *******************************************
//  CODE BELOW HERE 
//   IS FOR SPECIFIC SHADERS PRESENT
// *******************************************
// *******************************************

void Scene::SetParametersBumpShader(const int texUnitBase, const int texUnitHeight)
{
	((BumpShader *) this->shader[this->activeShaderID])->SetParameters(texUnitBase, texUnitHeight);
}

void Scene::SetParametersPhongShader(const int halfWayApprox, const int texUnitBase)
{
	((PhongShader *) this->shader[this->activeShaderID])->SetParameters(halfWayApprox, texUnitBase);
}

void Scene::SetParametersTissueShader(	const int	texUnitBase,
										const int	texUnitHeightconst,
										const float ambientContribution,
										const float diffuseContribution,
										const float specularContribution,
										const float glossiness,
										const float stepSize,
										const float bumpiness,
										const float opacity,
										const float displacement )
{
	((TissueShader *) this->shader[this->activeShaderID])->SetParameters(	texUnitBase, texUnitHeightconst, ambientContribution, diffuseContribution,
																			specularContribution, glossiness, stepSize, bumpiness, opacity, displacement);
}