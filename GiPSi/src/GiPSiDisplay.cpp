/*
The contents of this file are subject to the GiPSi Public License
Version 1.0 (the "License"); you may not use this file except in
compliance with the License. You may obtain a copy of the License at
http://gipsi.case.edu/GiPSiPL/

Software distributed under the License is distributed on an "AS IS"
basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
License for the specific language governing rights and limitations
under the License.

The Original Code is GiPSiDisplay Implementation (GiPSiDisplay.cpp).

The Initial Developers of the Original Code are Tolga Goktekin and M. Cenk Cavusoglu.  
Portions created by Tolga Goktekin and M. Cenk Cavusoglu are Copyright (C) 2004.
All Rights Reserved.

Contributor(s): Tolga Goktekin, M. Cenk Cavusoglu.
*/

////	GiPSiDisplay.cpp v1.0
////
////	Implements TriSurfaceDisplayManager and TetVolumeDisplayManager
////
////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

#include "algebra.h"
#include "errors.h"
#include "GiPSiDisplay.h"
#include "GiPSiException.h"
#include "logger.h"

/**
 * Finds and returns the XMLNode in the input XMLNodeList corresponding
 * to the input target name.
 * 
 * @param params XMLNodeList containing a list of shader 'param' nodes.
 * @param targetName Character string containing the name of the node to be returned.
 */
XMLNode * ShaderParams::FindParam(XMLNodeList * params, const char * targetName)
{
	try
	{
		XMLNode * param = NULL;
		int length = params->GetLength();
		for (int i = 0; i < length && param == NULL; i++)
		{
			XMLNode * node = params->GetNode(i);
			XMLNodeList * nodeChildren = node->GetChildren();
			XMLNode * nameNode = nodeChildren->GetNode("name");
			char * paramName = nameNode->GetValue();
			if (strcmp(paramName, targetName) == 0)
				param = node;
			delete paramName;
			delete nameNode;
			delete nodeChildren;
		}

		if (param == NULL)
		{
			char ex[256];
			sprintf_s(ex, 256, "Param \'%s\' not found.", targetName);
			throw new GiPSiException("ShaderParams.FindParam", ex);
		}

		return param;
	}
	catch (...)
	{
		throw;
		return NULL;
	}
}

/**
 * Returns the 'value' node of the requested param in the input XMLNodeList.
 *
 * @param params XMLNodeList containing a list of shader 'param' nodes.
 * @param targetName Character string containing the name of the node to be returned.
 */
const char * ShaderParams::GetValue(XMLNodeList * params, const char * targetName)
{
	try
	{
		XMLNode * param = FindParam(params, targetName);
		XMLNodeList * paramChildren = param->GetChildren();
		XMLNode * paramValueNode = paramChildren->GetNode("value");
		const char * paramStr = paramValueNode->GetValue();
		delete paramValueNode;
		delete paramChildren;
		delete param;

		return paramStr;
	}
	catch (...)
	{
		throw;
		return NULL;
	}
}

/**
 * Constructor.  Allocates and initializes members.
 *
 * @param manager Pointer to the display manager that created this DisplayBuffer.
 * @param inheader The header that this DisplayBuffer will use to calculate the DisplayArray sizes.
 * @param textType The type of texture contained by this DisplayBuffer, default GIPSI_NO_TEXTURE.
 */
DisplayBuffer::DisplayBuffer(DisplayManager * manager, const DisplayHeader * inheader, DisplayTextureType textType) :
	manager(manager),	header(NULL),		data(NULL),
	name(NULL),			objectName(NULL),	next(NULL),
	nTextures(0),		texture(NULL),		textureType(textType),
	writeIndex(0),		queueIndex(1),		readIndex(2),
	nArrays(3),			shaderParams(NULL)
	
