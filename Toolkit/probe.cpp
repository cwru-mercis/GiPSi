/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Rigid Probe HIO Implementation (probe.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong.
*/

////	instrument.cpp v1.0
////
////	Implements the rigid probe Haptic Interface Object
////
////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <iostream>

using namespace std;

#include "algebra.h"
#include "errors.h"
#include "GiPSiException.h"
#include "logger.h"
#include "XMLNodeList.h"
#include "probe.h"


RigidProbeHIO::RigidProbeHIO(XMLNode * simObjectNode) :
	HapticInterfaceObject(simObjectNode)
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

		// Initialize visualization
		InitializeVisualization(simObjectChildren);

		// Load geometry
		LoadGeometry(simObjectChildren);

		// Initialize transformation
		InitializeTransformation(simObjectChildren);

		// Final setup
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


//////////////////////////////////////
//
//	RigidProbeHIO::InitializeVisualization()
//
//		Initialize visualization parameters
//
void RigidProbeHIO::InitializeVisualization(XMLNodeList * simObjectChildren)
{
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

		// Extract green
		XMLNode * greenNode = baseColorChildren->GetNode("green");
		const char * green = greenNode->GetValue();

		// Extract blue
		XMLNode * blueNode = baseColorChildren->GetNode("blue");
		const char * blue = blueNode->GetValue();

		// Extract opacity
		XMLNode * opacityNode = baseColorChildren->GetNode("opacity");
		const char * opacity = opacityNode->GetValue();

		// Initialize Geometry using color information
		initialcolor[0] = (float)atof(red);
		initialcolor[1] = (float)atof(green);
		initialcolor[2] = (float)atof(blue);
		initialcolor[3] = (float)atof(opacity);
		delete red;
		delete green;
		delete blue;
		delete opacity;
	}
	catch (...)
	{
		throw;
		return;
	}
}


////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::LoadGeometry()
//
//		Initialize visualization parameters
//
void RigidProbeHIO::LoadGeometry(XMLNodeList * simObjectChildren)
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


////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::InitializeTransformation()
//
//		Initialize transformation parameters
//
void RigidProbeHIO::InitializeTransformation(XMLNodeList * simObjectChildren)
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
		// Set scale
		s_h_e[0] = (float)atof(sx);
		s_h_e[1] = (float)atof(sy);
		s_h_e[2] = (float)atof(sz);

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
		ToRotationMatrix(R_h_e, TORAD(atof(rangle)), atof(rx), atof(ry), atof(rz));
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
		t_h_e[0] = (float)atof(dx);
		t_h_e[1] = (float)atof(dy);
		t_h_e[2] = (float)atof(dz);


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


