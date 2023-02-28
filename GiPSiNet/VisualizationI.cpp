/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is VisualizationI Implementation (VisualizationI.cpp).

The Initial Developers of the Original Code are Qingbo Cai and Suriya Natsupakpong.  
Portions created by Qingbo Cai and Suriya Natsupakpong are Copyright (C) 2007-2008.
All Rights Reserved.

Contributor(s): Qingbo Cai, Suriya Natsupakpong.
*/

////	VISUALIZATIONI.CPP v0.0
////
////	VisualizationI Implementation
////
////////////////////////////////////////////////////////////////

#ifdef GIPSINET

#include "VisualizationI.h"

#include "GiPSiDisplay.h"
#include "GiPSiVisualizationEngine.h"
#include "GiPSiException.h"
#include "ToolkitShaderParamLoader.h"


// Implementation skeleton constructor
CORBASKP_i::CORBASKP_i (SimulationKernelProxy * s)
{
	skptr = s;
}

// Implementation skeleton destructor
CORBASKP_i::~CORBASKP_i (void)
{
}

::CORBA::Short CORBASKP_i::setArray (
    const char * displayBufferName,
    const ::CORBADisplayArray & displayArray
  )
  ACE_THROW_SPEC ((
    ::CORBA::SystemException
  ))
{	
	//cout << "CORBASKP_i::setArray()..." << endl;
	// Add your implementation here
	DisplayArray *dArray = new DisplayArray;

	//interpret objType
	switch (displayArray.header.objType){
		case CORBAGIPSI_DRAW_POINT:
			(dArray->header).objType = GIPSI_DRAW_POINT;
			break;
		case CORBAGIPSI_DRAW_LINE:
			(dArray->header).objType = GIPSI_DRAW_LINE;
			break;
		case CORBAGIPSI_DRAW_TRIANGLE:
			(dArray->header).objType = GIPSI_DRAW_TRIANGLE;
			break;
		case CORBAGIPSI_DRAW_QUAD:
			(dArray->header).objType = GIPSI_DRAW_QUAD;
			break;
		case CORBAGIPSI_DRAW_POLYGON:
			(dArray->header).objType = GIPSI_DRAW_POLYGON;
			break;
		case CORBAGIPSI_DRAW_CUSTOM:
			(dArray->header).objType = GIPSI_DRAW_CUSTOM;
			break;
		default:
			break;
	}

	//interpret polyMode
	switch (displayArray.header.polyMode){
		case CORBAGIPSI_POLYGON_OUTLINE:
			(dArray->header).polyMode = GIPSI_POLYGON_OUTLINE;
			break;
		case CORBAGIPSI_POLYGON_FILL:
			(dArray->header).polyMode = GIPSI_POLYGON_FILL;
			break;
		default:
			break;
	}

	//interpret shadeMode
	switch (displayArray.header.shadeMode){
		case CORBAGIPSI_SHADE_FLAT:
			(dArray->header).shadeMode = GIPSI_SHADE_FLAT;
			break;
		case CORBAGIPSI_SHADE_SMOOTH:
			(dArray->header).shadeMode = GIPSI_SHADE_SMOOTH;
			break;
		default:
			break;
	}
	
	//interpret the rest of header
	(dArray->header).dataType = (displayArray.header).dataType;
	(dArray->header).pointSize = (displayArray.header).pointSize;
	(dArray->header).lineSize = (displayArray.header).lineSize;

	dArray->DISPARRAY_NODESIZE = displayArray.DISPARRAY_NODESIZE;
	dArray->dA_size = displayArray.dA_size;
	dArray->iA_size = displayArray.iA_size;
	dArray->full = displayArray.full;

	dArray->dispArray = new float [dArray->dA_size];
	for (long i = 0; i < dArray->dA_size; i++)
		(dArray->dispArray)[i] = (displayArray.dispArray)[i];

	dArray->indexArray = new unsigned int [dArray->iA_size];
	for (long i = 0; i < dArray->iA_size; i++)
		(dArray->indexArray)[i] = (displayArray.indexArray)[i];

	skptr->setArray(const_cast< char* > (displayBufferName), dArray);

	delete dArray->indexArray;
	delete dArray->dispArray;
	delete dArray;
	return 0;
};


void CORBASKP_i :: setSKP (SimulationKernelProxy * ptr)
{
	skptr = ptr;
};

SimulationKernelProxy	::	SimulationKernelProxy	(XMLNode * rootNode, 
													 ObjectLoader * objLoader,
													 ConnectorLoader * connectorLoader,
													 CollisionDARLoader * CDARLoader,
													 Collision * newCollision)