{
	// init mutex
	mutex = PTHREAD_MUTEX_INITIALIZER;

	// If this is static data, only alloc one array
	if (textureType == GIPSI_2D_STATIC_CLIENT ||
		textureType == GIPSI_2D_STATIC_SERVER ||
		textureType == GIPSI_3D_STATIC_CLIENT)
	{
		nArrays = 1;
		queueIndex = 0;
		readIndex = 0;
	}

	if (inheader)
		SetDisplayHeader(inheader);

	// allocate DisplayArray and full members
	data	= new DisplayArray[nArrays];

	// initialize DisplayArray members
	for (int i = 0; i < nArrays; i++)
	{
		data[i].full				= false;
		data[i].DISPARRAY_NODESIZE	= 0;
		data[i].dA_size				= 0;
		data[i].iA_size				= 0;
		data[i].dispArray			= NULL;
		data[i].indexArray			= NULL;
		GetDisplayHeader(&(data[i].header));
	}

	// No next yet
	next					= NULL;
}


/**
 * Deallocate any members currently allocated.
 */
DisplayBuffer::~DisplayBuffer()
{
	if (header)
	{
		delete header;
		header = NULL;
	}

	if (shaderParams)
	{
		delete shaderParams;
		shaderParams = NULL;
	}

	if (data)
	{
		if (nArrays > 0)
		{
			for (int i = 0; i < nArrays; i++)
			{
				if (data[i].iA_size > 0
					&& data[i].indexArray)
				{
					delete data[i].indexArray;
					data[i].indexArray = NULL;
				}
				if (data[i].dA_size > 0
					&& data[i].dispArray)
				{
					delete data[i].dispArray;
					data[i].dispArray = NULL;
				}
			}
		}
		delete data;
		data = NULL;
	}

	if (texture)
	{
		if (nTextures > 0)
		{
			for (int i = 0; i < nTextures; i++)
			{
				delete texture[i];
				texture[i] = NULL;
			}
		}
		delete texture;
		texture = NULL;
	}

	if (name)
	{
		delete name;
		name = NULL;
	}
	if (objectName)
	{
		delete objectName;
		objectName = NULL;
	}
}


/**
 * Returns a pointer to the DisplayArray in the readIndex position.
 */
DisplayArray * DisplayBuffer::GetReadArray()
{
	return &(data[readIndex]);
}


/**
 * Initializes members of the DisplayArray in the writeIndex position
 * for input.
 * Returns a pointer to the DisplayArray in the writeIndex position.
 * 
 * @param visible True if the object should be displayed, false otherwise
 */
DisplayArray * DisplayBuffer::GetWriteArray(bool visible)
{
	int temp_DISPARRAY_NODESIZE	= manager->GetDISPARRAY_NODESIZE(header);
	int temp_dA_size			= manager->GetdA_size(temp_DISPARRAY_NODESIZE);
	int temp_iA_size			= manager->GetiA_size(header);
	return GetWriteArray(temp_DISPARRAY_NODESIZE, temp_dA_size, temp_iA_size, visible);
}


/**
 * Initializes members of the DisplayArray in the writeIndex position
 * for input.
 * Returns a pointer to the DisplayArray in the writeIndex position.
 *
 * @param DISPARRAY_NODESIZE The node size of the data packed in the dispArray array.
 * @param dA_size The length of the dispArray array.
 * @param iA_size The length of the indexArray array.
 * @param visible True if the object should be displayed, false otherwise.
 */
DisplayArray * DisplayBuffer::GetWriteArray(int DISPARRAY_NODESIZE, int dA_size, int iA_size, bool visible)
{
	// if we have indexArray content of an inappropriate size, it should be deallocated
	if (iA_size != data[writeIndex].iA_size ||
		!visible)
	{
		if (data[writeIndex].indexArray)
		{
			delete data[writeIndex].indexArray;
			data[writeIndex].indexArray = NULL;
			data[writeIndex].iA_size = 0;
		}
	}

	// if we have dispArray content of an inappropriate size, it should be deallocated
	if (dA_size != data[writeIndex].dA_size ||
		!visible)
	{
		if (data[writeIndex].dispArray)
		{
			delete data[writeIndex].dispArray;
			data[writeIndex].dispArray = NULL;
			data[writeIndex].dA_size = 0;
		}
	}

	// if the object is visible, allocate arrays in shared memory to hold the data
	if (visible)
	{
		if (header)
			GetDisplayHeader(&(data[writeIndex].header));
		data[writeIndex].DISPARRAY_NODESIZE = DISPARRAY_NODESIZE;
		data[writeIndex].iA_size = iA_size;
		data[writeIndex].dA_size = dA_size;

		// alloc dispArray if size indicated as > 0 and it is not yet allocated
		if (data[writeIndex].indexArray == NULL)
			if (iA_size > 0)
				data[writeIndex].indexArray = new unsigned int[data[writeIndex].iA_size];

		// alloc indexArray if size indicated as > 0 and it is not yet allocated
		if (data[writeIndex].dispArray == NULL)
			if (dA_size > 0)
				data[writeIndex].dispArray = new float[data[writeIndex].dA_size];
	}

	return &data[writeIndex];
}


