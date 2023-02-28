/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Visualization Engine Definition
(GiPSiVisualizationEngine.h).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

#ifndef GIPSIVISUALIZATIONENGINE__H
#define GIPSIVISUALIZATIONENGINE__H

/*
===============================================================================
	Headers
===============================================================================
*/

#include "GiPSiScene.h"
#include "GiPSiShader.h"
#include "GiPSiTexture.h"
#include "ShaderLoader.h"
#include "simulator.h"
#include "XMLNode.h"

using namespace GiPSiXMLWrapper;

class LoaderUnitTest;

/*
===============================================================================
	Interface independant visualization engine class
===============================================================================
*/

class VisualizationEngine
{
public:
	VisualizationEngine(DisplayBuffer * dbHead, ShaderLoader * newShaderLoader);
	~VisualizationEngine();
	virtual void Start(SimulationKernel * newSimKernel) { simKernel = newSimKernel; }
	SimulationKernel * simKernel;
protected:
	int					 nScenes;
	Scene			   **allScenes;
	int					 nBuffers;
	DisplayBuffer	   **allBuffers;
	int					 nTextures;
	Texture			   **allTextures;
	int					 nShaders;
	Shader			   **allShaders;
	
	void Init(XMLNode * visualizationNode);
	virtual void OnDisplay		(void) = 0;
	virtual void OnKeyboard		(unsigned char key, int x, int y) = 0;
	virtual void OnMouseClick	(int button, int state, int x, int y) = 0;
	virtual void OnMouseMotion	(int x, int y) = 0;
	virtual void OnReshape		(int width, int height) = 0;

	void CreateDisplayBufferArray(DisplayBuffer * dbHead);
	
	friend LoaderUnitTest;
	
protected:
	ShaderLoader * shaderLoader;

	
};

#endif // #ifndef GIPSIVISUALIZATIONENGINE__H

