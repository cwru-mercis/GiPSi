/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSi Visualization Engine Implementation
(GiPSiVisualizationEngine.cpp).

The Initial Developers of the Original Code is Svend Johannsen.  
Portions created by Svend Johannsen are Copyright (C) 2005.
All Rights Reserved.

Contributor(s): Svend Johannsen.
*/

/*
===============================================================================
	Headers
===============================================================================
*/

#include <GL/glut.h>

#include "GiPSiException.h"
#include "GiPSiVisualizationEngine.h"
#include "logger.h"

/*
===============================================================================
	VisualizationEngine
===============================================================================
*/

/**
 * Constructor.
 * 
 * @param dbHead Pointer to the list of DisplayBuffers.
 * @param newShaderLoader Initialized ShaderLoader to use for loading shader data.
 */
VisualizationEngine::VisualizationEngine(DisplayBuffer * dbHead, ShaderLoader * newShaderLoader)
	:	shaderLoader(newShaderLoader)
{
	// Create Displaybuffers
	this->nBuffers		= 0;
	this->allBuffers	= NULL;
	this->nScenes		= 0;
	this->allScenes		= NULL;
	this->nTextures		= 0;
	this->allTextures	= NULL;
	this->nShaders		= 0;
	this->allShaders	= NULL;

	CreateDisplayBufferArray(dbHead);
}

/**
 * Constructs an array of DisplayBuffers (for ease of use) using the input DisplayBuffer pointer.
 * 
 * @param newShaderLoader Initialized ShaderLoader to use for loading shader data.
 */
void VisualizationEngine::CreateDisplayBufferArray(DisplayBuffer * dbHead)
{
	if (nBuffers > 0)
	{
		logger->Message("VisualizationEngine.CreateDisplayBufferArray", "DisplayBuffer array already constructed.", 3);
		return;
	}
	DisplayBuffer * next = dbHead;
	while (next)
	{
		nBuffers++;
		next = next->GetNext();
	}

	allBuffers = new DisplayBuffer*[nBuffers];
	next = dbHead;
	for (int i = 0; i < nBuffers; i++)
	{
		allBuffers[i] = next;
		next = next->GetNext();
	}
}

/**
 * Destructor.
 */
VisualizationEngine::~VisualizationEngine()
{
	if (nScenes > 0 && allScenes)
	{
		for (int i = 0; i < nScenes; i++)
			if (allScenes[i])
			{
				delete allScenes[i];
				allScenes[i] = NULL;
			}
		delete allScenes;
		allScenes = NULL;
	}
	if (nBuffers > 0 && allBuffers)
	{
		delete allBuffers;
		allBuffers = NULL;
	}
	if (nTextures > 0 && allTextures)
	{
		// The texture buffers themselves are deleted by the buffers block;
		// Just delete the array here
		delete allTextures;
		allTextures = NULL;
	}
	if (nShaders > 0 && allShaders)
	{
		// The shader buffers themselves are deleted by the buffers block;
		// Just delete the array here
		delete allShaders;
		allShaders = NULL;
	}
/*
	if (simKernel)
	{
		delete simKernel;
		simKernel = NULL;
	}
*/
	if (shaderLoader)
	{
		delete shaderLoader;
		shaderLoader = NULL;
	}
}

/**
 * Initalizes VisualizationEngine using 'visualization' XML project node.
 */
void VisualizationEngine::Init(XMLNode * visualizationNode)
{
	try
	{
		// Setup OpenGL

		glClearDepth(1.0f);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
		//glEnable(GL_CULL_FACE);		
		glDisable(GL_CULL_FACE);
		
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK,	GL_LINE);

		// Load Textures
		this->nTextures = 0;
		for (int i = 0; i < this->nBuffers; i++)
		{
			if (this->allBuffers[i]->GetTextureType() != GIPSI_NO_TEXTURE)
				this->nTextures++;
		}

		int iTexture = 0;
		this->allTextures = new Texture*[this->nTextures];
		for (int i = 0; i < this->nBuffers && iTexture < this->nTextures; i++)
		{
			switch (this->allBuffers[i]->GetTextureType())
			{
			case GIPSI_2D_STATIC_CLIENT:
			case GIPSI_2D_STATIC_SERVER:
			case GIPSI_2D_DYNAMIC_SERVER:				
				this->allTextures[iTexture] = new Texture2D(this->allBuffers[i]);
				iTexture++;
				break;
			case GIPSI_3D_STATIC_CLIENT:
				this->allTextures[iTexture] = NULL;
	//			this->allTextures[iTexture] = new Texture3D(this->allBuffers[i]);
				iTexture++;
				break;
			case GIPSI_NO_TEXTURE:
			default:
				break;
			}
		}

		XMLNodeList * visualizationChildren = visualizationNode->GetChildren();

		// Load Shaders

		if (Shader::Compatibility())
		{
			XMLNode * shadersNode		= visualizationChildren->GetNode("shaders");
			if (shadersNode->GetNumChildren() > 0)
			{
				XMLNodeList * shaderList	= shadersNode->GetChildren();
				this->nShaders				= shaderList->GetLength();
				this->allShaders			= new Shader*[this->nShaders];

				for (int i = 0; i < this->nShaders; i++)
				{
					XMLNode * shaderNode	= shaderList->GetNode(i);
					this->allShaders[i]		= shaderLoader->LoadShader(shaderNode);

					// make sure we haven't loaded another shader by this name
					for (int j = 0; j < i; j++)
					{
						if (this->allShaders[j]->GetShaderName() == this->allShaders[i]->GetShaderName())
						{
							throw new GiPSiException("GiPSiVisualizationEngine.Init", "Duplicate shader entry found in XML project file.");
							delete shadersNode;
							delete shaderList;
							delete visualizationChildren;
							return;
						}
					}
				}
				delete shaderList;
			}
			delete shadersNode;
		}
		else
		{
			logger->Error("VisualizationEngine.Init", "Driver does not support OpenGL Shading Language.");
		}

		// Create Scenes
		XMLNode * scenesNode	= visualizationChildren->GetNode("scenes");
		XMLNodeList * sceneList	= scenesNode->GetChildren();
		this->nScenes			= sceneList->GetLength();
		this->allScenes			= new Scene*[this->nScenes];

		// For each scene defined in the XML project file,
		for (unsigned int i = 0; i < this->nScenes; i++)
		{
			// Init a new scene using the scene node
			XMLNode * sceneNode = sceneList->GetNode(i);
			this->allScenes[i]	= new Scene(sceneNode, this->nBuffers, this->allBuffers, this->nTextures, this->allTextures, this->nShaders, this->allShaders);
		}

		delete visualizationChildren;
		delete sceneList;
	}
	catch (...)
	{
		throw;
		return;
	}
}