/**
 * Acquire the lock associated with the queue.
 * Call DequeueExchange.
 */
void DisplayBuffer::Dequeue()
{
	// mutex.lock
	int rc = pthread_mutex_lock(&mutex);
	if (rc)		/* an error has occurred */
	{
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}

	DequeueExchange();
}


/**
 * Acquire the lock associated with the queue.
 * If the queue is empty, release the lock associated with the queue and return.
 *		Otherwise, execute a dequeue exchange.
 */
void DisplayBuffer::ConditionalDequeue()
{
	// mutex.lock
	int rc = pthread_mutex_lock(&mutex);
	if (rc)		/* an error has occurred */
	{
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}

	if (!data[queueIndex].full)
	{
		// mutex.unlock
		rc = pthread_mutex_unlock(&mutex);
		if (rc)
		{
			perror("pthread_mutex_unlock");
			pthread_exit(NULL);
		}

		return;
	}

	DequeueExchange();
}


/**
 * Swap the queue index and the output buffer index.
 * Set the queue to be empty.
 * Release the lock on the queue.
 */
void DisplayBuffer::DequeueExchange()
{
	// swap read index and queue index
	int tempIndex = readIndex;
	readIndex = queueIndex;
	queueIndex = tempIndex;

	// set queue to empty
	data[queueIndex].full = false;

	// mutex.unlock
	int rc = pthread_mutex_unlock(&mutex);
	if (rc)
	{
		perror("pthread_mutex_unlock");
		pthread_exit(NULL);
	}
}


/**
 * Acquire the lock associated with the queue.
 * Swap the queue index with the input buffer index.
 * Set the queue to full.
 * Release the lock on the queue.
 */
void DisplayBuffer::Enqueue()
{
	// mutex.lock()
	int rc = pthread_mutex_lock(&mutex);
	if (rc)		/* an error has occurred */
	{
		perror("pthread_mutex_lock");
		pthread_exit(NULL);
	}

	// swap write index and queue index
	int tempIndex = writeIndex;
	writeIndex = queueIndex;
	queueIndex = tempIndex;

	// set queue to full
	data[queueIndex].full = true;

	// mutex.unlock()
	rc = pthread_mutex_unlock(&mutex);
	if (rc)
	{
		perror("pthread_mutex_unlock");
		pthread_exit(NULL);
	}
}


/**
 * Set the member DisplayHeader’s values to the input DisplayHeader’s values.
 *
 * @param inheader New values for the DisplayBuffer's stored DisplayHeader.
 */
void DisplayBuffer::SetDisplayHeader(const DisplayHeader * inheader) {
	if (!header)
		header = new DisplayHeader();
	header->dataType		= inheader->dataType;
	header->lineSize		= inheader->lineSize;
	header->objType			= inheader->objType;
	header->pointSize		= inheader->pointSize;
	header->polyMode		= inheader->polyMode;
	header->shadeMode		= inheader->shadeMode;

	UpdateDataType();
}


/**
 * Sets outheader’s values using the stored GetDisplayHeader
 * method.  Returns true if successful, false if the operation
 * cannot be completed.
 *
 * @param outheader Set to the current values of the DisplayBuffer's stored DisplayHeader.
 */
bool DisplayBuffer::GetDisplayHeader(DisplayHeader * outheader) {
	if (!header)
		return false;
	outheader->dataType		= header->dataType;
	outheader->lineSize		= header->lineSize;
	outheader->objType		= header->objType;
	outheader->pointSize	= header->pointSize;
	outheader->polyMode		= header->polyMode;
	outheader->shadeMode	= header->shadeMode;
	return true;
}


