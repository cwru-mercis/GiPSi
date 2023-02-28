/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Camera Definition (GiPSiCamera.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen, Suriya Natsupakpong.
*/

////	GIPSICAMERA.H v0.0
////
////	GiPSi Camera
////
////////////////////////////////////////////////////////////////

#ifndef GIPSICAMERA__H
#define GIPSICAMERA__H

/*
===============================================================================
	Headers
===============================================================================
*/

#include "algebra.h"
#include "GiPSiHaptics.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class Light;

/*
===============================================================================
	Basic camera class
===============================================================================
*/

class LoaderUnitTest;

enum CameraMode
{
	CameraMode_Default,
	CameraMode_Pan,
	CameraMode_Planar,
	CameraMode_Spherical,
	CameraMode_Zoom
};

class Camera
{
public:
	Camera(XMLNode * cameraNode);
	~Camera();
	
	CameraMode	GetMode(void);
	void		SetMode(CameraMode mode);

	void		Pan			(float x, float y);
	void		Planar		(float x, float y);
	void		Spherical	(float x, float y);
	void		Zoom		(float y);

	void		Reset(void);
	void		GetPosition(float &x, float &y, float &z);
	void		Specify(Vector<Real> pos, Vector<Real> lookAt, Vector<Real> up);

	float      *PerspectiveProjection(float fovy, float aspect, float zNear, float zFar);
	float	   *ViewingTransformation(void);

	void		AddLight(Light * newLight);

	int				getType(void);
	unsigned int	getAttachedHapticInterfaceID(void);
	void			AttachHapticInterface(HapticInterface *HI);
		
	const char		*GetName() { return name; }

protected:

private:
	char			   *name;
	int					type;		// 0: free, 1:haptic
	CameraMode			mode;	
	unsigned int		attachedID;	// HapticInterfaceID
	HapticInterface	   *attached;
	Light			   **light;
	int nLights;
	
	Vector<Real>		pos;
	Vector<Real>		lookAt;
	Vector<Real>		up;

	Matrix<Real>		g_w_lh;	// Transformation from local HI to world
	Matrix<Real>		g_h_v;	// Transformation from viewing camera to HI
	Matrix<Real>		GetTransformationParameter(XMLNode * transformationNode);
	//Vector<Real>		oldHIpos;

	Real				fov;
	
	float				VT[16];
	float				PP[16];

	friend LoaderUnitTest;
};

#endif // #ifndef GIPSICAMERA__H