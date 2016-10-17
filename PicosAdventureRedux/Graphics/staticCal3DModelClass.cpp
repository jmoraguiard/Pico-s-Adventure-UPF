#include "staticCal3DModelClass.h"

StaticCal3DModelClass::StaticCal3DModelClass():Cal3DModelClass()
{
}

StaticCal3DModelClass::StaticCal3DModelClass(const StaticCal3DModelClass& other)
{
}

StaticCal3DModelClass::~StaticCal3DModelClass()
{
}

bool StaticCal3DModelClass::setup(ID3D11Device* device, std::string modelName)
{
	bool result;

	modelName_ = modelName;

	cal3dCoreModel_ = new CalCoreModel(modelName);
	if(!cal3dCoreModel_)
	{
		return false;
	}

	if(!parseModelConfiguration(modelName))
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = setupBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

void StaticCal3DModelClass::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	drawBuffers(deviceContext);

	return;
}

bool StaticCal3DModelClass::setupBuffers(ID3D11Device* device)
{
	TexturedVertexType *vertices = 0, *verticesSubmesh = 0;
	CalIndex *indices = 0, *indicesSubmesh = 0;
	int vertexCountSubmesh, indexCountSubmesh;
	D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
	D3D11_SUBRESOURCE_DATA vertex_data, index_data;
	HRESULT result;

	//Release the old buffers to create the new ones
	if(vertexBuffer_)
		vertexBuffer_->Release();

	if(indexBuffer_)
		indexBuffer_->Release();

	vertexCount_ = 0;
	indexCount_ = 0;

	//Start loading information
	CalRenderer *cal3dRenderer = cal3dModel_->getRenderer();

	// begin the rendering loop
	if(!cal3dRenderer->beginRendering()){
		int errorCode = CalError::getLastErrorCode();
		std::string errorString = CalError::getLastErrorDescription();
		std::string errorFile = CalError::getLastErrorFile();
		int errorLine = CalError::getLastErrorLine();
		std::stringstream errorStream;
		errorStream << "Error number " << errorCode << ": " << errorString << ". File: " << errorFile << " line " << errorLine;
		MessageBoxA(NULL, errorStream.str().c_str(), "StaticModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	int meshCount = cal3dRenderer->getMeshCount();

	// render all meshes of the model
	for(int meshId = 0; meshId < meshCount; meshId++)
	{
		// get the number of submeshes
		int submeshCount = cal3dRenderer->getSubmeshCount(meshId);

		// render all submeshes of the mesh
		for(int submeshId = 0; submeshId < submeshCount; submeshId++)
		{
			// select mesh and submesh for further data access
			if(cal3dRenderer->selectMeshSubmesh(meshId, submeshId))
			{
				verticesSubmesh = new TexturedVertexType[cal3dRenderer->getVertexCount()];
				if(!verticesSubmesh)
				{
					MessageBox(NULL, L"No vertices in submesh.", L"StaticModel - Error", MB_ICONERROR | MB_OK);
					return false;
				}
				vertexCountSubmesh = cal3dRenderer->getVerticesNormalsAndTexCoords(&verticesSubmesh->position.x);

				// Create the index array.
				indicesSubmesh = new CalIndex[cal3dRenderer->getFaceCount()*3];
				if(!indicesSubmesh)
				{
					MessageBox(NULL, L"No faces in submesh.", L"StaticModel - Error", MB_ICONERROR | MB_OK);
					return false;
				}
				indexCountSubmesh = cal3dRenderer->getFaces(indicesSubmesh)*3;

				//Save in temp arrays the already stored data
				TexturedVertexType *verticesTemp = new TexturedVertexType[vertexCount_];
				std::memcpy(verticesTemp, vertices, sizeof(TexturedVertexType)*vertexCount_);
				delete [] vertices;

				CalIndex *indicesTemp = new CalIndex[indexCount_];
				std::memcpy(indicesTemp, indices, sizeof(CalIndex)*indexCount_);
				delete [] indices;

				//We update the total amount of vertices and indices
				vertexCount_ += vertexCountSubmesh;
				indexCount_ += indexCountSubmesh;

				//Update the new arrays for vertices and indices
				vertices = new TexturedVertexType[vertexCount_];
				indices = new CalIndex[indexCount_];

				//We copy in thew arrays the old and the new data
				memcpy(vertices, verticesTemp, sizeof(TexturedVertexType)*(vertexCount_-vertexCountSubmesh));
				for(int i = vertexCount_-vertexCountSubmesh; i < vertexCount_; i++)
				{
					vertices[i] = verticesSubmesh[i-(vertexCount_-vertexCountSubmesh)];
				}
				memcpy(indices, indicesTemp, sizeof(CalIndex)*(indexCount_-indexCountSubmesh));
				for(int i = indexCount_-indexCountSubmesh; i < indexCount_; i++)
				{
					indices[i] = indicesSubmesh[i-(indexCount_-indexCountSubmesh)]+(vertexCount_-vertexCountSubmesh);
				}

				//Delete all useless arrays to free memory
				delete [] verticesTemp;
				delete [] indicesTemp;

				delete [] verticesSubmesh;
				delete [] indicesSubmesh;
			}
		}
	}

	// end the rendering
	cal3dRenderer->endRendering();

	// Set up the description of the static vertex buffer.
	vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	vertex_buffer_desc.ByteWidth = sizeof(TexturedVertexType) * vertexCount_;
	vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertex_buffer_desc.CPUAccessFlags = 0;
	vertex_buffer_desc.MiscFlags = 0;
	vertex_buffer_desc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertex_data.pSysMem = vertices;
	vertex_data.SysMemPitch = 0;
	vertex_data.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertex_buffer_desc, &vertex_data, &vertexBuffer_);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed creating vertex buffer.", L"StaticModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Set up the description of the static index buffer.
	index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	index_buffer_desc.ByteWidth = sizeof(CalIndex) * indexCount_;
	index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	index_buffer_desc.CPUAccessFlags = 0;
	index_buffer_desc.MiscFlags = 0;
	index_buffer_desc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	index_data.pSysMem = indices;
	index_data.SysMemPitch = 0;
	index_data.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&index_buffer_desc, &index_data, &indexBuffer_);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed creating index buffer.", L"StaticModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] indices;
	indices = 0;
	
	delete [] vertices;
	vertices = 0;

	return true;
}