/**
 * Returns a pointer to the DisplayBuffer's shader parameters.
 */
ShaderParams * DisplayBuffer::GetShaderParams()
{
	return shaderParams;
}

/**
 * Sets the DisplayBuffer's shader parameters.
 *
 *@param newShaderParams The new shader parameters to use.
 */
void DisplayBuffer::SetShaderParams(ShaderParams * newShaderParams)
{
	if (shaderParams)
	{
		logger->Message("DisplayBuffer.SetName", "ShaderParams already set.", 3);
		return;
	}
	shaderParams = newShaderParams;

	UpdateDataType();
}



/**
 * Updates the header information to include the shader attribute mask, if one is specified.
 */
void DisplayBuffer::UpdateDataType()
{
	// If both the header and shader are present...
	if (header)
		if (shaderParams)
			// update the header dataType with the shader attribute mask
			this->header->dataType |= this->shaderParams->GetShaderAttributes();
}



/**
 * Gets the DisplayBuffer name.
 */
const char * DisplayBuffer::GetName()
{
	return this->name;
}


/**
 * Sets the DisplayBuffer name.  This method does not set the DisplayBuffer
 * name if the name has previously been set.
 *
 * @param newName Set the name of the DisplayBuffer.
 */
void DisplayBuffer::SetName(const char * newName)
{
	if (name)
	{
		logger->Message("DisplayBuffer.SetName", "Name already set.", 3);
		return;
	}
	this->name = new char[strlen(newName) + 1];
	strcpy_s(this->name, strlen(newName) + 1, newName);
}


/**
 * Gets the name of the object that this DisplayBuffer represents.
 */
const char * DisplayBuffer::GetObjectName()
{
	return this->objectName;
}


/**
 * Sets the name of the object that this DisplayBuffer represents.
 * This method does not set the name if the name has previously been set.
 *
 * @param newObjectName Set the name of the DisplayBuffer.
 */
void DisplayBuffer::SetObjectName(const char * newObjectName)
{
	if (objectName)
	{
		logger->Message("DisplayBuffer.SetObjectName", "Object name already set.", 3);
		return;
	}
	this->objectName = new char[strlen(newObjectName) + 1];
	strcpy_s(this->objectName, strlen(newObjectName) + 1, newObjectName);
}


/**
 * Sets the next db name in shared memory.
 * Allocs nextName.
 * This method does not set the DisplayBuffer 'next' if 'next' has previously been set.
 *
 * @param newNext Pointer to the DisplayBuffer to be set as 'next'.
 */
void DisplayBuffer::SetNext(DisplayBuffer * newNext)
{
	/*
	if (next != NULL)
	{
		logger->Message("DisplayBuffer.SetNext", "Next DisplayBuffer already set.", 3);
		return;
	}

	this->next		= newNext;
	*/
	//sxn66 edit this to gain multi display buffer in one simulation object

	DisplayBuffer *current = this;
	while (current->next != NULL) {		
		//printf("current object = %s\n",current->GetObjectName());		
		current = current->next;		
	}
	current->next = newNext;	
}


/**
 * Returns address of the next db in the linked list
 */
DisplayBuffer * DisplayBuffer::GetNext()
{
	return next;
}


void DisplayBuffer::AddTexture(char * textName)
{
	// create new arrays to hold the texture existing information plus the new texture information
	char ** newTexture = new char*[nTextures + 1];

	// fill in the new arrays with the existing information
	for (int i = 0; i < nTextures; i++)
	{
		newTexture[i] = texture[i];
	}

	// delete any old arrays
	if (texture != NULL)
		delete texture;

	// alloc the new texture name array
	newTexture[nTextures] = new char[strlen(textName) + 1];

	// copy the new texture name into the allocated file
	strcpy_s(newTexture[nTextures], strlen(textName) + 1, textName);

	// update member variables
	nTextures++;
	texture = newTexture;
}


