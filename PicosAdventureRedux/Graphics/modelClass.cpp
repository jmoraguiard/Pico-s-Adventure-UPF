#include "modelClass.h"

ModelClass::ModelClass()
{
	vertexBuffer_ = 0;
	indexBuffer_ = 0;
	vertexCount_ = 0;
	indexCount_ = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

int ModelClass::getIndexCount()
{
	return indexCount_;
}

std::string ModelClass::getModelName()
{
	return modelName_;
}

void ModelClass::drawBuffers(ID3D11DeviceContext* device_context)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(TexturedVertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device_context->IASetVertexBuffers(0, 1, &vertexBuffer_, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	device_context->IASetIndexBuffer(indexBuffer_, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void ModelClass::destroyBuffers()
{
	// Release the index buffer.
	if(indexBuffer_)
	{
		indexBuffer_->Release();
		indexBuffer_ = 0;
	}

	// Release the vertex buffer.
	if(vertexBuffer_)
	{
		vertexBuffer_->Release();
		vertexBuffer_ = 0;
	}

	return;
}
