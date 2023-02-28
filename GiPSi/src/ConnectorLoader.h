/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is Connector Loader Definition (ConnectorLoader.h).

The Initial Developer of the Original Code is John Pao.
Portions created by John Pao are Copyright (C) 2006.
All Rights Reserved.

Contributor(s): John Pao.
*/

////	CONNECTOR LOADER.H v0.0
////
////	Connector Loader
////
////////////////////////////////////////////////////////////////

#ifndef _CONNECTOR_LOADER_H_
#define _CONNECTOR_LOADER_H_

#include "GiPSiAPI.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class ConnectorLoader
{
public:
	/**
	 * Load connector object.
	 * 
	 * @param connectorNode Project file XML 'connector' node.
	 * @param objects List of simulation objects.
	 * @param num_objects Number of simulation objects in 'objects'.
	 */
	virtual Connector * LoadConnector(XMLNode * connectorNode, SIMObject * objects[MAX_SIMOBJ], int num_objects) = 0;
};

#endif
