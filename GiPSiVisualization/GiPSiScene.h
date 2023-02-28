/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Scene Definition (GiPSiScene.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen, Nathan Brown.
*/

////	GIPSISCENE.H v0.0
////
////	GiPSi Scene
////
////////////////////////////////////////////////////////////////

#ifndef GIPSISCENE__H
#define GIPSISCENE__H

/*
===============================================================================
	Headers
===============================================================================
*/

#define WIN32_LEAN_AND_MEAN

#include "GiPSiAPI.h"
#include "GiPSiCamera.h"
#include "GiPSiDisplay.h"
#include "GiPSiLight.h"
#include "GiPSiShader.h"
#include "GiPSiTexture.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;

/*
===============================================================================
	Frames per second class
===============================================================================
*/

class FramesPerSecond
{
public:
	FramesPerSecond(void);

	void Display	(int width, int height);
	void Begin		(void);
	void End		(void);

private:
	char	str[256];
	long	count;
	double	time;
	bool	flag;

	void DrawString(char *str, float x, float y, int width, int height);
};

/*
===============================================================================
	Scene class
===============================================================================
*/

struct Dimensions
{
	int	width;
	int height;
};

class Scene
{
public:
	Scene(XMLNode * sceneNode,
		  int nObject, DisplayBuffer **object,
		  int nTexture, Texture **texture,
		  int nShader, Shader **shader);
	~Scene()
	{
		// Do not delete the objects themselves; they belong to the VE
		if (shader)		delete shader;
		if (texture)	delete texture;
		if (buffer)		delete buffer;
		if (camera)		delete camera;
		if (light)		delete light;
	}

	int	 GetNumberOfCamera	(void);
	int	 GetCameraMode	(int id);
	void SetCameraMode	(int id, CameraMode cameraMode);
	void MoveCamera		(int id, float x, float y);
	int	 GetCameraType  (int id);
	unsigned int GetAttachedHapticInterfaceIDOfCamera(int id);
	void AttachHapticInterfaceToCamera(int id, HapticInterface *HI);

	void Render(int cameraID);
	void ToggleCoordinateSystem(void);
	void PerspectiveProjection(int cameraID, float fov, int width, int height);

protected:
	friend LoaderUnitTest;

private:
	int				  nCamera;
	Camera			**camera;
	int				  nLight;
	Light			**light;
	int				  nBuffer;
	DisplayBuffer	**buffer;
	int				  nTexture;
	Texture			**texture;
	int				  nShader;
	Shader			**shader;

	Dimensions		  dim;
	FramesPerSecond	 *fps;

	int				  activeShaderID;
	bool			  displayCoordinateSystem;
	bool			  useFragmentPixelShaders;
	
	void DrawCoordinateSystemAt		(float x, float y, float z);
	void DrawMesh					(int n);
	void DrawTestCubeAt				(float x, float y, float z);
	void DrawTestTriangleAt			(float x, float y, float z);

	void SelectTexture				(const char * name, const int unit);
	void DeselectAllTextures		(void);

	void SelectShader				(ShaderParams * params, const int nthPass);
	void DeselectShader				(void);

	void RenderToTextureBegin		(const int id);
	void RenderToTextureEnd			(const int id);
};

#endif // #ifndef GIPSISCENE__H
