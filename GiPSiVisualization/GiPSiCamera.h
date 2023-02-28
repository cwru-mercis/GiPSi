/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Camera implementation (GiPSiCamera.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

#ifndef GIPSICAMERA__H
#define GIPSICAMERA__H

/*
===============================================================================
	Headers
===============================================================================
*/

#include "algebra.h"

/*
===============================================================================
	Basic camera class
===============================================================================
*/

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
	Camera(void);
	
	CameraMode	GetMode(void);
	void		SetMode(CameraMode mode);

	void	Pan			(float x, float y);
	void	Planar		(float x, float y);
	void	Spherical	(float x, float y);
	void	Zoom		(float y);

	void	Reset(void);
	void	GetPosition(float &x, float &y, float &z);
	void	Specify(Vector<Real> pos, Vector<Real> lookAt, Vector<Real> up);

	float   *PerspectiveProjection(float fovy, float aspect, float zNear, float zFar);
	float   *ViewingTransformation(void);
	

protected:

private:
	CameraMode		mode;
	
	Vector<Real>	pos;
	Vector<Real>	lookAt;
	Vector<Real>	up;
	
	float			VT[16];
	float			PP[16];
	
};

#endif // #ifndef GIPSICAMERA__H