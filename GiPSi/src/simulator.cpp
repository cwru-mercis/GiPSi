/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Main Simulation Kernel Implementation (simulator.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	Simulator.CPP v0.0.9
////
////	Main Simulation Kernel module 
////
////////////////////////////////////////////////////////////////

// Standard includes
#include "algebra.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <crtdbg.h>

#include "GiPSiAPI.h"
#include "GiPSiCompToolset.h"
#include "GiPSiException.h"
#include "GiPSiHaptics.h"
#include "simulator.h"
#include "GiPSiHapticsManager.h"
#include "logger.h"
#include "collision.h"
#include "timing.h"

#include "ConnectorLoader.h"
#include "ObjectLoader.h"
#include "ProjectLoader.h"
#include "ShaderParamLoader.h"

//#include "msd_haptics.h"
//#include "probe.h"
#include "balloon.h"

/**
 * Constructor.
 * 
 * @param rootNode Root node of the project XML file.
 * @param objLoader ObjectLoader to use for loading simulation objects.
 * @param connectorLoader ConnectorLoader to use for loading connector objects.
 * @param CDARLoader CollisionDARLoader to use for loading collision detection parameters.  
 */
SimulationKernel::SimulationKernel(	XMLNode * rootNode,
									ObjectLoader * objLoader,
									ConnectorLoader * connectorLoader,
									CollisionDARLoader * CDARLoader)
									:	num_object(0),
										num_connector(0),
										RUN(false),
										EXIT(false),
										RTIME(false),
										objLoader(objLoader),
										connectorLoader(connectorLoader),
										CDARLoader(CDARLoader),
										boundingVolumes(NULL),
										hboundingVolumes(NULL),
										collision(NULL),
										hapticCollision(NULL),
										name(NULL), 
										time(0.0), 
										simTimeUse(0.0)
{
	try
	{
		XMLNodeList * rootChildren = rootNode->GetChildren();

		// Get general parameters
		XMLNode * generalParametersNode = rootChildren->GetNode("generalParameters");
		XMLNodeList * generalParametersChildren = generalParametersNode->GetChildren();
		// Set general parameters
		SetGeneralProjectParameters(generalParametersChildren);
		delete generalParametersChildren;
		delete generalParametersNode;
		
		// Get simulation objects
		XMLNode * simulationObjectsNode = rootChildren->GetNode("simulationObjects");
		if (simulationObjectsNode->GetNumChildren() > 0)
		{
			XMLNodeList * simulationObjectsChildren = simulationObjectsNode->GetChildren();
			// Create simulation objects
			CreateSimulationObjects(simulationObjectsChildren);
			delete simulationObjectsChildren;
		}
		delete simulationObjectsNode;

		// Get connectors
		XMLNode * connectorsNode = rootChildren->GetNode("connectors");
		if (connectorsNode->GetNumChildren() > 0)
		{
			XMLNodeList * connectorsChildren = connectorsNode->GetChildren();
			// Create connectors
			CreateConnectors(connectorsChildren);
			delete connectorsChildren;
		}
		delete connectorsNode;

		// Get textures
		XMLNode * visualizationNode = rootChildren->GetNode("visualization");
		XMLNodeList * visualizationChildren = visualizationNode->GetChildren();
		XMLNode * texturesNode = visualizationChildren->GetNode("textures");
		if (texturesNode->GetNumChildren() > 0)
		{
			XMLNodeList * texturesChildren = texturesNode->GetChildren();
			CreateTextures(texturesChildren);
			delete texturesChildren;
		}
		delete texturesNode;
		delete visualizationChildren;
		delete visualizationNode;
		
		// Get CollisionDAR parameters
		XMLNode * collisionDARNode = rootChildren->GetNode("collisionDAR");
		// Set CollisionDAR parameters
		collisionDAR = new CollisionDARParameters(collisionDARNode, CDARLoader);
		CreateCollisionDAR();
		delete collisionDARNode;

		// Get simulation order
		XMLNode * simOrderNode = rootChildren->GetNode("simulationorder");		
		// Create simulation order
		CreateSimulationOrder(simOrderNode);		
		delete simOrderNode;

		delete rootChildren;

		strcpy(uiCommand, "");
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
SimulationKernel::~SimulationKernel()
{
	if (objLoader)
	{
		delete objLoader;
		objLoader = NULL;
	}
	if (connectorLoader)
	{
		delete connectorLoader;
		connectorLoader = NULL;
	}
	if (CDARLoader)
	{
		delete CDARLoader;
		CDARLoader = NULL;
	}
	if (name)
	{
		delete name;
		name = NULL;
	}	
	if (collisionDAR)
	{
		delete collisionDAR;
		collisionDAR = NULL;
	}
	if (collision)
	{
		delete collision;
		collision = NULL;
	}
	if (hapticCollision)
	{
		delete hapticCollision;
		hapticCollision = NULL;
	}

	for (int i = 0; i < num_connector; i++)
		delete connector[i];

	for (int i = 0; i < num_object; i++)
		delete object[i];

	if (texture)
	{
		for (int i = 0; i < num_texture; i++)
			delete texture[i];
		delete texture;
		texture = NULL;
	}	
}

/**
 * Initialize the general project parameters.
 * 
 * @param generalParametersChildren Children of the 'generalParamters' XML project node.
 */
void SimulationKernel::SetGeneralProjectParameters(XMLNodeList * generalParametersChildren)
{
	try
	{
		if (generalParametersChildren == NULL)
		{
			throw new GiPSiException("SimulationKernel", "Empty generalParameters node found in project file.");
			return;
		}

// Set logger parameters
		// Set Verbose Mode
		XMLNode * verboseModeNode = generalParametersChildren->GetNode("verboseMode");
		if (verboseModeNode == NULL)
		{
			throw new GiPSiException("SimulationKernel", "generalParameters.verboseMode node not found in project file.");
			return;
		}

		const char * verboseMode = verboseModeNode->GetValue();
		logger->SetVerboseMode(verboseMode);
		delete verboseModeNode;
		delete verboseMode;

		// Set Verbose Level
		XMLNode * verboseLevelNode = generalParametersChildren->GetNode("verboseLevel");
		if (verboseLevelNode == NULL)
		{
			throw new GiPSiException("SimulationKernel", "generalParameters.verboseLevel node not found in project file.");
			return;
		}

		const char * verboseLevel = verboseLevelNode->GetValue();
		logger->SetVerboseLevel(atoi(verboseLevel));
		delete verboseLevelNode;
		delete verboseLevel;

// Set general project parameters
		// Set Computational Hook
		XMLNode * computationalHookNode = generalParametersChildren->GetNode("computationalHook");
		if (computationalHookNode == NULL)
		{
			throw new GiPSiException("SimulationKernel", "generalParameters.computationalHook node not found in project file.");
			return;
		}

		const char * compHook = computationalHookNode->GetValue();
		if		(strcmp(compHook, "true") == 0)	computationalHook = true;
		else if	(strcmp(compHook, "false") == 0)	computationalHook = false;
		else
		{
			throw new GiPSiException("SimulationKernel", "Unrecognized value for computationalHook found in project file.");
			return;
		}
		delete computationalHookNode;
		delete compHook;

		// Set Network Hook
		XMLNode * networkHookNode = generalParametersChildren->GetNode("networkHook");
		if (networkHookNode == NULL)
		{
			throw new GiPSiException("SimulationKernel", "generalParameters.networkHook node not found in project file.");
			return;
		}

		const char * netHook = networkHookNode->GetValue();
		if		(strcmp(netHook, "true") == 0)	networkHook = true;
		else if	(strcmp(netHook, "false") == 0)	networkHook = false;
		else
		{
			throw new GiPSiException("SimulationKernel", "Unrecognized value for networkHook found in project file.");
			return;
		}
		delete networkHookNode;
		delete netHook;

		// Set timestep
		XMLNode * simTimeNode = generalParametersChildren->GetNode("simTime");
		if (simTimeNode == NULL)
		{
			throw new GiPSiException("SimulationKernel", "generalParameters.simTime node not found in project file.");
			return;
		}

		const char * simTimeStr = simTimeNode->GetValue();
		simTime = atof(simTimeStr);
		timestep = simTime;
		delete simTimeNode;
		delete simTimeStr;

		// Set gravity
		XMLNode * gravityNode = generalParametersChildren->GetNode("gravity");
		if (gravityNode == NULL)
		{
			throw new GiPSiException("SimulationKernel", "generalParameters.gravity node not found in project file.");
			return;
		}

		const char * gravityStr = gravityNode->GetValue();
		g = atof(gravityStr);
		delete gravityNode;
		delete gravityStr;
	}
	catch(...)
	{
		throw;
		return;
	}
}

/**
 * Create simulation objects defined in the input project file node list.
 * 
 * @param simulationObjectsChildren Children of the 'simulationObjects' XML project node.
 */
void SimulationKernel::CreateSimulationObjects(XMLNodeList * simulationObjectsChildren)
{
	try
	{
		for (unsigned int i = 0; i < simulationObjectsChildren->GetLength(); i++)
		{
			XMLNode * simObjectNode = simulationObjectsChildren->GetNode(i);
			SIMObject * simObj = objLoader->LoadObject(simObjectNode, g);

			for (int j = 0; j < i; j++)
			{
				if (strcmp(this->object[j]->GetName(), simObj->GetName()) == 0)
				{
					char message[] = "Duplicate simualtion object found: %s.";
					int message_size = strlen(message) + strlen(simObj->GetName()) + 1;
					char * formattedMessage = new char[message_size];
					sprintf_s(formattedMessage, message_size, message, simObj->GetName());
					throw new GiPSiException("SimulationKernel initialization", formattedMessage);
					delete formattedMessage;
					return;
				}
			}
			if (simObj == NULL)
			{
				char message[] = "Could not initialize simulation object #%d";
				char * formattedMessage = new char[strlen(message) + 1];
				sprintf_s(formattedMessage, strlen(message) + 1, message, num_object);
				throw new GiPSiException("ObjectLoader", formattedMessage);
				delete formattedMessage;
				return;
			}
			this->object[this->num_object++] = simObj;
			// Set this object as the next in the list from the previous object.
			if (num_object > 1)
				this->object[num_object - 2]->displayMngr->GetDisplayBuffer()->SetNext(simObj->displayMngr->GetDisplayBuffer());
			delete simObjectNode;
		}
	}
	catch(...)
	{
		throw;
		return;
	}
}


/**
 * Create connectors defined in the input project file node list.
 * 
 * @param connectorsChildren Children of the 'connectors' XML project node.
 */
void SimulationKernel::CreateConnectors(XMLNodeList * connectorsChildren)
{
	try
	{
		for (unsigned int i = 0; i < connectorsChildren->GetLength(); i++)
		{
			XMLNode * connectorNode = connectorsChildren->GetNode(i);
			Connector * connector = connectorLoader->LoadConnector(connectorNode, this->object, this->num_object);
			if (connector == NULL)
			{
				char message[] = "Could not initialize connector #%d";
				char * formattedMessage = new char[strlen(message) + 1];
				sprintf_s(formattedMessage, strlen(message) + 1, message, i);
				throw new GiPSiException("ConnectorLoader", formattedMessage);
				delete formattedMessage;
				return;
			}
			this->connector[this->num_connector++] = connector;
			delete connectorNode;
		}
	}
	catch(...)
	{
		throw;
		return;
	}
}


/**
 * Create textures defined in the input project file node list.
 * 
 * @param texturesChildren Children of the 'textures' XML project node.
 */
void SimulationKernel::CreateTextures(GiPSiXMLWrapper::XMLNodeList *texturesChildren)
{
	num_texture = texturesChildren->GetLength();
	texture = new TextureDisplayManager*[num_texture];

	// load all texture nodes
	for (int i = 0; i < texturesChildren->GetLength(); i++)
	{
		// get the texture name
		XMLNode * textureNode = texturesChildren->GetNode(i);
		XMLNodeList * textureChildren = textureNode->GetChildren();
		XMLNode * nameNode = textureChildren->GetNode("name");
		if (!nameNode)
		{
			throw new GiPSiException("TextureDisplayManager constructor", "name node not found.");
			return;
		}
		char * textureName = nameNode->GetValue();
		delete nameNode;
		// make sure this isn't an existing texture's name
		for (int j = 0; j < i; j++)
		{
			if (strcmp(texture[j]->GetObjectName(), textureName) == 0)
			{
				char message[] = "Duplicate texture name found: %s.";
				int message_size = strlen(message) + strlen(textureName) + 1;
				char * formattedMessage = new char[message_size];
				sprintf_s(formattedMessage, message_size, message, textureName);
				throw new GiPSiException("TextureDisplayManager constructor", formattedMessage);
				delete formattedMessage;
				return;
			}
		}
		// or an existing SIMObject name
		for (int j = 0; j < num_object; j++)
		{
			if (strcmp(object[j]->GetName(), textureName) == 0)
			{
				char message[] = "Texture name found that matches an existing SIMObject name: %s.";
				int message_size = strlen(message) + strlen(textureName) + 1;
				char * formattedMessage = new char[message_size];
				sprintf_s(formattedMessage, message_size, message, textureName);
				throw new GiPSiException("TextureDisplayManager constructor", formattedMessage);
				delete formattedMessage;
				return;
			}
		}

		// get the texture type
		XMLNode * typeNode = textureChildren->GetNode("type");
		if (!typeNode)
		{
			throw new GiPSiException("TextureDisplayManager constructor", "type node not found.");
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
			throw new GiPSiException("TextureDisplayManager constructor", "Unrecognized texture type.");
			return;
		}
		delete typeNode;

		// format the texture file path
		XMLNode * pathNode = textureChildren->GetNode("path");
		if (!pathNode)
		{
			throw new GiPSiException("TextureDisplayManager constructor", "path node not found.");
			return;
		}
		char * path = pathNode->GetValue();
		delete pathNode;

		XMLNode * fileNameNode = textureChildren->GetNode("fileName");
		if (!fileNameNode)
		{
			throw new GiPSiException("TextureDisplayManager constructor", "fileName node not found.");
			return;
		}
		char * fileName = fileNameNode->GetValue();
		delete fileNameNode;

		int size = strlen(fileName) + strlen(path) + 2;
		char * formattedFileName = new char[size];
		sprintf_s(formattedFileName, size, "%s\\%s", path, fileName);
		delete path;
		delete fileName;

		// create the new texture
		texture[i] = new TextureDisplayManager(textureType, formattedFileName);
		texture[i]->SetObjectName(textureName);
		delete textureName;
		// the first texture is appended to the end of the SIMObject DisplayBuffer list.
		// all other textures are added to the previous texture.
		if (i == 0)
			object[num_object - 1]->displayMngr->GetDisplayBuffer()->SetNext(texture[i]->GetDisplayBuffer());
		else
			texture[i - 1]->GetDisplayBuffer()->SetNext(texture[i]->GetDisplayBuffer());
	}
}

/**
 * Create collisionDAR defined in the input project file node list.
 * 
 * @param collisionDARChildren Children of the collisionDAR XML project node.
 */
void SimulationKernel::CreateCollisionDAR(void)
{	
	// Collision for simulation object expect haptic interface object
	if(collisionDAR->collisionMethodParameters->GetEnabled()) 
	{
		const char * name;
		int id=0;	

		// Create new collision object
		name = collisionDAR->collisionMethodParameters->GetMethodName();		
		if (strcmp(name, "Basic") == 0) {
			if (collision) delete collision;
			collision = new BasicCollision();			
		}
		else if (strcmp(name, "PDepth") == 0) {
			if (collision) delete collision;
			collision = new PDepthCollision();
		}		
	
		// Add and build the bounding volume
		Boundary *bound = NULL;
		BoundingVolume *bv = NULL;
		if (boundingVolumes) delete boundingVolumes;
		boundingVolumes = new BoundingVolumes();
		
		for (int j = 0; j < num_object; j++)
		{	
			bound = object[j]->GetBoundaryPtr();
			if (bound!=NULL && object[j]->isCDEnable() && bound->isCollisionEnabledBoundaryType() && !((CollisionEnabledBoundary*)bound)->IsHapticAttached())
			{
				((CollisionEnabledBoundary*)bound)->SetDetection(object[j]->isCDEnable());
				((CollisionEnabledBoundary*)bound)->SetResponse(object[j]->isCREnable());				
				object[j]->SetID(id+1);				
				bv = new AABBBoundingVolume();
				bv->setBoundary((CollisionEnabledBoundary*)bound);
				bv->build();				
				boundingVolumes->add(bv);
				id++;
			}
		}

		// Add bounding volume into collision
		collision->setBoundingVolume(boundingVolumes);

		// Set collision rule
		int numOfCollisionEnabledObject = id;
		name = collisionDAR->collisionMethodParameters->Getfilename();		
		CollisionRule *collisionRule = new CollisionRule();			
		collisionRule->initialize(numOfCollisionEnabledObject);	
		// read file and set rule		
		collisionRule->setRule(collisionDAR->collisionMethodParameters->GetCollisionRule(), name, object, num_object);		
		collision->setCollisionRule(collisionRule);

		// Initialize collision
		collision->initialize();
	}

	// Collision for only haptic interface object
	if(collisionDAR->hapticCollisionMethodParameters->GetEnabled()) 
	{
		const char * name;
		int id = 0;	
		int	hid = 0;

		// Create new collision object
		name = collisionDAR->hapticCollisionMethodParameters->GetMethodName();		
		if (strcmp(name, "Haptic") == 0) {
			if (hapticCollision != NULL) delete hapticCollision;
			hapticCollision = new TLHapticCollision();			
		}		
	
		// Add collision boundary into collision
		Boundary *bound = NULL;
		BoundingVolume *hbv = NULL;
		if (hboundingVolumes) delete hboundingVolumes;
		hboundingVolumes = new BoundingVolumes();

		for (int j = 0; j < num_object; j++)
		{	
			bound = object[j]->GetBoundaryPtr();
			if (bound!=NULL && object[j]->isCDEnable() && bound->isCollisionEnabledBoundaryType())
			{
				((CollisionEnabledBoundary*)bound)->SetDetection(object[j]->isCDEnable());
				((CollisionEnabledBoundary*)bound)->SetResponse(object[j]->isCREnable());
				
				if (((CollisionEnabledBoundary*)bound)->IsHapticAttached()) {					
					object[j]->SetID(hid+1);
					hbv = new AABBBoundingVolume();
					hbv->setBoundary((CollisionEnabledBoundary*)bound);
					hbv->build();				
					hboundingVolumes->add(hbv);					
					hid++;
				}
				else {					
					object[j]->SetID(id+1);
					id++;
				}				
			}
		}
		// Add bounding volume into collision
		hapticCollision->setHBoundingVolume(hboundingVolumes);
		hapticCollision->setBoundingVolume(boundingVolumes);

		// Set collision rule
		int numOfCollisionEnabledObject = id;
		name = collisionDAR->hapticCollisionMethodParameters->Getfilename();		
		CollisionRule *collisionRule = new CollisionRule();		
		collisionRule->setType(true);	// set to haptic collision rule
		collisionRule->initialize(numOfCollisionEnabledObject, hid);	
		// read file and set rule		
		collisionRule->setRule(collisionDAR->hapticCollisionMethodParameters->GetCollisionRule(), name, object, num_object);		
		hapticCollision->setCollisionRule(collisionRule);

		// Initialize collision
		hapticCollision->initialize();
	}
}


/**
 * Create simulation order defined in the input project file node list.
 * 
 * @param simulationOrderNode The simulation order XML project node.
 */
void SimulationKernel::CreateSimulationOrder(XMLNode * simulationOrderNode)
{
	try
	{
		int index = 0;
		int num_simOrder_xml = 0;
		SimOrder **normalSimOrder;

		// number of simulation order is simObjects + connectors
		num_simOrder = num_object + num_connector;
		// create normal simulation order in appearance of simulation object and connector in XML
		normalSimOrder = new SimOrder*[num_simOrder];
		// get information for all simulation objects
		for (int i = 0; i < num_object; i++)
		{
			normalSimOrder[i] = new SimOrder();
			normalSimOrder[i]->setObjectPtr(object[i]);
			normalSimOrder[i]->setName(object[i]->GetName());
			normalSimOrder[i]->setType(1);			
			normalSimOrder[i]->setFlag(false);	
		}
		index = num_object;
		// get information for all connectors
		for (int i = 0; i < num_connector; i++)
		{
			normalSimOrder[index] = new SimOrder();
			normalSimOrder[index]->setObjectPtr(connector[i]);
			normalSimOrder[index]->setName(connector[i]->GetName());
			normalSimOrder[index]->setType(2);			
			normalSimOrder[index]->setFlag(false);	
			index++;
		}

		// create simulation order in order appear in XML
		simOrder = new SimOrder*[num_simOrder];

		// get the number of simulation order provided in XML
		if (simulationOrderNode->GetNumChildren() > 0)
		{
			XMLNodeList * simulationOrderChildren = simulationOrderNode->GetChildren();			
			num_simOrder_xml = simulationOrderChildren->GetLength();
			// for all object element in xml
			for (int i=0; i<num_simOrder_xml; i++)
			{			
				// new simOrder element
				simOrder[i] = new SimOrder();

				XMLNode * objectNode = simulationOrderChildren->GetNode(i);
				// get name node
				XMLNodeList * objectChildren = objectNode->GetChildren();			
				XMLNode * nameNode = objectChildren->GetNode("name");
				if (!nameNode)
				{
					throw new GiPSiException("Simulation order constructor", "name node not found.");
					return;
				}
				char * objectName = nameNode->GetValue();
				delete nameNode;

				// get type node
				XMLNode * typeNode = objectChildren->GetNode("type");
				if (!typeNode)
				{
					throw new GiPSiException("Simulation order constructor", "type node not found.");
					return;
				}
				char * typeVal = typeNode->GetValue();
				delete typeNode;

				// search object in normalSimOrder
				for(int j=0; j<num_simOrder; j++)
				{
					// found, then set in simOrder
					if (strcmp(objectName, normalSimOrder[j]->getName()) == 0) 
					{					
						if ((strcmp(typeVal, "simObject") == 0) && (normalSimOrder[j]->getType()==1))
							simOrder[i]->setType(normalSimOrder[j]->getType());	
						else 
						{
							if ((strcmp(typeVal, "connector") == 0) && (normalSimOrder[j]->getType()==2))
								simOrder[i]->setType(normalSimOrder[j]->getType());	
							else {
								throw new GiPSiException("Simulation order constructor", "type node conflict with name node.");
								return;
							}
						}					
						simOrder[i]->setObjectPtr(normalSimOrder[j]->getObjectPtr());
						simOrder[i]->setName(normalSimOrder[j]->getName());									
						normalSimOrder[j]->setFlag(true);
						break;
					}
									
				}
			} // end for all object element in xml
			delete simulationOrderChildren;
		}
		
		index = num_simOrder_xml;
		// add the simulation order for all other objects in normalSimOrder with flag = false
		for (int i = 0; i < num_simOrder; i++)
		{
			if (!normalSimOrder[i]->getFlag())	
			{
				simOrder[index] = new SimOrder();
				simOrder[index]->setObjectPtr(normalSimOrder[i]->getObjectPtr());
				simOrder[index]->setName(normalSimOrder[i]->getName());
				simOrder[index]->setType(normalSimOrder[i]->getType());			
				normalSimOrder[i]->setFlag(true);	
				index++;
			}
		}		

#ifdef _DEBUG
		// print simulation order
		printf("================================================\n");		
		printf("Simulation Order\n");
		for (int i = 0; i < num_simOrder; i++)
			printf("Type: %3d, Name: %s \n", simOrder[i]->getType(), simOrder[i]->getName());						
#endif

		delete [] normalSimOrder;
					
	}
	catch(...)
	{
		throw;
		return;
	}
}


/**
 * Starts the simulation thread.
 * 
 * @param arg Argument list containing the simulation kernel to start.
 */
void *StartSimulationThread(void *arg)
{
	int					status;
	SimulationKernel	*sim;

	sim = (SimulationKernel *) arg;

    // Detach itself
	/*
	Removed - Did not know how to safetly exit a detached thread.

    status = pthread_detach(pthread_self());
	*/
	sim->SimulationThread();

	return NULL;
}


/**
 * Write display data from the list of simulation objects.
 */
void SimulationKernel::Display(void)
{
	for(int i=0; i<num_texture; i++) {
		texture[i]->Display();
	}
	
	for(int i=0; i<num_object; i++) {
		object[i]->Display();
	}
}


/**
 * Main simulation loop.
 */
void SimulationKernel::SimulationThread()
{
	int					tc = 0;	
	Real				time1,time2;
	char				temp[256];

	bool				last_cycle_running = false;
	Real				oldgsimTimestep;
	Real				min_maxTimestep = 100.0;
	int					SKtimerID = 10;

	gsimTime = time;
	gsimTimestep = timestep;
	xmlTimestep = timestep;
	oldgsimTimestep = 0.0;

	// Find the minimum of maximun time step of all simulation object
	Real tmax;
	for(int i=0; i<num_object; i++) {
		tmax = object[i]->GetMaxTimestep();		
		if (tmax < min_maxTimestep)
			min_maxTimestep = tmax;
	}
	
	//printf("gsimTimestep = %lf, min_maxTimestep = %lf, gclockTimestep = %lf\n", gsimTimestep, min_maxTimestep, gclockTimestep);
	
	start_timer(SKtimerID);

	start_timer(2);	

	while(1) {
		gclockTimestep = get_timer(SKtimerID)/1000.0;
		if(last_cycle_running)
		{			
			if(xmlTimestep == 0.0)
				gsimTimestep = gclockTimestep;
			else
				gsimTimestep = min(xmlTimestep, min_maxTimestep);

			if(gsimTimestep > min_maxTimestep)
			{				
				sprintf(temp,"gsimTimestep(%lf) > min_maxTimestep(%lf)",gsimTimestep, min_maxTimestep);
				logger->Message("No real-time simulation.", temp, 1);
				gsimTimestep = min_maxTimestep;
			}			
		}
		else
		{
			if(xmlTimestep == 0.0)
				gsimTimestep = min_maxTimestep;
			else
				gsimTimestep = min(xmlTimestep, min_maxTimestep);
		}
		//printf("gsimTimestep = %lf, min_maxTimestep = %lf, gclockTimestep = %lf\n", gsimTimestep, min_maxTimestep, gclockTimestep);

		last_cycle_running = false;
		
		// set simTimestep to all simulation objects
		if (oldgsimTimestep != gsimTimestep)
		{
			for(int i=0; i<num_object; i++)
				object[i]->SetTimestep(gsimTimestep);	
		}

		oldgsimTimestep = gsimTimestep;

		start_timer(SKtimerID);
		
		if(this->IsRunning()) {
			
			time2=get_timer(2);  // Record the duration of last simulation kernel loop

			start_timer(2);   // Timer #2 measures the execution time of complete simuation kernel loop
							  //    including Simulate() and Display()
			start_timer(1);   // Timer #1 measures the execution time for the simulation computation, Simulate()

			// Perform one step of simulation for all simulation objects.
			this->Simulate();

			time1 = get_timer(1); // Record the duration of the simulation computation

			//printf("%lf, %lf ms\n", time1, time2);

			// If RTIME flag is set, we will store the timer measurements in recordtime[]
			if(this->RTIME) {				
				if(rtime_counter < 201) {
					recordtime[2*rtime_counter+1] = time1;
					recordtime[2*rtime_counter] = time2;	
					rtime_counter++;
				}
			}
			// Accumlate total time spent in Simulation Kernel Loop
			simTimeUse += time2;

			gclockTime += gclockTimestep;
 			gsimTime += gsimTimestep;
 	 		last_cycle_running = true;			
		}		

		// Display simulation objects.
		this->Display();

		// Execute user interface commands
		this->executeUICommand();

		// If we have been told to exit, then exit the simulation thread
		if (this->EXIT)
		{
			return;
		}
	}
}


/**
 * Perform one step of simulation for all simulation objects.
 */
inline void SimulationKernel::Simulate(void)
{
	int		i;

	for(i=0; i<num_simOrder; i++)
	{
		// if simulation order object is SIMObject
		if(simOrder[i]->getType()==1) 
		{
			// Simulaion object simulates
			((SIMObject*)simOrder[i]->getObjectPtr())->Simulate();
			// Reset boundary condition by apply type0 boundary to simobject			
			Boundary *bound = ((SIMObject*)simOrder[i]->getObjectPtr())->GetBoundaryPtr();
			if (bound!=NULL)
				bound->ResetBoundaryCondition();			
		}
		// if simulation order object is connector
		if(simOrder[i]->getType()==2)
			// Connector processes
			((Connector*)simOrder[i]->getObjectPtr())->process();
	}		
	
	// Collision Detection and Response
	if (collision->isEnabled())
	{
		collision->detection();
		collision->response();	
	}
	if (hapticCollision->isEnabled())
	{
		hapticCollision->detection();
		hapticCollision->response();	
	}	

	// Increse time
	time += timestep;	
}


/**
 * Returns the first DisplayBuffer in the SimulationKernel’s DisplayBuffer linked list.
 */
DisplayBuffer * SimulationKernel::GetDisplayBufferHead()
{
	if (!object)
		return NULL;

	return object[0]->displayMngr->GetDisplayBuffer();
}


void SimulationKernel::StartRecordTime(void)
{
	char temp[256];
	RTIME = true;
	rtime_counter = 0;
	recordtime = new double[402];
	if( recordtime == NULL) 
	{
		error_exit(-1, "Cannot allocate memory for recordtime!\n");
	}
	sprintf(temp,"Time: %lf, Simulation Time: %lf",time, simTimeUse/1000.0);
	logger->Message("Start   ",temp ,1);
}

void SimulationKernel::StopRecordTime(void)
{
	char temp[256];
	RTIME = false;	
	for(int j=0;j<rtime_counter-1;j++) 
	{
		//printf("%2.5lf, %2.5lf ms\n",recordtime[2*j], recordtime[2*(j+1)+1]-recordtime[2*j]); 
		sprintf(temp,"%2.5lf, %2.5lf ms",recordtime[2*j], recordtime[2*(j+1)+1]-recordtime[2*j]); 
		logger->Message("    ",temp ,1);
	}
	sprintf(temp,"Time: %lf, Simulation Time: %lf, Total record: %d",time, simTimeUse/1000.0, rtime_counter);
	logger->Message("End     ",temp ,1);
	delete recordtime;
}

bool SimulationKernel::setUICommand(const char * command)
{
	bool result = false;
	if(strcmp(uiCommand, "") == 0)
	{
		strcpy(uiCommand, command);
		result = true;
	}
	else
		logger->Message("SimulationKernel", "uiCommand is full.", 1);
	
	return result;
}

bool SimulationKernel::executeUICommand(void)
{
	bool result = false;
	if(strcmp(uiCommand, "") != 0)
	{
		result = true;
		if(strcmp(uiCommand, "simulation") == 0)
		{		
			if (this->IsRunning())
			{
				this->Stop();
				logger->Message("SimulationKernel", "Simulation is stopped.", 1);
			}
			else
			{
				this->Run();
				logger->Message("SimulationKernel", "Simulation is started.", 1);
			}
		}				
		else if(strcmp(uiCommand, "exit") == 0)
		{		
			this->Exit();
		}
		else if(strcmp(uiCommand, "gravity") == 0)
		{		
		}
		else if(strcmp(uiCommand, "collision") == 0)
		{		
		}
		else if(strcmp(uiCommand, "timestepUp") == 0)
		{		
			timestep += timestep*0.1;
			char temp[256];
			char name[256];
			Real tstep;
			bool tresult = false;

			for(int i=0; i<num_object; i++) {
				strcpy(name, object[i]->GetName());
				tstep = object[i]->GetTimestep();
				tresult = object[i]->SetTimestep(tstep+tstep*0.1);
				tstep = object[i]->GetTimestep();
			
				if (tresult)
					sprintf(temp,"Timestep of %s is increased to %f", name, tstep);
				else
					sprintf(temp,"Timestep of %s exceeds the max timestep, set timestep to %f", name, tstep);
				logger->Message("SimulationKernel", temp, 1);
			}
		}
		else if(strcmp(uiCommand, "timestepDown") == 0)
		{	
			timestep -= timestep*0.1;
			char temp[256];
			char name[256];
			Real tstep;
			bool tresult = false;
			
			for(int i=0; i<num_object; i++) {
				strcpy(name, object[i]->GetName());
				tstep = object[i]->GetTimestep();
				tresult = object[i]->SetTimestep(tstep-tstep*0.1);
				tstep = object[i]->GetTimestep();
			
				if (tresult)
					sprintf(temp,"Timestep of %s is decreased to %f", name, tstep);				
				else
					sprintf(temp,"Timestep of %s reachs the min timestep, set timestep to %f", name, tstep);
				logger->Message("SimulationKernel", temp, 1);
			}
		}
		else if(strcmp(uiCommand, "computationalHook") == 0)
		{	
			if (this->IsRecording())
			{
				this->StopRecordTime();
				logger->Message("SimulationKernel", "Record Time is stopped.", 1);
			}
			else
			{
				logger->Message("SimulationKernel", "Record Time is started.", 1);
				this->StartRecordTime();			
			}
		}
		else if(strcmp(uiCommand, "networkHook") == 0)
		{		
		}
		else if(strcmp(uiCommand, "inflateBalloon") == 0)
		{	
			// Find the balloon object
			for(int i=0;i<num_object;i++)
			{				
				if (strcmp(object[i]->GetType(), "BALLOON") == 0)
				{
					BalloonObject *balloon = (BalloonObject*)object[i];				
					balloon->Inflate();					
				}	
			}
		}
		else if(strcmp(uiCommand, "deflateBalloon") == 0)
		{	
			// Find the balloon object
			for(int i=0;i<num_object;i++)
			{				
				if (strcmp(object[i]->GetType(), "BALLOON") == 0)
				{
					BalloonObject *balloon = (BalloonObject*)object[i];				
					balloon->Deflate();					
				}	
			}

		}

		// set uiCommand to NULL
		strcpy(uiCommand, "");
	}
	return result;
}