////////////////////////////////////////////////////////////////
//
//	PointCloudDisplayManager::Display()
//
//
void PointCloudDisplayManager::Display(void)
{
    unsigned int	i;
	int				offset;

	DisplayArray*	display = displayBuffer.GetWriteArray(visible);
	char			type = display->header.dataType;
	float*			darray = display->dispArray;
    unsigned int*	iarray = display->indexArray;

    for(i=0; i<geometry->num_vertex; i++) {
		offset = 0;

		if(IS_RGB_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
		} else if(IS_RGBA_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[3];
		}

		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[0];
		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[1];
		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[2];

		if (IS_TANGENT_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[2];
		}
    }
	for(i=0; i<geometry->num_vertex; i++)
		iarray[i]	= i;

	displayBuffer.Enqueue();
}


/**
 * Calculate the DISPARRAY_NODESIZE for a DisplayArray
 *
 * @param header Pointer to the DisplayHeader values to use to calculate the DISPARRAY_NODESIZE.
 */
int PointCloudDisplayManager::GetDISPARRAY_NODESIZE(const DisplayHeader * header)
{
	int DISPARRAY_NODESIZE = 3;
	
	char	type = header->dataType;

	if(IS_RGB_ON(type))
		DISPARRAY_NODESIZE += 3;
	else if(IS_RGBA_ON(type))
		DISPARRAY_NODESIZE += 4;

	if (IS_TANGENT_ON(type))
		DISPARRAY_NODESIZE += 3;

	return DISPARRAY_NODESIZE;
}


/**
 * Calculate the dispArray length for a DisplayArray
 *
 * @param DISPARRAY_NODESIZE The node size of the DisplayArray.
 */
int PointCloudDisplayManager::GetdA_size(int DISPARRAY_NODESIZE)
{
	return geometry->num_vertex * DISPARRAY_NODESIZE;
}

/**
 * Calculate the indexArray length for a DisplayArray
 *
 * @param header Pointer to the DisplayHeader values to use to calculate the indexArray size.
 */
int PointCloudDisplayManager::GetiA_size(const DisplayHeader * header)
{
	return geometry->num_vertex;
}


/**
 * Write display data to buffer.
 */
void VectorFieldDisplayManager::Display(void)
{
    unsigned int	i,j;
	int				offset1,offset2;

	DisplayArray*	display = displayBuffer.GetWriteArray(visible);
	char			type = display->header.dataType;
	float*			darray = display->dispArray;
    unsigned int*	iarray = display->indexArray;

	for(i=0; i<geometry->num_vertex; i++) {
		offset1 = 0;
		offset2 = 0;
		j=geometry->num_vertex+i;
		if(IS_RGB_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].color[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].color[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].color[2];

			darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].color[0];
			darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].color[1];
			darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].color[2];
		} else if(IS_RGBA_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].color[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].color[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].color[2];
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].color[3];

			darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].color[0];
			darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].color[1];
			darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].color[2];
			darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].color[3];
		}

		darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].pos[0];
		darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].pos[1];
		darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].pos[2];

		darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].pos[0]+geometry->vertex[i].pos[0];
		darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].pos[1]+geometry->vertex[i].pos[1];
		darray[(j*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].pos[2]+geometry->vertex[i].pos[2];

		if (IS_TANGENT_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].tangent[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].tangent[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset1++] = geometry->vertex[i].tangent[2];

			darray[(i*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].tangent[0]+geometry->vertex[i].tangent[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].tangent[1]+geometry->vertex[i].tangent[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset2++] = geometry->field_vector[i].tangent[2]+geometry->vertex[i].tangent[2];
		}
    }

	for(i=0; i<geometry->num_vertex; i++) {
		iarray[(i*2)]	= i;	
		iarray[(i*2)+1]	= geometry->num_vertex+i;	
		//printf("index %d = [%f %f %f]\n",i,geometry->vertex[i].pos[0],geometry->vertex[i].pos[1],geometry->vertex[i].pos[2]);
		//printf("		 = [%f %f %f]\n",geometry->field_vector[i].pos[0],geometry->field_vector[i].pos[1],geometry->field_vector[i].pos[2]);
	}

	displayBuffer.Enqueue();
}