////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::Load()
//
//		Reads in .obj file
//
void RigidProbeHIO::Load(char *filename)   
{
	/****************************************************/
	/*	Load Obj file containing instrument geometry	*/
	/****************************************************/
	TriSurface *geom;
	LoadData *data;
	data = LoadObj(filename);

	/****************************************/
	/*	Set up initial geometry				*/
	/****************************************/
	if((InitialGeometry = new TriSurface(initialcolor[0],initialcolor[1],initialcolor[2],initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
	}	
	InitialGeometry->Load(data);
	InitialGeometry->calcNormals();

	/****************************************/
	/*	Set up geometry						*/
	/****************************************/
	if((geometry = new TriSurface(initialcolor[0],initialcolor[1],initialcolor[2],initialcolor[3])) == NULL) {
		printf("ERROR: Could not allocate geometry!\n");
		exit(0);
	}
	geom = (TriSurface *) geometry;	
	geom->Load(data);	
	geom->calcNormals();


	/****************************************/
	/*  Deallocate data buffer				*/
	/****************************************/
	delete data;

	// LoadBoundary
	char filename2[50]="";
	char filename3[50]="";
	char *pdest;
	int result;

	// Search forward. 
	pdest = strstr( filename, ".obj" );
	result = (int)(pdest - filename + 1);
	for(int i=0;i<result-1;i++)
	{
		filename2[i] = filename[i];	
		filename3[i] = filename[i];	
	}
	strcat(filename2,"_boundary.obj");
	LoadBoundary(filename2);
	strcat(filename3,"_boundary.map");
	LoadBoundaryMap(filename3);	

	// Allocate domain
	if((domain = new RigidProbeHIODomain()) == NULL) {
		printf("ERROR: Could not allocate domain!\n");
		exit(0);
	}
	
	Init();
}


////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::LoadBoundary()
//
//		Reads in .obj file for boundary
//
void RigidProbeHIO::LoadBoundary(char *filename)   
{
	/****************************************************/
	/*	Load Obj file containing instrument geometry	*/
	/****************************************************/
	LoadData *data;
	data = LoadObj(filename);

	// initialize
	boundary = (RigidProbeHIOBoundary*)new RigidProbeHIOBoundary();

	boundary->Load(data);
	boundary->calcNormals();

	delete data;
}

////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::LoadBoundaryMap()
//
//		Reads in .map file for mapping from boundary to geometry
//
void RigidProbeHIO::LoadBoundaryMap(char *filename)
{
	FILE			*fp;
	unsigned int	i;
	char			errmsg[1024], 
					buffer[1024],
					file_type[64];
	unsigned int	offset_obj1, offset_obj2;
	unsigned int	index_obj1, index_obj2;
  
	fp = fopen(filename, "r");
	if (fp == NULL) {
		sprintf(errmsg, "Cannot open map file %s\n", filename);
		error_exit(-1, errmsg);
	}

	fscanf(fp, "%s%d%d%d%\n", file_type, &(num_mapping), &(offset_obj1), &(offset_obj2));
	printf("File type : <%s>\tMapping: %d\tOBJ1 Offset: %d\tOBJ2 Offset: %d\n",file_type, num_mapping, offset_obj1, offset_obj2);

	if(strcmp("OBJTOOBJ", file_type)) {
		sprintf(errmsg, "Invalid map file type %s\n", filename);
		error_exit(-1, errmsg);
	}

	// Allocate mapping array
	mapping = (unsigned int *) malloc(2*num_mapping*sizeof(int));		
	if(mapping == NULL) {
		error_exit(-1, "Cannot allocate memory for mapping!\n");
	}
		
	/****************************************/
	/*	Read mapping						*/
	/****************************************/
	i = 0;
	while (i < num_mapping) {
		fgets(buffer, 512,fp);
		sscanf(buffer,"%d%d\n", &index_obj1, &index_obj2);
		
		index_obj1 -= offset_obj1;
		index_obj2 -= offset_obj2;

		index_obj1--;
		index_obj2--;

		*(mapping+2*i)= index_obj1;
		*(mapping+2*i+1)= index_obj2;
		
		i++;
	}
	fclose(fp);
}


////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::Init()
//
//		Initialize HIO
//
void RigidProbeHIO::Init(void)
{

	/****************************************/
	/*	Set up states						*/
	/****************************************/
	// state not defined

	/****************************************/
	/*	Set up boundary						*/
	/****************************************/
	// boundary and the object geometry are the same
	State2Bound();
	RigidProbeHIOBoundary *bound = (RigidProbeHIOBoundary *) boundary;	
	bound->Init(this);
	bound->SetNullModel();	
	
	
	/****************************************/
	/*	Set up domain						*/
	/****************************************/
	RigidProbeHIODomain *dom = (RigidProbeHIODomain *) domain;
	dom->Init(this);

	/****************************************/
	/*	Set up integrator					*/
	/****************************************/
	// we don't need an integrator	
}

////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::Simulate()
//
//		Simulates the RigidProbeHIO object by a single timestep
//
//
void RigidProbeHIO::Simulate(void)
{
	//cout << "in RP:Simulate " << attach_flag << endl;
	if (attach_flag) 
	{
		HI->ReadConfiguration(HI_Position, HI_Orientation, HI_ButtonState);		
		
		// Convert from m to cm
		HI_Position *= 1E2;
		
		// Update haptic model
		RigidProbeHIOBoundary *bound = (RigidProbeHIOBoundary *) boundary;
		HI->UseHapticModel( bound->ReturnHapticModel());		
	}
	else
		error_display(-1,"HapticInterface has not been attached.\n");
	
	// We need to update Geometry and Boundary from the states
	//   so that we will have the correct geometry to dispalu and correct boundary definition 
	//   for boundary calculations
	//State2Geom();
	State2Bound();	
}



////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::Geom2State()
//
//		Copies geometry to state
//
//
inline void RigidProbeHIO::Geom2State(void)
{
	// We don't need to do anything here	
}

////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::State2Geom()
//
//		Copies state to geometry
//
//
inline void RigidProbeHIO::State2Geom(void)
{
	TriSurface *geom = (TriSurface *) geometry;	
	Matrix<Real> g_w_e = GetConfiguration();
	
	// reset to initial setup (at origin with initial rotation)
	for (unsigned int i=0; i<geom->num_vertex; i++) {
		geom->vertex[i].pos = InitialGeometry->vertex[i].pos;
	}	

	// rotation
	Matrix<Real> rotation = zero_matrix3;
	GetRotationMatrix(rotation, g_w_e);
	geom->Rotate(rotation);		
	// translate 
	Vector<Real> translate = zero_vector3;
	GetTranslationVector(translate, g_w_e);
	geom->Translate(translate);
	
	geom->calcNormals();	
}

////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::State2Bound()
//
//		Copies state to boundary
//
//
inline void RigidProbeHIO::State2Bound(void)
{
	// We call Geom2Bound() as the Geometry should have already been updated
	Geom2Bound();
}


////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::Geom2Bound()
//		Copies geometry to boundary
//
inline void RigidProbeHIO::Geom2Bound(void)
{
	TriSurface *geom = (TriSurface *) geometry;
	Boundary *bound = (RigidProbeHIOBoundary *) boundary;
	unsigned int	index_obj1;
	unsigned int	index_obj2;
	
	for(unsigned int i=0;i<num_mapping;i++) 
	{			
		index_obj1 = *(mapping+2*i);
		index_obj2 = *(mapping+2*i+1);
		bound->vertex[index_obj1].pos = geom->vertex[index_obj2].pos;				
	}
}


////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::Bound2Geom()
//		Copies boundary to geometry
//
inline void RigidProbeHIO::Bound2Geom(void)
{
	TriSurface *geom = (TriSurface *) geometry;
	const Boundary *bound = (RigidProbeHIOBoundary *) boundary;
	geom->Translate(bound->vertex[*(mapping)].pos[0] - geom->vertex[*(mapping+1)].pos[0],
					bound->vertex[*(mapping)].pos[1] - geom->vertex[*(mapping+1)].pos[1],
					bound->vertex[*(mapping)].pos[2] - geom->vertex[*(mapping+1)].pos[2]);		            
}


////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::Display()
//
//		Displays the RigidProbeHIO mesh
//
//
void RigidProbeHIO::Display(void)
{
	// Copy the state into the geometry
	State2Geom();

	// Call the display manager
	displayMngr->Display();
}

////////////////////////////////////////////////////////////////
//
//	RigidProbeHIO::SetupDisplay()
//
//		Sets up the display of the the RigidProbeHIO
//
//
void RigidProbeHIO::SetupDisplay(XMLNodeList * simObjectChildren) 
{
	DisplayHeader	header;

	header.objType				= GIPSI_DRAW_TRIANGLE;
	header.polyMode				= GIPSI_POLYGON_FILL;
	header.shadeMode			= GIPSI_SHADE_SMOOTH;
	header.dataType				= 0x0E;					// TEXTURE ON, NORMAL ON, RGBA
//	header.dataType				= 0x06;

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