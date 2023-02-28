/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Simple Test Object Model Implementation (simple.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	SIMPLE.cpp	v0.0
////
////	Simple test object class.
////
////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "algebra.h"
#include "errors.h"
#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "GiPSiException.h"
#include "logger.h"
#include "simple.h"
#include "XMLNodeList.h"

#define		DELIM 10


/**
 * Constructor.
 * 
 * @param simObjectNode XML project file 'simObject' node.
 */
SimpleTestObject::SimpleTestObject(XMLNode * simObjectNode)
								   :	SIMObject(simObjectNode)
{
	try
	{
		// Extract initialization information
		if (simObjectNode == NULL)
		{
			char location[256]("");
			sprintf_s(location, 256, "%s constructor", GetName());
			throw new GiPSiException(location, "Empty simObject node recieved.");
			return;
		}
		XMLNodeList * simObjectChildren = simObjectNode->GetChildren();

		// Set object parameters
		SetParameters(simObjectChildren);

		// Initialize visualization
		InitializeVisualization(simObjectChildren);

		// Load geometry
		LoadGeometry(simObjectChildren);

		// Initialize transformation
		InitializeTransformation(simObjectChildren);

		// Final setup
		Geom2State();
		State2Bound();
		SetupDisplay(simObjectChildren);
		delete simObjectChildren;
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Set object general parameters using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void SimpleTestObject::SetParameters(XMLNodeList * simObjectChildren)
{
	try
	{
		// Get parameters node
		XMLNode * objParametersNode = simObjectChildren->GetNode("objParameters");
		XMLNodeList * objParametersChildren = objParametersNode->GetChildren();
		XMLNode * NHParametersNode = objParametersChildren->GetNode("NHParameters");
		XMLNodeList * NHParametersChildren = NHParametersNode->GetChildren();

		// Extract type-specific parameters
		// Set type-specific parameters

		delete NHParametersChildren;
		delete NHParametersNode;
		delete objParametersChildren;
		delete objParametersNode;
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Set object visualization parameters using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void SimpleTestObject::InitializeVisualization(XMLNodeList * simObjectChildren)
{
	// This loader contains a lot of checking.  It makes it safer,
	// but substantially increases the length of the method.  Is
	// it worth adding these checks to the other init methods?
	try
	{
		// Extract baseColor
		XMLNode * visualizationNode = simObjectChildren->GetNode("visualization");
		XMLNodeList * visualizationChildren = visualizationNode->GetChildren();

		// Extract baseColor
		XMLNode * baseColorNode = visualizationChildren->GetNode("baseColor");
		XMLNodeList * baseColorChildren = baseColorNode->GetChildren();

		// Extract red
		XMLNode * redNode = baseColorChildren->GetNode("red");
		const char * red = redNode->GetValue();
		initialcolor[0] = (float)atof(red);
		delete red;
		delete redNode;

		// Extract green
		XMLNode * greenNode = baseColorChildren->GetNode("green");
		const char * green = greenNode->GetValue();
		initialcolor[1] = (float)atof(green);
		delete green;
		delete greenNode;

		// Extract blue
		XMLNode * blueNode = baseColorChildren->GetNode("blue");
		const char * blue = blueNode->GetValue();
		initialcolor[2] = (float)atof(blue);
		delete blue;
		delete blueNode;

		// Extract opacity
		XMLNode * opacityNode = baseColorChildren->GetNode("opacity");
		const char * opacity = opacityNode->GetValue();
		initialcolor[3] = (float)atof(opacity);
		delete opacity;
		delete opacityNode;

		delete baseColorChildren;
		delete baseColorNode;
		delete visualizationChildren;
		delete visualizationNode;
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Load object geometry using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void SimpleTestObject::LoadGeometry(XMLNodeList * simObjectChildren)
{
	try
	{
		// Get path and file name information
		XMLNode * geometriesNode = simObjectChildren->GetNode("geometries");
		XMLNodeList * geometriesChildren = geometriesNode->GetChildren();
		XMLNode * geometryNode = geometriesChildren->GetNode("geometry");
		XMLNodeList * geometryChildren = geometryNode->GetChildren();
		XMLNode * geometryFileNode = geometryChildren->GetNode("geometryFile");
		XMLNodeList * geometryFileChildren = geometryFileNode->GetChildren();

		XMLNode * pathNode = geometryFileChildren->GetNode("path");
		const char * path = pathNode->GetValue();
		XMLNode * fileNameNode = geometryFileChildren->GetNode("fileName");
		const char * fileName = fileNameNode->GetValue();
		delete fileNameNode;
		delete pathNode;
		delete geometryFileChildren;
		delete geometryFileNode;
		delete geometryChildren;
		delete geometryNode;
		delete geometriesChildren;
		delete geometriesNode;

		// Format file name
		char * formattedFileName = new char[strlen(path) + strlen(fileName) + 4];
		sprintf_s(formattedFileName, strlen(path) + strlen(fileName) + 4, ".\\%s\\%s", path, fileName);

		// Load the object file
		logger->Message(GetName(), "Loading object file...", 1);
		Load(formattedFileName);
		delete formattedFileName;
		delete path;
		delete fileName;
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Perform object initial transformation using values from the input XML node list.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void SimpleTestObject::InitializeTransformation(XMLNodeList * simObjectChildren)
{
	try
	{
		Geometry * geom = GetGeometryPtr();
		XMLNode * transformationNode = simObjectChildren->GetNode("transformation");
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
		// Scale the object
		geom->Scale((float)atof(sx), (float)atof(sy), (float)atof(sz));
		delete sx;
		delete sy;
		delete sz;
		delete szNode;
		delete syNode;
		delete sxNode;
		delete scalingChildren;
		delete scalingNode;

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
		// Rotate the object
		geom->Rotate(TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));
		delete rangle;
		delete rx;
		delete ry;
		delete rz;
		delete rangleNode;
		delete rzNode;
		delete ryNode;
		delete rxNode;
		delete axisChildren;
		delete axisNode;
		delete axisRotationChildren;
		delete axisRotationNode;
		delete rotationChildren;
		delete rotationNode;

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
		// Translate the object
		geom->Translate((float)atof(dx), (float)atof(dy), (float)atof(dz));
		delete dx;
		delete dy;
		delete dz;
		delete dzNode;
		delete dyNode;
		delete dxNode;
		delete translationChildren;
		delete translationNode;

		delete transformationChildren;
		delete transformationNode;
	}
	catch (...)
	{
		throw;
		return;
	}
}


/**
 * Load object geometry using the input formatted file name.
 * 
 * @param filename Character string containing formatted geometry file name.
 */
void SimpleTestObject::Load(char *filename)
{
	TriSurface	*geom;
  
	/****************************************/
	/*	Set up geometry						*/
	/****************************************/
	if((geometry = new TriSurface((float)initialcolor[0],(float)initialcolor[1],(float)initialcolor[2],(float)initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}
	geom = (TriSurface *) geometry;

	geom->Load(filename);
	// should we replace this with the following?
	// LoadData *data;
	// data=LoadObj(filename);
	// geom->Load(data);

	geom->calcNormals();

	/****************************************/
	/*	Set up states						*/
	/****************************************/
	state.t=0.0;

	/****************************************/
	/*	Set up boundary						*/
	/****************************************/
	//   SimpleTestObject Model does not have a Boundary Implemented
	
	/****************************************/
	/*	Set up integrator					*/
	/****************************************/
	// MENTAL NOTE: move this to somewhere logical asap!
	integrator = new ERKHeun3<SimpleTestObject>(*this);

}




/**
 * Computes y(t+h)	= y(t) + h * f(..)
 * 
 * @param new_state Where y(t+h) is stored.
 * @param state y(t).
 * @param deriv f(..).
 * @param h h.
 */
inline void SimpleTestObject::AccumState(State &new_state, const State &state, const State &deriv, const Real &h)
{
	new_state.t=deriv.t*h+state.t;
}




/**
 * Computes the derivatives of state variables. This is simply the f()
 * evaluation where f() = dy/dt.
 * 
 * @param deriv Where the derived state information is stored.
 * @param state Current state information.
 */
inline void SimpleTestObject::DerivState(State &deriv, State &state)
{
	deriv.t	=	1.0;
}



/**
 * Allocates the memory for the integrator's local state members.
 * 
 * @param s Pointer where memory is allocated.
 */
inline void SimpleTestObject::AllocState(State &s)
{
}



/**
 * Simulates the SimpleTestObject object by a single timestep.
 */
void SimpleTestObject::Simulate(void)
{
	integrator->Integrate(*this, timestep);
	time+=timestep;

	//We alse need to update the Boundary from states 
	//   so that we will have the correct Boundary definition for boundary calculations
	State2Bound();

}



/**
 * Copies geometry to state.
 */
void SimpleTestObject::Geom2State(void)
{
}



/**
 * Copies state to geometry.
 */
inline void SimpleTestObject::State2Geom(void)
{
}

/**
 * Copies state to boundary.
 */
inline void SimpleTestObject::State2Bound(void)
{
	//printf("No SimpleTestObject Boundary implemented...\n");
}


/**
 * Displays the SimpleTestObject mesh.
 */
void SimpleTestObject::Display(void)
{
	TriSurfaceDisplayManager	*tmp = (TriSurfaceDisplayManager	*) displayMngr;

	// Copy the state into the geometry
	State2Geom();

	// As simple object is not deformable, the geometry will not changed.  Therefore,
	//  we can display the simple object only once.
	if(geometry->modified) 
	{
		displayMngr->Display();
		geometry->modified=false;
	}
}

/**
 * Sets up the display of the the SimpleTestObject.
 * 
 * @param simObjectChildren XML project file 'simObject' node children.
 */
void SimpleTestObject::SetupDisplay(XMLNodeList * simObjectChildren) 
{
	DisplayHeader	header;

	header.objType				= GIPSI_DRAW_TRIANGLE;
	header.polyMode				= GIPSI_POLYGON_FILL;
	header.shadeMode			= GIPSI_SHADE_SMOOTH;
	header.dataType				= 0x0E;					// TEXTURE ON, NORMAL ON, RGBA

	displayMngr = (TriSurfaceDisplayManager *) new TriSurfaceDisplayManager((TriSurface *) geometry, &header);
	displayMngr->SetObjectName(GetName());

	// Load texture names for each DisplayManager
	XMLNode * geometriesNode = simObjectChildren->GetNode("geometries");
	XMLNodeList * geometriesChildren = geometriesNode->GetChildren();
	XMLNode * geometryNode = geometriesChildren->GetNode("geometry");
	XMLNodeList * geometryChildren = geometryNode->GetChildren();
	XMLNode * textureNamesNode = geometryChildren->GetNode("textureNames");
	if (textureNamesNode->GetNumChildren() > 0)
	{
		XMLNodeList * textureNamesChildren = textureNamesNode->GetChildren();

		for (int i = 0; i < textureNamesChildren->GetLength(); i++)
		{
			XMLNode * textureNameNode = textureNamesChildren->GetNode(i);
			char * textureName = textureNameNode->GetValue();
			displayMngr->AddTexture(textureName);
			delete textureName;
			delete textureNameNode;
		}
		delete textureNamesChildren;
	}
	delete textureNamesNode;
	delete geometryChildren;
	delete geometryNode;
	delete geometriesChildren;
	delete geometriesNode;
}
