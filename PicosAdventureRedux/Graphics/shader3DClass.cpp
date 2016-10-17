#include "shader3DClass.h"

Shader3DClass::Shader3DClass()
{
	vertexShader_ = 0;
	pixelShader_ = 0;
	layout_ = 0;
	sampleState_ = 0;
	matrixBuffer_ = 0;
	lightBuffer_ = 0;
}


Shader3DClass::Shader3DClass(const Shader3DClass& other)
{
}


Shader3DClass::~Shader3DClass()
{
}

bool Shader3DClass::draw(ID3D11DeviceContext* deviceContext, int indexCount, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, 
						 XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView** textureArray, LightClass* light)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = setShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, light);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	drawShader(deviceContext, indexCount);

	return true;
}

void Shader3DClass::destroy()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	destroyShader();

	return;
}

void Shader3DClass::drawShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout_);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader_, NULL, 0);
	deviceContext->PSSetShader(pixelShader_, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState_);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void Shader3DClass::destroyShader()
{
	// Release the sampler state.
	if(sampleState_)
	{
		sampleState_->Release();
		sampleState_ = 0;
	}

	// Release the matrix constant buffer.
	if(matrixBuffer_)
	{
		matrixBuffer_->Release();
		matrixBuffer_ = 0;
	}

	// Release the light constant buffer.
	if(lightBuffer_)
	{
		lightBuffer_->Release();
		lightBuffer_ = 0;
	}

	// Release the layout.
	if(layout_)
	{
		layout_->Release();
		layout_ = 0;
	}

	// Release the pixel shader.
	if(pixelShader_)
	{
		pixelShader_->Release();
		pixelShader_ = 0;
	}

	// Release the vertex shader.
	if(vertexShader_)
	{
		vertexShader_->Release();
		vertexShader_ = 0;
	}

	return;
}

void Shader3DClass::outputShaderErrorMessage(ID3D10Blob* errorMessage, WCHAR* shaderFilename)
{
	char* compile_errors;
	unsigned long buffer_size, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compile_errors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	buffer_size = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<buffer_size; i++)
	{
		fout << compile_errors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(NULL, L"Error compiling shader.  Check shader-error.txt for message.", L"Shader 3D - Error", MB_ICONERROR | MB_OK);

	return;
}
