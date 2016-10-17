#include "OBJModelClass.h"

OBJModelClass::OBJModelClass()
{
	vertexBuffer_ = 0;
	indexBuffer_ = 0;
}

OBJModelClass::OBJModelClass(const OBJModelClass& other)
{
}

OBJModelClass::~OBJModelClass()
{
}

bool OBJModelClass::setup(ID3D11Device* device, std::string modelName)
{
	std::ifstream fin;
	char input;
	bool result;

	modelName_ = modelName;

	std::string root = "./Data/models/" + modelName + "/" + modelName + ".obj";

	// Initialize the counts.
	objModel_.vertexCount = 0;
	objModel_.textureCount = 0;
	objModel_.normalCount = 0;
	objModel_.faceCount = 0;

	// Open the file.
	fin.open(root);

	// Check if it was successful in opening the file.
	if(fin.fail() == true)
	{
		return false;
	}

	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while(!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if(input == 'v')
		{
			fin.get(input);
			if(input == ' ') { objModel_.vertexCount++; }
			if(input == 't') { objModel_.textureCount++; }
			if(input == 'n') { objModel_.normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if(input == 'f')
		{
			fin.get(input);
			if(input == ' ') { objModel_.faceCount++; }
		}
		
		// Otherwise read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	result = parseModelConfiguration(root);
	if(!result)
	{
		MessageBox(NULL, L"Could not parse configuration file.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = setupBuffers(device);
	if(!result)
	{
		return false;
	}

	// Close the file.
	fin.close();

	return true;
}

void OBJModelClass::draw(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	drawBuffers(deviceContext);
}

void OBJModelClass::destroy()
{
	// Release the vertex and index buffers.
	destroyBuffers();

	// Release the model
	// Release the four data structures.
	if(objModel_.vertices)
	{
		delete [] objModel_.vertices;
		objModel_.vertices = 0;
	}
	if(objModel_.texcoords)
	{
		delete [] objModel_.texcoords;
		objModel_.texcoords = 0;
	}
	if(objModel_.normals)
	{
		delete [] objModel_.normals;
		objModel_.normals = 0;
	}
	if(objModel_.faces)
	{
		delete [] objModel_.faces;
		objModel_.faces = 0;
	}

	return;
}

bool OBJModelClass::setupBuffers(ID3D11Device* device)
{
	TexturedVertexType *vertices = 0;
	unsigned int *indices = 0;
	D3D11_BUFFER_DESC vertex_buffer_desc, index_buffer_desc;
	D3D11_SUBRESOURCE_DATA vertex_data, index_data;
	HRESULT result;

	//Release the old buffers to create the new ones
	if(vertexBuffer_)
		vertexBuffer_->Release();

	if(indexBuffer_)
		indexBuffer_->Release();

	vertexCount_ = objModel_.faceCount*3;
	indexCount_ = objModel_.faceCount*3;

	vertices = new TexturedVertexType[vertexCount_];
	indices = new unsigned int[indexCount_];

	for(unsigned int i = 0; i < objModel_.faceCount; i++)
	{
		FaceType face = objModel_.faces[i];

		vertices[i*3].position = XMFLOAT3(objModel_.vertices[face.vIndex1-1].x, objModel_.vertices[face.vIndex1-1].y, objModel_.vertices[face.vIndex1-1].z);
		vertices[i*3].normal = XMFLOAT3(objModel_.normals[face.nIndex1-1].x, objModel_.normals[face.nIndex1-1].y, objModel_.normals[face.nIndex1-1].z);
		vertices[i*3].tu = objModel_.texcoords[face.tIndex1-1].x;
		vertices[i*3].tv = objModel_.texcoords[face.tIndex1-1].y;
		indices[i*3] = i*3;

		vertices[i*3+1].position = XMFLOAT3(objModel_.vertices[face.vIndex2-1].x, objModel_.vertices[face.vIndex2-1].y, objModel_.vertices[face.vIndex2-1].z);
		vertices[i*3+1].normal = XMFLOAT3(objModel_.normals[face.nIndex2-1].x, objModel_.normals[face.nIndex2-1].y, objModel_.normals[face.nIndex2-1].z);
		vertices[i*3+1].tu = objModel_.texcoords[face.tIndex2-1].x;
		vertices[i*3+1].tv = objModel_.texcoords[face.tIndex2-1].y;
		indices[i*3+1] = i*3+1;

		vertices[i*3+2].position = XMFLOAT3(objModel_.vertices[face.vIndex3-1].x, objModel_.vertices[face.vIndex3-1].y, objModel_.vertices[face.vIndex3-1].z);
		vertices[i*3+2].normal = XMFLOAT3(objModel_.normals[face.nIndex3-1].x, objModel_.normals[face.nIndex3-1].y, objModel_.normals[face.nIndex3-1].z);
		vertices[i*3+2].tu = objModel_.texcoords[face.tIndex3-1].x;
		vertices[i*3+2].tv = objModel_.texcoords[face.tIndex3-1].y;
		indices[i*3+2] = i*3+2;
	}

	// Set up the description of the static vertex buffer.
	vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
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
		MessageBox(NULL, L"Failed creating vertex buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Set up the description of the static index buffer.
	index_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	index_buffer_desc.ByteWidth = sizeof(unsigned int) * indexCount_;
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
		MessageBox(NULL, L"Failed creating index buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	return true;
}

bool OBJModelClass::parseModelConfiguration(std::string modelName)
{
	std::ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex;
	char input, input2;

	// Initialize the four data structures.
	objModel_.vertices = new VertexType[objModel_.vertexCount];
	if(!objModel_.vertices)
	{
		MessageBox(NULL, L"Failed creating vertices model buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	objModel_.texcoords = new VertexType[objModel_.textureCount];
	if(!objModel_.texcoords)
	{
		MessageBox(NULL, L"Failed creating texcoords model buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	objModel_.normals = new VertexType[objModel_.normalCount];
	if(!objModel_.normals)
	{
		MessageBox(NULL, L"Failed creating normals model buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	objModel_.faces = new FaceType[objModel_.faceCount];
	if(!objModel_.faces)
	{
		MessageBox(NULL, L"Failed creating faces model buffer.", L"OBJModel - Error", MB_ICONERROR | MB_OK);
		return false;
	}

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(modelName);

	// Check if it was successful in opening the file.
	if(fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while(!fin.eof())
	{
		if(input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if(input == ' ') 
			{ 
				fin >> objModel_.vertices[vertexIndex].x >> objModel_.vertices[vertexIndex].y >> objModel_.vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				objModel_.vertices[vertexIndex].z = objModel_.vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			// Read in the texture uv coordinates.
			if(input == 't') 
			{ 
				fin >> objModel_.texcoords[texcoordIndex].x >> objModel_.texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				objModel_.texcoords[texcoordIndex].y = 1.0f - objModel_.texcoords[texcoordIndex].y;
				texcoordIndex++; 
			}

			// Read in the normals.
			if(input == 'n') 
			{ 
				fin >> objModel_.normals[normalIndex].x >> objModel_.normals[normalIndex].y >> objModel_.normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				objModel_.normals[normalIndex].z = objModel_.normals[normalIndex].z * -1.0f;
				normalIndex++; 
			}
		}

		// Read in the faces.
		if(input == 'f') 
		{
			fin.get(input);
			if(input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> objModel_.faces[faceIndex].vIndex3 >> input2 >> objModel_.faces[faceIndex].tIndex3 >> input2 >> objModel_.faces[faceIndex].nIndex3
				    >> objModel_.faces[faceIndex].vIndex2 >> input2 >> objModel_.faces[faceIndex].tIndex2 >> input2 >> objModel_.faces[faceIndex].nIndex2
				    >> objModel_.faces[faceIndex].vIndex1 >> input2 >> objModel_.faces[faceIndex].tIndex1 >> input2 >> objModel_.faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while(input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	return true;
}