//									: SimulationKernel (rootNode, objLoader, connectorLoader, CDARLoader, newCollision)
: SimulationKernel ()
{
	num_Buffers = CountBuffers(rootNode);
	//cout << "num_Buffers:" << num_Buffers <<endl;
	buffers = new DisplayBuffer*[num_Buffers];
	XMLNodeList * rootChildren = rootNode->GetChildren();
	int nBuffers = 0;
	
	// Get simulation objects
	XMLNode * simulationObjectsNode = rootChildren->GetNode("simulationObjects");
	XMLNodeList * simulationObjectsChildren = simulationObjectsNode->GetChildren();

	//For each <simObject> node:
	for (int i = 0; i < simulationObjectsChildren->GetLength(); i++)
	{
		XMLNode * simObjectNode = simulationObjectsChildren->GetNode(i);
		XMLNodeList * simObjectChildren = simObjectNode->GetChildren();

		//For <visualization> node:
		XMLNode * visualizationNode = simObjectChildren->GetNode("visualization");
		XMLNodeList * visualizationChildren = visualizationNode->GetChildren();

		//For <shader> node:
		XMLNode * shaderNode = visualizationChildren->GetNode("shader");
		ToolkitShaderParamLoader * shaderParamLoader = new ToolkitShaderParamLoader();
		ShaderParams * shaderParams = shaderParamLoader->LoadShaderParams(shaderNode);		
		
		delete shaderNode;

		delete visualizationChildren;
		delete visualizationNode;

		//For <geometry> node:
		XMLNode * geometriesNode = simObjectChildren->GetNode("geometries");
		XMLNodeList * geometriesChildren = geometriesNode->GetChildren();		

		//For each <geometry> node:
		for (int j = 0; j < geometriesChildren->GetLength(); j++)
		{
			DisplayBuffer * newDisplayBuffer = new DisplayBuffer(NULL, NULL, GIPSI_NO_TEXTURE);
			buffers[nBuffers] = newDisplayBuffer;
			/*
			char name[10];
			buffers[nBuffers]->SetName(itoa(nBuffers, name, 16));
			*/
			
			XMLNode * nameNode = simObjectChildren->GetNode("name");			
			char * objectName = nameNode->GetValue();
			delete nameNode;	
			buffers[nBuffers]->SetObjectName(objectName);
			buffers[nBuffers]->SetName(objectName);
			delete objectName;

			buffers[nBuffers]->SetShaderParams(shaderParams);

			if (nBuffers > 0)
			{	int nb = nBuffers - 1;
				buffers[nb]->SetNext(newDisplayBuffer);
			}
			nBuffers++;

			XMLNode * geometryNode = geometriesChildren->GetNode("geometry");
			XMLNodeList * geometryChildren = geometryNode->GetChildren();
			XMLNode * textureNamesNode = geometryChildren->GetNode("textureNames");
			
			if (textureNamesNode->GetNumChildren() > 0)
			{
				XMLNodeList * textureNamesChildren = textureNamesNode->GetChildren();

				//For each <textureName> node:
				for (int k = 0; k < textureNamesChildren->GetLength(); k++)
				{
					XMLNode * textureNameNode = textureNamesChildren->GetNode(k);
					char * textureName = textureNameNode->GetValue();
					newDisplayBuffer->AddTexture(textureName);					
					delete textureName;
					delete textureNameNode;
				}

				delete textureNamesChildren;
			}
			delete textureNamesNode;
			delete geometryChildren;
			delete geometryNode;			
		}
	
		delete geometriesChildren;
		delete geometriesNode;

		delete simObjectChildren;
		delete simObjectNode;
	}

	XMLNode * visualizationNode = rootChildren->GetNode("visualization");
	XMLNodeList * visualizationChildren = visualizationNode->GetChildren();
	XMLNode * texturesNode = visualizationChildren->GetNode("textures");
	if (texturesNode->GetNumChildren() > 0)
	{
		XMLNodeList * texturesChildren = texturesNode->GetChildren();
		int num_texture = texturesChildren->GetLength();

		for (int i = 0; i<num_texture; i++)
		{	
			XMLNode * textureNode = texturesChildren->GetNode(i);
			XMLNodeList * textureChildren = textureNode->GetChildren();
			XMLNode * typeNode = textureChildren->GetNode("type");
			if (!typeNode)
			{
				throw new GiPSiException("SimulationKernelProxy constructor", "type node not found.");
				return;
			}

			char * typeVal = typeNode->GetValue();
			DisplayTextureType textureType;
			if (strcmp(typeVal, "GIPSI_NO_TEXTURE") == 0)
				textureType = GIPSI_NO_TEXTURE;
			else if (strcmp(typeVal, "GIPSI_2D_STATIC_CLIENT") == 0)	
				textureType = GIPSI_2D_STATIC_CLIENT;
			else if (strcmp(typeVal, "GIPSI_2D_STATIC_SERVER") == 0)	
				textureType = GIPSI_2D_STATIC_SERVER;
			else if (strcmp(typeVal, "GIPSI_2D_DYNAMIC_SERVER") == 0)	
				textureType = GIPSI_2D_DYNAMIC_SERVER;
			else if (strcmp(typeVal, "GIPSI_3D_STATIC_CLIENT") == 0)	
				textureType = GIPSI_3D_STATIC_CLIENT;
			else
			{
				throw new GiPSiException("SimulationKernelProxy constructor", "Unrecognized texture type.");
				return;
			}

			DisplayBuffer * newDisplayBuffer = new DisplayBuffer(NULL, NULL, textureType);
			buffers[nBuffers] = newDisplayBuffer;
			/*
			char name[3];
			buffers[nBuffers]->SetName(itoa(nBuffers, name, 10));		
			*/
			XMLNode * nameNode = textureChildren->GetNode("name");			
			char * textureName = nameNode->GetValue();
			delete nameNode;	
			buffers[nBuffers]->SetObjectName(textureName);
			buffers[nBuffers]->SetName(textureName);
			delete textureName;

			if (nBuffers > 0)
			{	int nb = nBuffers -1;
				buffers[nb]->SetNext(newDisplayBuffer);
			}
			nBuffers++;

			delete typeVal;
			delete typeNode;
			delete textureChildren;
			delete textureNode;
		}
		delete texturesChildren;
	}
	delete texturesNode;

	delete visualizationChildren;
	delete visualizationNode;

	//cout << "nBuffers:" << nBuffers <<endl;

	return;
}


