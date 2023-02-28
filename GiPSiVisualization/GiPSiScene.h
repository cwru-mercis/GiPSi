/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Scene implementation (GiPSiScene.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

#ifndef GIPSISCENE__H
#define GIPSISCENE__H

/*
===============================================================================
	Headers
===============================================================================
*/

#include "GiPSiCamera.h"
#include "GiPSiShader.h"
#include "GiPSiTexture.h"
#include "GiPSiDisplay.h"

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

	void DrawString(char *str, float x, float y, int width, int height);
};

/*
===============================================================================
	Scene class
===============================================================================
*/

struct Dimensions
{
	int	width,
	int height;
};

class Scene
{
public:
	Scene(int nMesh, DisplayArray **mesh,
		  int nTexture, Texture **texture,
		  int nShader, Shader **shader);

	int	 GetCameraMode	(int id);
	void SetCameraMode	(int id, CameraMode cameraMode);
	void MoveCamera		(int id, float x, float y);

	void Render(void);
	void ToggleCoordinateSystem(void);
	void PerspectiveProjection(float fov, int width, int height);

protected:

private:
	int				  nCamera;
	Camera			**camera;
	int				  nMesh;
	DisplayArray	**mesh;
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

	void SelectTexture				(const TextureName name, const int unit);
	void DeselectAllTextures		(void);

	void SelectShader				(const ShaderName name, const int nthPass);
	void DeselectShader				(void);

	void RenderToTextureBegin		(const int id);
	void RenderToTextureEnd			(const int id);

	// The below are specific
	//  to the shaders present
	void SetParametersPhongShader	(const int halfWayApprox, const int texUnitBase);
	void SetParametersBumpShader	(const int texUnitBase, const int texUnitHeight);
	void SetParametersTissueShader	(	const int texUnitBase,
										const int texUnitHeightconst,
										const float ambientContribution,
										const float diffuseContribution,
										const float specularContribution,
										const float glossiness,
										const float stepSize,
										const float bumpiness,
										const float opacity,
										const float displacement				);
};

#endif // #ifndef GIPSISCENE__H