/**
 * Calculate the DISPARRAY_NODESIZE for a DisplayArray
 */
int VectorFieldDisplayManager::GetDISPARRAY_NODESIZE(const DisplayHeader * header)
{
	int DISPARRAY_NODESIZE = 3;
	
	char	type = header->dataType;

	if(IS_RGB_ON(type))
		DISPARRAY_NODESIZE += 3;
	else if(IS_RGBA_ON(type))
		DISPARRAY_NODESIZE += 4;

	if (IS_TANGENT_ON(type))
		DISPARRAY_NODESIZE += 3;

	return DISPARRAY_NODESIZE;
}


/**
 * Calculate the dA_size for a DisplayArray
 */
int VectorFieldDisplayManager::GetdA_size(int DISPARRAY_NODESIZE)
{
	return geometry->num_vertex * DISPARRAY_NODESIZE * 2;
}

/**
 * Calculate the iA_size for a DisplayArray
 */
int VectorFieldDisplayManager::GetiA_size(const DisplayHeader * header)
{
	return geometry->num_vertex * 2;
}


/**
 * Write display data to buffer.
 */
void TriSurfaceDisplayManager::Display(void)
{

    unsigned int	i;
	int				offset;
	DisplayArray*	display = displayBuffer.GetWriteArray(visible);
	char			type = display->header.dataType;
	float*			darray = display->dispArray;
    unsigned int*	iarray = display->indexArray;

	geometry->calcNormals();

    for(i=0; i<geometry->num_vertex; i++) {
		offset = 0;

		if(IS_TEXCOORD_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].texcoord[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].texcoord[1];
		}

		if(IS_RGB_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
		} else if(IS_RGBA_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[3];
		}

		if(IS_NORMAL_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[2];
		}

		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[0];
		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[1];
		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[2];

		if (IS_TANGENT_ON(type)) {
			// TODO: for deformable objects, need to add the calculation of tangents
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[2];
		}
    }

    for(i=0; i<geometry->num_face; i++) {
		iarray[(i*3)]	= geometry->face[i].vertex[0]->refid;
		iarray[(i*3)+1] = geometry->face[i].vertex[1]->refid;
		iarray[(i*3)+2] = geometry->face[i].vertex[2]->refid;
    }
	
	displayBuffer.Enqueue();
}

/**
 * Calculate the DISPARRAY_NODESIZE for a DisplayArray.
 *
 * @param header DisplayHeader for this DisplayManager.
 */
int TriSurfaceDisplayManager::GetDISPARRAY_NODESIZE(const DisplayHeader * header)
{
	int DISPARRAY_NODESIZE = 3;
	
	char	type = header->dataType;

	if(IS_RGB_ON(type))
		DISPARRAY_NODESIZE += 3;
	else if(IS_RGBA_ON(type))
		DISPARRAY_NODESIZE += 4;

	if(IS_NORMAL_ON(type))
		DISPARRAY_NODESIZE += 3;

	if(IS_TEXCOORD_ON(type))
		DISPARRAY_NODESIZE += 2;

	if (IS_TANGENT_ON(type))
		DISPARRAY_NODESIZE += 3;

	return DISPARRAY_NODESIZE;
}

/**
 * Calculate the dA_size for a DisplayArray.
 *
 * @param DISPARRAY_NODESIZE The node size of the contained data.
 */
int TriSurfaceDisplayManager::GetdA_size(int DISPARRAY_NODESIZE)
{
	return geometry->num_vertex * DISPARRAY_NODESIZE;
}

/**
 * Calculate the iA_size for a DisplayArray.
 *
 * @param header DisplayHeader for this DisplayManager.
 */
int TriSurfaceDisplayManager::GetiA_size(const DisplayHeader * header)
{
	return geometry->num_face * 3;
}

/**
 * Write display data to buffer.
 */