void	SimulationKernelProxy :: SimulationThread ()
{		
	return;
}


int		SimulationKernelProxy :: setArray(char * name, DisplayArray * inArray)
{
	//cout << "SimulationKernelProxy :: setArray() of " << name << endl;
	DisplayBuffer * buffer = GetDisplayBufferHead();

	/*int i = 0;
	while (buffer->GetNext() != NULL)
	{	
		cout << "buffer name: " << buffer->GetName() << endl;
		buffer = buffer->GetNext();
		i++;
	}
	cout << "buffer name: " << buffer->GetName() << endl;
	cout << "i: " << i << endl;

	buffer = GetDisplayBufferHead();*/

	while ( (buffer != NULL) && (strcmp(buffer->GetName(),name)!=0) )
		buffer = buffer->GetNext();				

	//If no display buffer with the name, then return.
	if (buffer == NULL) 
	{
		cout << "No display buffer with the name: " << name << endl;
		return -1;	
	}
		
	DisplayArray * array = buffer->GetWriteArray(inArray->DISPARRAY_NODESIZE, inArray->dA_size, inArray->iA_size, true);

	//Copy contents of inArray to array
	array->header = inArray->header;
	array->DISPARRAY_NODESIZE = inArray->DISPARRAY_NODESIZE;
	array->dA_size = inArray->dA_size;
	array->iA_size = inArray->iA_size;
	for (int i = 0; i < array->dA_size; i++)
		array->dispArray[i] = inArray->dispArray[i];
	for (int i = 0; i < array->iA_size; i++)
		array->indexArray[i] = inArray->indexArray[i];
	array->full = inArray->full;

	/*cout << "array->DISPARRAY_NODESIZE: " << array->DISPARRAY_NODESIZE << endl;
	cout << "array->dA_size: " << array->dA_size << endl;
	for (int i = 0; i < array->dA_size; i++)
		cout << "array->dispArray[" << i << "]: " << array->dispArray[i] << endl;
	cout << "array->iA_size: " << array->iA_size << endl;
	for (int i = 0; i < array->iA_size; i++)
		cout << "array->indexArray[" << i << "]: " << array->indexArray[i] << endl;
	cout << "array->full: " << array->full << endl;*/

	buffer->Enqueue();

	return 0;
}


