/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Toolkit Connector Loader Implementation (ToolkitConnectorLoader.cpp).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao, Suriya Natsupakpong.
*/

////	TOOLKITCONNECTORLOADER.CPP v0.0
////
////	Toolkit Connector Loader
////
////////////////////////////////////////////////////////////////

#include "bioe.h"
#include "connector.h"
#include "fem.h"
#include "GiPSiException.h"
#include "lumpedfluid.h"
#include "msd.h"
#include "simple.h"
#include "ToolkitConnectorLoader.h"
#include "XMLNodeList.h"

/**
 * Load connector object.
 * 
 * @param connectorNode Project file XML 'connector' node.
 * @param objects List of simulation objects.
 * @param num_objects Number of simulation objects in 'objects'.
 */
Connector * ToolkitConnectorLoader::LoadConnector(XMLNode * connectorNode, SIMObject * objects[MAX_SIMOBJ], int num_objects)
{
	try
	{
		XMLNodeList * connectorChildren = connectorNode->GetChildren();
		XMLNode * typeNode = connectorChildren->GetNode("type");
		const char * type = typeNode->GetValue();

		Connector * newConnector = NULL;
		if (strcmp(type, "FEM/CBE") == 0)
		{
			newConnector = new FEM3LM_BIOE_Connector(connectorNode, objects, num_objects);
		}
		else if (strcmp(type, "FEM/LF") == 0)
		{
			newConnector = new FEM3LM_LUMPEDFLUID_Connector(connectorNode, objects, num_objects);
		}
		else if (strcmp(type, "QSDS/MSD") == 0)
		{
			newConnector = new QSDS_MSD_Connector(connectorNode, objects, num_objects);
		}
		else if (strcmp(type, "ENDO/CAT") == 0)
		{
			newConnector = new Endo_Catheter_Connector(connectorNode, objects, num_objects);
		}
		else if (strcmp(type, "CAT/BALL") == 0)
		{
			newConnector = new Catheter_Balloon_Connector(connectorNode, objects, num_objects);
		}
		else
		{
			char error[256]("");
			sprintf_s(error, 256, "Unrecognized connector type %s", type);
			throw new GiPSiException("ConnectorLoader", error);
			return NULL;
		}
		delete typeNode;
		delete connectorChildren;
		delete type;

		return newConnector;
	}
	catch(...)
	{
		throw;
		return NULL;
	}
}