void TetVolumeDisplayManager::Display(void)
{
    unsigned int	i;
	int				offset;
	DisplayArray*	display = displayBuffer.GetWriteArray(visible);
	char			type = display->header.dataType;
    float*			darray = display->dispArray;
    unsigned int*	iarray = display->indexArray;

    for(i=0; i<geometry->num_vertex; i++) {
		offset = 0;

		if(IS_TEXCOORD_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].texcoord[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].texcoord[1];
		}

		if(IS_RGB_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
		} else if(IS_RGBA_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[2];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].color[3];
		}

		if(IS_NORMAL_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].n[2];
		}

		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[0];
		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[1];
		darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].pos[2];

		if (IS_TANGENT_ON(type)) {
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[0];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[1];
			darray[(i*display->DISPARRAY_NODESIZE)+offset++] = geometry->vertex[i].tangent[2];
		}
    }

	if(display->header.objType == GIPSI_DRAW_TRIANGLE) {
		// if object type is GIPSI_DRAW_TRIANGLE then we will draw the faces
		for(i=0; i<geometry->num_face; i++) {
			iarray[(i*3)]	= geometry->face[i].vertex[0]->refid;
			iarray[(i*3)+1] = geometry->face[i].vertex[1]->refid;
			iarray[(i*3)+2] = geometry->face[i].vertex[2]->refid;
		}
	}
	else if (display->header.objType == GIPSI_DRAW_LINE){
		// if object type is GIPSI_DRAW_LINE then we will draw the tet edges
		for(i=0; i<geometry->num_tet; i++) {
			iarray[(i*6*2)+0]		= geometry->tet[i].vertex[0]->refid;
			iarray[(i*6*2)+1]		= geometry->tet[i].vertex[1]->refid;
			iarray[(i*6*2)+2]		= geometry->tet[i].vertex[0]->refid;
			iarray[(i*6*2)+3]		= geometry->tet[i].vertex[2]->refid;
			iarray[(i*6*2)+4]		= geometry->tet[i].vertex[0]->refid;
			iarray[(i*6*2)+5]		= geometry->tet[i].vertex[3]->refid;
			iarray[(i*6*2)+6]		= geometry->tet[i].vertex[1]->refid;
			iarray[(i*6*2)+7]		= geometry->tet[i].vertex[2]->refid;
			iarray[(i*6*2)+8]		= geometry->tet[i].vertex[1]->refid;
			iarray[(i*6*2)+9]		= geometry->tet[i].vertex[3]->refid;
			iarray[(i*6*2)+10]		= geometry->tet[i].vertex[2]->refid;
			iarray[(i*6*2)+11]		= geometry->tet[i].vertex[3]->refid;
		}
	}
	else {
		// Unknown type
		error_exit(1,"Unknown object type for TetVolume Display Manager");
	}

	displayBuffer.Enqueue();
}

/**
 * Calculate the DISPARRAY_NODESIZE for a DisplayArray
 *
 * @param header DisplayHeader for this DisplayManager.
 */
int TetVolumeDisplayManager::GetDISPARRAY_NODESIZE(const DisplayHeader * header)
{
	int DISPARRAY_NODESIZE = 3;
	
	char	type = header->dataType;

	if(IS_RGB_ON(type))
		DISPARRAY_NODESIZE += 3;
	else if(IS_RGBA_ON(type))
		DISPARRAY_NODESIZE += 4;

	if(IS_NORMAL_ON(type))
		DISPARRAY_NODESIZE += 3;

	if(IS_TEXCOORD_ON(type))
		DISPARRAY_NODESIZE += 2;

	if (IS_TANGENT_ON(type))
		DISPARRAY_NODESIZE += 3;

	return DISPARRAY_NODESIZE;
}

/**
 * Calculate the dA_size for a DisplayArray.
 *
 * @param DISPARRAY_NODESIZE The node size of the contained data.
 */
int TetVolumeDisplayManager::GetdA_size(int DISPARRAY_NODESIZE)
{
	return geometry->num_vertex * DISPARRAY_NODESIZE;
}

/**
 * Calculate the iA_size for a DisplayArray
 *
 * @param header DisplayHeader for this DisplayManager.
 */