int SimulationKernelProxy	::	CountBuffers	(XMLNode * rootNode)
{
	int temp = 0;
	XMLNodeList * rootChildren = rootNode->GetChildren();
		
	// Get simulation objects
	XMLNode * simulationObjectsNode = rootChildren->GetNode("simulationObjects");
	XMLNodeList * simulationObjectsChildren = simulationObjectsNode->GetChildren();

	//Count the number of geometries
	for (int i = 0; i < simulationObjectsChildren->GetLength(); i++)
	{
		XMLNode * simObjectNode = simulationObjectsChildren->GetNode(i);
		XMLNodeList * simObjectChildren = simObjectNode->GetChildren();

		XMLNode * geometriesNode = simObjectChildren->GetNode("geometries");
		XMLNodeList * geometriesChildren = geometriesNode->GetChildren();

		temp = temp + geometriesChildren->GetLength();

		delete geometriesChildren;
		delete geometriesNode;
		delete simObjectChildren;
		delete simObjectNode;
	}
	delete simulationObjectsChildren;
	delete simulationObjectsNode;

	//count the number of textures
	XMLNode * visualizationNode = rootChildren->GetNode("visualization");
	XMLNodeList * visualizationChildren = visualizationNode->GetChildren();
	XMLNode * texturesNode = visualizationChildren->GetNode("textures");
	if (texturesNode->GetNumChildren() > 0)
		temp = temp + texturesNode->GetNumChildren();
	delete texturesNode;
	delete visualizationChildren;
	delete visualizationNode;

	return temp;
}

bool SimulationKernelProxy :: setUICommand(const char * command)
{
	cout << "SimulationKernelProxy :: setUICommand() with " << command << endl;

	skvar->setUICommand(CORBA::string_dup(command));
	return true;
}

void VisualizationEngineProxy :: Start (void)
{
	// run the VEP loop until STOP = true.
	while (!STOP)
	{	
		//cout << "nBuffers:" << nBuffers <<endl;
		for (int i=0; i<nBuffers; i++)
		{	
			if (STOP) return;
			allBuffers[i]->ConditionalDequeue();
			DisplayArray * array = allBuffers[i]->GetReadArray();
			setArray(allBuffers[i]->GetName(), array);
		}		
	}
	return;
}

void VisualizationEngineProxy :: Stop (void)
{
	STOP = true;
}

int VisualizationEngineProxy :: setArray (const char * name, DisplayArray * displayArray)
{
	
	//cout << "VisualizationEngineProxy :: setArray() of " << name << endl;
	CORBADisplayArray dArray;

	//interpret objType
	switch ((displayArray->header).objType){
		case GIPSI_DRAW_POINT:
			(dArray.header).objType = CORBAGIPSI_DRAW_POINT;
			break;
		case GIPSI_DRAW_LINE:
			(dArray.header).objType = CORBAGIPSI_DRAW_LINE;
			break;
		case GIPSI_DRAW_TRIANGLE:
			(dArray.header).objType = CORBAGIPSI_DRAW_TRIANGLE;
			break;
		case GIPSI_DRAW_QUAD:
			(dArray.header).objType = CORBAGIPSI_DRAW_QUAD;
			break;
		case GIPSI_DRAW_POLYGON:
			(dArray.header).objType = CORBAGIPSI_DRAW_POLYGON;
			break;
		case GIPSI_DRAW_CUSTOM:
			(dArray.header).objType = CORBAGIPSI_DRAW_CUSTOM;
			break;
		default:
			break;
	}

	//interpret polyMode
	switch ((displayArray->header).polyMode){
		case GIPSI_POLYGON_OUTLINE:
			(dArray.header).polyMode = CORBAGIPSI_POLYGON_OUTLINE;
			break;
		case GIPSI_POLYGON_FILL:
			(dArray.header).polyMode = CORBAGIPSI_POLYGON_FILL;
			break;
		default:
			break;
	}

	//interpret shadeMode
	switch ((displayArray->header).shadeMode){
		case GIPSI_SHADE_FLAT:
			(dArray.header).shadeMode = CORBAGIPSI_SHADE_FLAT;
			break;
		case GIPSI_SHADE_SMOOTH:
			(dArray.header).shadeMode = CORBAGIPSI_SHADE_SMOOTH;
			break;
		default:
			break;
	}
	
	//interpret the rest of header
	(dArray.header).dataType = (displayArray->header).dataType;
	(dArray.header).pointSize = (displayArray->header).pointSize;
	(dArray.header).lineSize = (displayArray->header).lineSize;

	dArray.DISPARRAY_NODESIZE = displayArray->DISPARRAY_NODESIZE;
	dArray.dA_size = displayArray->dA_size;
	dArray.iA_size = displayArray->iA_size;
	dArray.full = displayArray->full;

	dArray.dispArray.length(dArray.dA_size);
	for (long i = 0; i < dArray.dA_size; i++)
		(dArray.dispArray)[i] = (displayArray->dispArray)[i];

	dArray.indexArray.length(dArray.iA_size); 
	for (long i = 0; i < dArray.iA_size; i++)
		(dArray.indexArray)[i] = (displayArray->indexArray)[i];
	
	return skptr->setArray(name, dArray);	
}

void VisualizationEngineProxy :: setSimKernel(SimulationKernel * newSimKernel)
{
	simKernel = newSimKernel;
}

#endif