/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Camera implementation (GiPSiCamera.cpp).

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

#include "algebra.h"
#include "GiPSiCamera.h"

/*
===============================================================================
	Camera
===============================================================================
*/

Camera::Camera(void)
{
	this->Reset();
}

CameraMode Camera::GetMode(void)
{
	return this->mode;
}

void Camera::GetPosition(float &x, float &y, float &z)
{
	x = pos.begin()[0];
	y = pos.begin()[1];
	z = pos.begin()[2];
}

void Camera::Pan(float x, float y)
{
	Vector<Real> n, v, u;

	n = pos - lookAt;
	v = up;
	u = Vector<Real>(3);
	crossVV(u, n, v);
	u *= -1;

	n.normalize();
	v.normalize();
	u.normalize();

	u *= x;
	v *= y;

	lookAt	= lookAt + u + v;
	pos		= pos + u + v;
}

void Camera::Planar(float x, float y)
{
	Vector<Real> n, v, u;

	n = pos - lookAt;
	v = up;
	u = Vector<Real>(3);
	crossVV(u, n, v);
	u *= -1;

	n.normalize();
	u.normalize();

	n *= y;
	u *= x;

	lookAt	= lookAt - n + u;
	pos		= pos	 - n;
}

float *Camera::PerspectiveProjection(float fovy, float aspect, float zNear, float zFar)
{
	Vector<Real>	prp, vp;
	float			fovyRad, f;

	fovyRad	= fovy * 2 * M_PI / 360;
	f		= 1 / tan(fovyRad/2);

	PP[0] = f/aspect;	PP[4] = 0;		PP[8]  = 0;							PP[12] = 0;
	PP[1] = 0;			PP[5] = f;		PP[9]  = 0;							PP[13] = 0;
	PP[2] = 0;			PP[6] = 0;		PP[10] = (zFar+zNear)/(zNear-zFar);	PP[14] = (2*zFar*zNear)/(zNear-zFar);
	PP[3] = 0;			PP[7] = 0;		PP[11] = -1;						PP[15] = 0;

	return PP;
}

void Camera::Reset(void)
{
	mode	= CameraMode_Default;

	pos		= Vector<Real>(3, "0.0 0.0 45.0");
	lookAt	= Vector<Real>(3, "0.0 0.0 0.0");
	up		= Vector<Real>(3, "0.0 1.0 0.0");
}

void Camera::SetMode(CameraMode mode)
{
	this->mode = mode;
}

void Camera::Specify(Vector<Real> pos, Vector<Real> lookAt, Vector<Real> up)
{
	this->pos	 = pos;
	this->lookAt = lookAt;
	this->up	 = up;
}

void Camera::Spherical(float x, float y)
{
	/*
	This function moves the camera around the a center point as if it was moving
	on the surface of an invisible sphere. The center point depends on how far
	zoomed in or out the camera is. The zooming is done by the function Zoom.
	*/

	float oldPitch, oldYaw, newPitch, newYaw, radius, planarRadius;

	radius			= (pos - lookAt).length();
	planarRadius	= sqrt((pos[0]-lookAt[0])*(pos[0]-lookAt[0]) +
						   (pos[2]-lookAt[2])*(pos[2]-lookAt[2]));

	// Determine current angles

	oldPitch = atan((pos[1]-lookAt[1]) / planarRadius);

	if (pos[2]>lookAt[2])
	{
		oldYaw = atan((pos[0]-lookAt[0]) / (pos[2]-lookAt[2]));
	}
	else
	{
		oldYaw = float(M_PI) + atan((lookAt[0]-pos[0]) / (lookAt[2]-pos[2]));
	}

	// Assign new angles

	newPitch	= oldPitch + y;
	newYaw		= oldYaw + x;

	// Avoid schizophrenia

	if (newPitch < -float(M_PI) / 2.0f + 0.05f)	{ newPitch = -float(M_PI) / 2 + 0.05f; }
	if (newPitch >  float(M_PI) / 2.0f - 0.05f)	{ newPitch =  float(M_PI) / 2 - 0.05f; }

	// Calculate new position

	pos[0] = lookAt[0] + radius * sin(newYaw) * cos(newPitch);
	pos[1] = lookAt[1] + radius * sin(newPitch);
	pos[2] = lookAt[2] + radius * cos(newYaw) * cos(newPitch);

	// Calculate new up vector

	up[0] = -sin(newYaw) * sin(newPitch);
	up[1] = cos(newPitch);
	up[2] = -cos(newYaw) * sin(newPitch);
	up.normalize();
}

float *Camera::ViewingTransformation(void)
{
	Vector<Real> u, v, n;

	// Obtain u, v & n vectors

	n = pos - lookAt;
	v = up;
	u = Vector<Real>(3);
	crossVV(u, n, v);
	u *= -1;

	n.normalize();
	v.normalize();
	u.normalize();

	// Define the Model View Matrix - column major order

	VT[0] = u.begin()[0];	VT[4] = u.begin()[1];	VT[8]  = u.begin()[2];	VT[12] = -(u*pos);
	VT[1] = v.begin()[0];	VT[5] = v.begin()[1];	VT[9]  = v.begin()[2];	VT[13] = -(v*pos);
	VT[2] = n.begin()[0];	VT[6] = n.begin()[1];	VT[10] = n.begin()[2];	VT[14] = -(n*pos);
	VT[3] = 0;				VT[7] = 0;				VT[11] = 0;				VT[15] = 1;
	
	return VT;
}

void Camera::Zoom(float y)
{
	Vector<Real> n;

	// Obtain n vector

	n = pos - lookAt;
	n.normalize();

	// Scale the n vector and perform the zoom by moving the position along the n vector

	n *= y;
	pos = pos - n;
}