int TetVolumeDisplayManager::GetiA_size(const DisplayHeader * header)
{
	if (header->objType == GIPSI_DRAW_TRIANGLE)
		return geometry->num_face * 3;
	else if (header->objType == GIPSI_DRAW_LINE)
		return geometry->num_tet * 12;
	else 
		error_exit(1,"Unknown object type for TetVolume Display Manager");
}


/**
 * The TextureDisplayManager is initialized using a DisplayTextureType and a
 * formatted file name.
 *
 * The formattedFileName is used to load the texture data during initialization.
 */
TextureDisplayManager::TextureDisplayManager(DisplayTextureType textType, char * formattedFileName) :
	DisplayManager(NULL, textType),
	geometry(NULL)
{
	textureFile = formattedFileName;

	if (displayBuffer.GetTextureType() == GIPSI_2D_STATIC_SERVER ||
		displayBuffer.GetTextureType() == GIPSI_2D_DYNAMIC_SERVER)
	{
		geometry = new TextureGeometry();
		geometry->Load(textureFile);
		static_texture_sent=false;
	}
}


/**
 * Writes texture information to a DisplayArray.
 */
void TextureDisplayManager::Display()
{
	switch (displayBuffer.GetTextureType())
	{
	case GIPSI_2D_STATIC_CLIENT:
	case GIPSI_3D_STATIC_CLIENT:
		{
			// fill in the texture name
			DisplayArray * display = displayBuffer.GetWriteArray(visible);
			int size = strlen(textureFile);
			for (int i = 0; i < size; i++)
			{
				display->indexArray[i] = (int)textureFile[i];
			}
			displayBuffer.Enqueue();
			break;
		}
	case GIPSI_2D_STATIC_SERVER:
		{
			if (!static_texture_sent) {
				// fill in the data
				DisplayArray * display = displayBuffer.GetWriteArray(visible);
				int size = geometry->height * geometry->width;
				for (int i = 0; i < size; i++)
				{
					display->indexArray[i] =( (geometry->data[4*i]) | (geometry->data[4*i+1] << 8) | (geometry->data[4*i+2] << 16) | (geometry->data[4*i+3] << 24) );
				}
				displayBuffer.Enqueue();
				static_texture_sent=true;
			}
			break;
		}
	case GIPSI_2D_DYNAMIC_SERVER:
		{
			// fill in the data
			DisplayArray * display = displayBuffer.GetWriteArray(visible);
			int size = geometry->height * geometry->width;
			for (int i = 0; i < size; i++)
			{
				display->indexArray[i] =( (geometry->data[4*i]) | (geometry->data[4*i+1] << 8) | (geometry->data[4*i+2] << 16) | (geometry->data[4*i+3] << 24) );
			}
			displayBuffer.Enqueue();
			break;
		}
	case GIPSI_NO_TEXTURE:
	default:
		// do nothing, i suppose.  this shouldn't happen.
		return;
	}
}


/**
 * Calculate the iA_size for a DisplayArray
 *
 * @param header DisplayHeader for this DisplayManager.
 */
int TextureDisplayManager::GetDISPARRAY_NODESIZE(const DisplayHeader *header)
{
	if (geometry)
		return geometry->width;
	else
		return 0;
}


/**
 * Calculate the dA_size for a DisplayArray.
 *
 * @param DISPARRAY_NODESIZE The node size of the contained data.
 */
int TextureDisplayManager::GetdA_size(int DISPARRAY_NODESIZE)
{
	return 0;
}


/**
 * Calculate the iA_size for a DisplayArray
 *
 * @param header DisplayHeader for this DisplayManager.
 */
int TextureDisplayManager::GetiA_size(const DisplayHeader *header)
{
	if (displayBuffer.GetTextureType() == GIPSI_2D_STATIC_SERVER  ||
		displayBuffer.GetTextureType() == GIPSI_2D_DYNAMIC_SERVER)
	{
		return geometry->width * geometry->height;
	}
	else if (	displayBuffer.GetTextureType() == GIPSI_2D_STATIC_CLIENT ||
				displayBuffer.GetTextureType() == GIPSI_3D_STATIC_CLIENT)
	{
		return strlen(textureFile);
	}
	else// if (displayBuffer.GetTextureType() == GIPSI_NO_TEXTURE)
		return 0;
}
