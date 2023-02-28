/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Camera Implementation (GiPSiCamera.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen, Suriya Natsupakpong
*/

/*
===============================================================================
	Headers
===============================================================================
*/

#include "algebra.h"
#include "GiPSiCamera.h"
#include "GiPSiException.h"
#include "GiPSiLight.h"
#include "XMLNodeList.h"

/*
===============================================================================
	Camera
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param cameraNode XML project "Camera" node for initialization.
 */
Camera::Camera(XMLNode * cameraNode) :
	light(NULL),
	nLights(0),
	attached(NULL)
{
	pos = zero_vector3;
	lookAt = zero_vector3;
	up = zero_vector3;
	g_w_lh = zero_matrix4;
	g_h_v = zero_matrix4;

	try
	{
		mode	= CameraMode_Default;

		// Set camera parameters
		XMLNodeList * cameraChildren = cameraNode->GetChildren();

		// name
		XMLNode * nameNode = cameraChildren->GetNode("name");
		name = nameNode->GetValue();
		delete nameNode;

		// type
		XMLNode * typeNode = cameraChildren->GetNode("type");
		char * typeVal = typeNode->GetValue();
		if (strcmp(typeVal, "FREE") == 0)
			type = 0;
		else if (strcmp(typeVal, "ATTACHED") == 0)
			type = 1;
		else
		{
			int errlen = strlen(typeVal) + 28;
			char * err = new char[errlen];
			sprintf_s(err, errlen, "Unknown camera type found: %s", typeVal);
			throw new GiPSiException("Camera constructor", err);
			delete err;
			delete typeVal;
			delete typeNode;
			delete cameraChildren;;
			return;
		}
		delete typeVal;
		delete typeNode;		

		if (type==0) {	// camera is free

			// position
			XMLNode * positionNode = cameraChildren->GetNode("position");
			XMLNodeList * positionChildren = positionNode->GetChildren();
			pos			= Vector<Real>(3);
			XMLNode * posxNode = positionChildren->GetNode("x");
			XMLNode * posyNode = positionChildren->GetNode("y");
			XMLNode * poszNode = positionChildren->GetNode("z");
			const char * posx = posxNode->GetValue();
			const char * posy = posyNode->GetValue();
			const char * posz = poszNode->GetValue();
			pos[0]		= (Real)atof(posx);
			pos[1]		= (Real)atof(posy);
			pos[2]		= (Real)atof(posz);
			delete posxNode;
			delete posyNode;
			delete poszNode;
			delete positionChildren;
			delete positionNode;
			delete posx;
			delete posy;
			delete posz;

			// look vector
			XMLNode * lookAtNode = cameraChildren->GetNode("lookAt");
			XMLNodeList * lookAtChildren = lookAtNode->GetChildren();
			lookAt		= Vector<Real>(3);
			XMLNode * lookxNode = lookAtChildren->GetNode("x");
			XMLNode * lookyNode = lookAtChildren->GetNode("y");
			XMLNode * lookzNode = lookAtChildren->GetNode("z");
			const char * lookx = lookxNode->GetValue();
			const char * looky = lookyNode->GetValue();
			const char * lookz = lookzNode->GetValue();
			lookAt[0]	= (Real)atof(lookx);
			lookAt[1]	= (Real)atof(looky);
			lookAt[2]	= (Real)atof(lookz);
			delete lookxNode;
			delete lookyNode;
			delete lookzNode;
			delete lookAtChildren;
			delete lookAtNode;
			delete lookx;
			delete looky;
			delete lookz;

			// up vector
			XMLNode * upVectorNode = cameraChildren->GetNode("upVector");
			XMLNodeList * upVectorChildren = upVectorNode->GetChildren();
			up			= Vector<Real>(3);
			XMLNode * upxNode = upVectorChildren->GetNode("x");
			XMLNode * upyNode = upVectorChildren->GetNode("y");
			XMLNode * upzNode = upVectorChildren->GetNode("z");
			const char * upx = upxNode->GetValue();
			const char * upy = upyNode->GetValue();
			const char * upz = upzNode->GetValue();
			up[0]		= (Real)atof(upx);
			up[1]		= (Real)atof(upy);
			up[2]		= (Real)atof(upz);
			delete upxNode;
			delete upyNode;
			delete upzNode;
			delete upVectorChildren;
			delete upVectorNode;
			delete upx;
			delete upy;
			delete upz;

			attachedID = -1;
		}

		else {	// camera is attached to haptic interface

			XMLNode * hapticInterfaceIDNode = cameraChildren->GetNode("attachedHapticInterfaceID");
			char * hiIDVal = hapticInterfaceIDNode->GetValue();
			attachedID = atoi(hiIDVal);
			delete hiIDVal;
			delete hapticInterfaceIDNode;

			XMLNode * baseToWorldTransformationNode = cameraChildren->GetNode("baseToWorldTransformation");
			g_w_lh = GetTransformationParameter(baseToWorldTransformationNode);
			delete baseToWorldTransformationNode;

			XMLNode * cameraToHapticTransformationNode = cameraChildren->GetNode("cameraToHapticTransformation");
			g_h_v = GetTransformationParameter(cameraToHapticTransformationNode);
			delete cameraToHapticTransformationNode;
		}

		// field of view
		XMLNode * fieldOfViewNode = cameraChildren->GetNode("fieldOfView");
		const char * fieldOfViewString = fieldOfViewNode->GetValue();
		fov = (Real)atof(fieldOfViewString);
		delete fieldOfViewString;
		delete fieldOfViewNode;		

		delete cameraChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}

/**
 * Destructor.
 */
Camera::~Camera()
{
	if (name)
	{
		delete name;
		name = NULL;
	}
}

/**
 * GetTransformationParameter from transformation node.
 * return tranformation matrix 4x4
 */
Matrix<Real> Camera::GetTransformationParameter(XMLNode * transformationNode)
{
	Matrix<Real> G(4,4,0.0);
	Matrix<Real> R(3,3,0.0);
	Vector<Real> t(3,0.0);
	Vector<Real> s(3,1.0);

	try
	{	
		XMLNodeList * transformationChildren = transformationNode->GetChildren();

		// Extract scaling information
		XMLNode * scalingNode = transformationChildren->GetNode("scaling");
		XMLNodeList * scalingChildren = scalingNode->GetChildren();

		// Extract scaling components
		XMLNode * sxNode = scalingChildren->GetNode("x");
		const char * sx = sxNode->GetValue();
		XMLNode * syNode = scalingChildren->GetNode("y");
		const char * sy = syNode->GetValue();
		XMLNode * szNode = scalingChildren->GetNode("z");
		const char * sz = szNode->GetValue();
		// Set scaling
		s[0] = (float)atof(sx);
		s[1] = (float)atof(sy);
		s[2] = (float)atof(sz);
		delete sx;
		delete sy;
		delete sz;

		// Extract rotation information
		XMLNode * rotationNode = transformationChildren->GetNode("rotation");
		XMLNodeList * rotationChildren = rotationNode->GetChildren();
		XMLNode * axisRotationNode = rotationChildren->GetNode("axisRotation");
		XMLNodeList * axisRotationChildren = axisRotationNode->GetChildren();
		XMLNode * axisNode = axisRotationChildren->GetNode("axis");
		XMLNodeList * axisChildren = axisNode->GetChildren();

		// Extract rotation components
		XMLNode * rxNode = axisChildren->GetNode("x");
		const char * rx = rxNode->GetValue();
		XMLNode * ryNode = axisChildren->GetNode("y");
		const char * ry = ryNode->GetValue();
		XMLNode * rzNode = axisChildren->GetNode("z");
		const char * rz = rzNode->GetValue();
		XMLNode * rangleNode = axisRotationChildren->GetNode("angle");
		const char * rangle = rangleNode->GetValue();
		// Set rotate
		ToRotationMatrix(R, TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));
		delete rangle;
		delete rx;
		delete ry;
		delete rz;

		// Extract translation information
		XMLNode * translationNode = transformationChildren->GetNode("translation");
		XMLNodeList * translationChildren = translationNode->GetChildren();

		// Extract translation components
		XMLNode * dxNode = translationChildren->GetNode("x");
		const char * dx = dxNode->GetValue();
		XMLNode * dyNode = translationChildren->GetNode("y");
		const char * dy = dyNode->GetValue();
		XMLNode * dzNode = translationChildren->GetNode("z");
		const char * dz = dzNode->GetValue();
		// Set translate
		t[0] = (float)atof(dx);
		t[1] = (float)atof(dy);
		t[2] = (float)atof(dz);
		delete dx;
		delete dy;
		delete dz;

		ToTransformationMatrix(G, R, t, s);
		return G;
	}
	catch (...)
	{
		throw;
		return G;
	}	
}

/**
 * Return current camera mode.
 */
CameraMode Camera::GetMode(void)
{
	return this->mode;
}

/**
 * Retrieve current camera position.
 * 
 * @param x Is set to current camera X position.
 * @param y Is set to current camera Y position.
 * @param z Is set to current camera Z position.
 */
void Camera::GetPosition(float &x, float &y, float &z)
{
	x = pos.begin()[0];
	y = pos.begin()[1];
	z = pos.begin()[2];
}

/**
 * Pan camera by input amount.
 * 
 * @param x X change in screen coordinates.
 * @param y Y change in screen coordinates.
 */
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

/**
 * Move camera in a plane by input amount.
 * 
 * @param x X change in screen coordinates.
 * @param y Y change in screen coordinates.
 */
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

/**
 * Set perspective projection.
 * 
 * @param fovy Horizontal field of view angle.
 * @param aspect Placeholder text.
 * @param zNear Position of near plane in world coordinates.
 * @param zFar Position of far plane in world coordinates.
 */
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

/**
 * Set camera to defaults.
 */
void Camera::Reset(void)
{
	mode	= CameraMode_Default;

	pos		= Vector<Real>(3, "0.0 0.0 45.0");
	lookAt	= Vector<Real>(3, "0.0 0.0 0.0");
	up		= Vector<Real>(3, "0.0 1.0 0.0");
}

/**
 * Set camera mode.
 * 
 * @param mode New camera mode.
 */
void Camera::SetMode(CameraMode mode)
{
	this->mode = mode;
}

/**
 * Specify camera position and orientation.
 * 
 * @param pos New position vector.
 * @param lookAt New look direction.
 * @param up New up vector.
 */
void Camera::Specify(Vector<Real> pos, Vector<Real> lookAt, Vector<Real> up)
{
	this->pos	 = pos;
	this->lookAt = lookAt;
	this->up	 = up;
}

/**
 * Moves the camera in a spherical arc around its view center.
 * 
 * @param x X change in screen coordinates.
 * @param y Y change in screen coordinates.
 */
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

/**
 * Get the model view matrix for camera.
 */
float *Camera::ViewingTransformation(void)
{
	Vector<Real> u(3,0.0), v(3,0.0), n(3,0.0);

	// if the camera is attached with the Haptic, do special viewing transformation with Haptic configuration
	if((attached!=NULL) && attached->IsEnabled() && (getType()==1)) 
	{		
		Vector<Real>		HI_Position(3, 0.0);
		Matrix<Real>		HI_Orientation(3, 3, 0.0);
		unsigned int		HI_ButtonState;
		Matrix<Real>		g_lh_h(4, 4, 0.0);
		Vector<Real>		scale(3, 1.0);
		Matrix<Real>		g_w_v(4, 4, 0.0);

		// read HapticInterface Configuration
		attached->ReadConfiguration(HI_Position, HI_Orientation, HI_ButtonState);		
		// Convert from m to cm
		HI_Position *= 1E2;
		ToTransformationMatrix(g_lh_h, HI_Orientation, HI_Position, scale);		
		g_w_v = g_w_lh * g_lh_h * g_h_v;

		GetRotationMatrix(HI_Orientation, g_w_v);
		GetTranslationVector(HI_Position, g_w_v);
		
		pos = HI_Position;		
		// Obtain u, v & n vectors
		u = HI_Orientation.column(0);
		v = HI_Orientation.column(1);	
		n = HI_Orientation.column(2);	

		// check normalize of u v n and orthogonal
	}
	else // normal viewing transformation
	{
		// Obtain u, v & n vectors
		n = pos - lookAt;
		v = up;	
		u = Vector<Real>(3);
		crossVV(u, n, v);
		u *= -1;		
		
		n.normalize();
		v.normalize();
		u.normalize();
	}

	

	// Define the Model View Matrix - column major order
	VT[0] = u.begin()[0];	VT[4] = u.begin()[1];	VT[8]  = u.begin()[2];	VT[12] = -(u*pos);
	VT[1] = v.begin()[0];	VT[5] = v.begin()[1];	VT[9]  = v.begin()[2];	VT[13] = -(v*pos);
	VT[2] = n.begin()[0];	VT[6] = n.begin()[1];	VT[10] = n.begin()[2];	VT[14] = -(n*pos);
	VT[3] = 0;				VT[7] = 0;				VT[11] = 0;				VT[15] = 1;	

	return VT;
}

/**
 * Zoom the camera.
 * 
 * @param y Y change in screen coordinates.
 */
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

/**
 * Add a new light to the camera's list of visible lights.
 */
void Camera::AddLight(Light * newLight)
{
	Light ** light2 = new Light*[nLights + 1];
	if (nLights > 0)
	{
		for (int i = 0; i < nLights; i++)
		{
			light2[i] = light[i];
		}
		delete light;
	}
	light2[nLights] = newLight;
	light = light2;
	nLights++;
}

int Camera::getType(void)
{	
	return type;
}

unsigned int Camera::getAttachedHapticInterfaceID(void)
{
	return attachedID;
}

void Camera::AttachHapticInterface(HapticInterface *HI)
{
	attached = HI;
}


	
		/*
		deltaPos = HI_Position - oldHIpos;		
		deltaPos *= speed;						
		
		lookAt = HI_Orientation * lookAt;
		lookAt = speed * HI_Position;			
		
		Zoom(-deltaPos[2]);		
		oldHIpos = HI_Position;				
		*/
		
		/*
		deltaPos = HI_Position - oldHIpos;
		deltaPos *= speed;						
		Pan(deltaPos[0], deltaPos[1]);
		Zoom(-deltaPos[2]);
		oldHIpos = HI_Position;
		*/
	
		/*
		deltaPos = HI_Position - oldHIpos;
		deltaPos *= speed;

		n = pos - lookAt;
		v = up;				
		u = Vector<Real>(3);
		crossVV(u, n, v);
		u *= -1;		
		
		n.normalize();
		u.normalize();
		v.normalize();

		u *= deltaPos[0];
		v *= deltaPos[1];
		n *= -deltaPos[2];		

		lookAt	= lookAt + u + v;				
		pos		= pos - n;	

		oldHIpos = HI_Position;
		*/