bool StaticCal3DModelClass::parseModelConfiguration(std::string strFilename)
{
	//We create the model Loader
	CalLoader::setLoadingMode( LOADER_ROTATE_X_AXIS );
	//We set the basic root for getting the models
	std::string root = "./Data/models/" + strFilename + "/";

	//Loading the skeleton
	bool is_ok = cal3dCoreModel_->loadCoreSkeleton( root + strFilename + ".csf" );
	if(!is_ok)
	{
		MessageBoxA(NULL, "Could not load the skeleton!", "ModelClass - Error", MB_ICONERROR | MB_OK);
		int errorCode = CalError::getLastErrorCode();
		std::string errorString = CalError::getLastErrorDescription();
		std::string errorFile = CalError::getLastErrorFile();
		int errorLine = CalError::getLastErrorLine();
		std::stringstream errorStream;
		errorStream << "Error number " << errorCode << ": " << errorString << ". File: " << errorFile << " line " << errorLine;
		MessageBoxA(NULL, errorStream.str().c_str(), "ModelClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	//Loading the mesh
	modelMeshID = cal3dCoreModel_->loadCoreMesh( root + strFilename + ".cmf" );
	if(modelMeshID < 0)
	{
		MessageBoxA(NULL, "Could not load the mesh!", "ModelClass - Error", MB_ICONERROR | MB_OK);
		int errorCode = CalError::getLastErrorCode();
		std::string errorString = CalError::getLastErrorDescription();
		std::string errorFile = CalError::getLastErrorFile();
		int errorLine = CalError::getLastErrorLine();
		std::stringstream errorStream;
		errorStream << "Error number " << errorCode << ": " << errorString << ". File: " << errorFile << " line " << errorLine;
		MessageBoxA(NULL, errorStream.str().c_str(), "ModelClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	//Set up the mesh
	cal3dModel_ = new CalModel(cal3dCoreModel_);
	if(!cal3dModel_->attachMesh(modelMeshID))
	{
		MessageBox(NULL, L"Could not attach a Mesh to the model", L"ModelClass - Error", MB_ICONERROR | MB_OK);
		int errorCode = CalError::getLastErrorCode();
		std::string errorString = CalError::getLastErrorDescription();
		std::string errorFile = CalError::getLastErrorFile();
		int errorLine = CalError::getLastErrorLine();
		std::stringstream errorStream;
		errorStream << "Error number " << errorCode << ": " << errorString << ". File: " << errorFile << " line " << errorLine;
		MessageBoxA(NULL, errorStream.str().c_str(), "ModelClass - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	cal3dModel_->update(0.0f);

	return true;
}
