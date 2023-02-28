/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Main Simulation Kernel Definition (simulator.h).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu, Suriya Natsupakpong
*/

////	SIMULATOR.H v0.1.0
////
////	Defines the simulation kernel
////
////////////////////////////////////////////////////////////////


#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include <pthread.h>

#include "CollisionDARLoader.h"
#include "CollisionDARParameters.h"
#include "ConnectorLoader.h"
#include "GiPSiAPI.h"      
#include "ObjectLoader.h"
#include "XMLNode.h"
#include "XMLNodeList.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;

/****************************************************************
 *							SIMULATOR							*  
 ****************************************************************/

// The main Simulation Kernel
class SimulationKernel {
public:
	Real				g;					// Gravity (= 1.0 g*cm/s2)	
    SIMObject*			object[MAX_SIMOBJ];	// Object array
	int					num_object;			// Number of objects
	
	// Constructors
	SimulationKernel(	XMLNode * rootNode,
						ObjectLoader * objLoader,
						ConnectorLoader * connectorLoader,
						CollisionDARLoader * CDARLoader);

	SimulationKernel() {};
	~SimulationKernel();

	void				Display(void);
	virtual void		SimulationThread(void);

	/**
	 * Return simulation thread.
	 */
	pthread_t		   *GetSimThread(void)	{	return &simThread; }

	/**
	 * Return run status.
	 */
	bool				IsRunning(void)		{	return RUN; }

	/**
	 * Set run status to 'true'.
	 */
	void				Run(void)			{	RUN = true; }

	/**
	 * Set run status to 'false'.
	 */
	void				Stop(void)			{	RUN = false; }

	/**
	 * Return exit status.
	 */
	bool				IsExiting(void)		{	return EXIT; }

	/**
	 * Set exit status to 'true'.
	 */
	void				Exit(void)			{	EXIT = true; }

	bool				IsRecording(void)		{ return RTIME; }
	void				StartRecordTime(void);
	void				StopRecordTime(void);

	/**
	 * Set uiCommand.
	 */
	virtual bool		setUICommand(const char * command);
	/**
	 * Get and Execute uiCommand.
	 */
	bool				executeUICommand(void);

	DisplayBuffer	   *GetDisplayBufferHead();

protected:
	void				SetGeneralProjectParameters(XMLNodeList * generalParametersChildren);
	void				CreateSimulationObjects(XMLNodeList * simulationObjectsChildren);
	void				CreateConnectors(XMLNodeList * connectorsChildren);
	void				CreateTextures(XMLNodeList * texturesChildren);
	void				CreateCollisionDAR(void);
	void				CreateSimulationOrder(XMLNode * simulationOrderNode);

	virtual void		Simulate(void);

	ObjectLoader	   *objLoader;
	ConnectorLoader	   *connectorLoader;
	CollisionDARLoader *CDARLoader;

	CollisionDARParameters *collisionDAR;

	char			   *name;				// Name of the project
	Real				time;				// Global Clock time	
	Real				simTimeUse;			// Global Simulation time
	Real				timestep;			// Global time step (= 0.01 s) 

	Real				xmlTimestep;		// Time step from xml project file
	Real				gclockTime;			// Global clock time
	Real				gclockTimestep;		// Global clock time step
	Real				gsimTime;			// Global simulation time
	Real				gsimTimestep;		// Global simulation time step

	Connector		   *connector[MAX_CONNECTOR];
	int					num_connector;		// Number of connectors
    DisplayArray		display;			// Global display
    pthread_t			simThread;			// The main simulation thread
	bool				RUN;
	bool				EXIT;

	bool				RTIME;				// record computation time
	double				*recordtime;		// record time data
	int					rtime_counter;		// record time counter

	int					num_texture;		// number of textures
	TextureDisplayManager **texture;

	int					num_simOrder;		// number of simulation order
	SimOrder			**simOrder;			// list of simulation order

	BoundingVolumes	   *boundingVolumes;	// Bounding Volumes
	BoundingVolumes	   *hboundingVolumes;	// Haptic Bounding Volumes
	Collision		   *collision;			// Collision 
	HapticCollision	   *hapticCollision;	// Haptic Collision	
	
	bool				computationalHook;	// Computational hook flag
	bool				networkHook;		// Network hook flag
	Real				simTime;			// Simulation time step

	char			   uiCommand[20];		// User Interface Command
	friend LoaderUnitTest;
};

// Simulation thread
void *StartSimulationThread(void *arg);

#endif