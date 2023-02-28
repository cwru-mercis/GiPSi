/*The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Haptics API Implementation (GiPSiHaptics.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu, Suriya Natsupakpong. 
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong.
*/

////	GISPIHAPTICS.CPP v0.0
////
////	Implementation for GiPSi Haptics API 
////
////////////////////////////////////////////////////////////////

#include "GiPSiHaptics.h"

HapticInterfaceObject::HapticInterfaceObject(XMLNode * simObjNode): RigidSolidObject(simObjNode)
{
	try
	{
		R_w_lh = zero_matrix3;
		t_w_lh = zero_vector3;
		s_w_lh = zero_vector3;
		R_h_e = zero_matrix3;
		t_h_e = zero_vector3;
		s_h_e = zero_vector3;
		XMLNode * idNode = simObjNode->GetChildren()->GetNode("objParameters")->GetChildren()->GetNode("NHParameters")->GetChildren()->GetNode("modelParameters")->GetChildren()->GetNode("HIOParameters")->GetChildren()->GetNode("ID");
		char * idVal = idNode->GetValue();
		Identifier = atoi(idVal);
		delete idVal;

		// Extract model parameter
		XMLNode * modelParametersNode = simObjNode->GetChildren()->GetNode("objParameters")->GetChildren()->GetNode("NHParameters")->GetChildren()->GetNode("modelParameters");
		XMLNodeList * modelParametersChildren = modelParametersNode->GetChildren();
		LoadParameter(modelParametersChildren);

		attach_flag = false;
		HI = NULL;

		// initialize HIO configuration
		Vector<Real>	p(3,0.0);
		Matrix<Real>	R(3,3,0.0);
		R[0][0] = R[1][1] = R[2][2] = 1.0;
		HI_Position = p;
		HI_Orientation = R;
		HI_ButtonState = 0x0000;
	}
	catch (...)
	{
		throw;
		return;
	}
}

////////////////////////////////////////////////////////////////
//
//	HapticInterfaceObject::LoadParameter()
//	Load model parameters for 
//		Get haptic interface identifier
//		Initialize transformation parameters
//
void HapticInterfaceObject::LoadParameter(XMLNodeList * simObjectChildren)
{
	try
	{
		XMLNode * idNode = simObjectChildren->GetNode("HIOParameters")->GetChildren()->GetNode("ID");		
		char * idVal = idNode->GetValue();
		Identifier = atoi(idVal);
		delete idVal;


		Geometry * geom = GetGeometryPtr();
		XMLNode * transformationNode = simObjectChildren->GetNode("HIOParameters")->GetChildren()->GetNode("transformation");
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
		// Set scale
		s_w_lh[0] = (float)atof(sx);
		s_w_lh[1] = (float)atof(sy);
		s_w_lh[2] = (float)atof(sz);
		
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
		ToRotationMatrix(R_w_lh, TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));
		
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
		t_w_lh[0] = (float)atof(dx);
		t_w_lh[1] = (float)atof(dy);
		t_w_lh[2] = (float)atof(dz);

		delete dx;
		delete dy;
		delete dz;
	}
	catch (...)
	{
		throw;
		return;
	}
}

int	HapticInterfaceObject::Attach(HapticInterface *hinterface) {
	if ( (attach_flag==false) && (hinterface->ReturnHapticInterfaceIdentifier() == Identifier)) {		
		HI=hinterface;
		attach_flag = true;
		return 1;
	}
	else {
		return 0;
	}
}

////////////////////////////////////////////////////////////////
//
//	HapticInterfaceObject::GetConfiguration()
//
//		Get the configuration of HapticInterfaceObject
//
//
Matrix<Real> HapticInterfaceObject::GetConfiguration(void)
{
	Matrix<Real> mat4 = zero_matrix4;
	Vector<Real> scale(3,1.0);
	Matrix<Real> g_w_lh = zero_matrix4;
	Matrix<Real> g_lh_h = zero_matrix4;
	Matrix<Real> g_h_e = zero_matrix4;

	ToTransformationMatrix(g_w_lh, R_w_lh, t_w_lh, s_w_lh);
	ToTransformationMatrix(g_lh_h, HI_Orientation, HI_Position, scale);
	ToTransformationMatrix(g_h_e, R_h_e, t_h_e, s_h_e);

	mat4 = g_w_lh * g_lh_h * g_h_e;
	/*
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			printf("%f ",mat4[i][j]);
		}
		printf("\n");
	}
	*/
	return mat4;
}

////////////////////////////////////////////////////////////////
//
//	HapticInterfaceObject::SetBaseConfiguration()
//
//		Set base the configuration of HapticInterfaceObject
//
//
void HapticInterfaceObject::SetBaseConfiguration(const Matrix<Real> &config)
{
	GetRotationMatrix(R_w_lh, config);
	GetTranslationVector(t_w_lh, config);
}