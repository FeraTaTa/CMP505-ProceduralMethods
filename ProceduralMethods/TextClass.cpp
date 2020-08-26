#include "stdafx.h"
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"


TextClass::TextClass()
{
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	// Store the screen width and height for calculating pixel location during the sentence updates.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix for 2D text rendering.
	m_baseViewMatrix = baseViewMatrix;

	// Create the font object.
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// Initialize the font object.
	bool result = m_Font->Initialize(device, "../ProceduralMethods/data/fontdata.txt", L"../Dx11Terrain_02/data/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence1, 150, device);
	if(!result)
	{
		return false;
	}

	// Initialize the second sentence.
	result = InitializeSentence(&m_sentence2, 32, device);
	if(!result)
	{
		return false;
	}

	// Initialize the third sentence.
	result = InitializeSentence(&m_sentence3, 16, device);
	if(!result)
	{
		return false;
	}

	// Initialize the fourth sentence.
	result = InitializeSentence(&m_sentence4, 16, device);
	if(!result)
	{
		return false;
	}

	// Initialize the fifth sentence.
	result = InitializeSentence(&m_sentence5, 16, device);
	if(!result)
	{
		return false;
	}

	// Initialize the sixth sentence.
	result = InitializeSentence(&m_sentence6, 16, device);
	if(!result)
	{
		return false;
	}

	// Initialize the seventh sentence.
	result = InitializeSentence(&m_sentence7, 16, device);
	if(!result)
	{
		return false;
	}

	// Initialize the eighth sentence.
	result = InitializeSentence(&m_sentence8, 16, device);
	if(!result)
	{
		return false;
	}

	// Initialize the ninth sentence.
	result = InitializeSentence(&m_sentence9, 16, device);
	if(!result)
	{
		return false;
	}

	// Initialize the tenth sentence.
	result = InitializeSentence(&m_sentence10, 16, device);
	if(!result)
	{
		return false;
	}

	return true;
}


void TextClass::Shutdown()
{
	// Release the font object.
	if(m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}

	// Release the sentences.
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);
	ReleaseSentence(&m_sentence3);
	ReleaseSentence(&m_sentence4);
	ReleaseSentence(&m_sentence5);
	ReleaseSentence(&m_sentence6);
	ReleaseSentence(&m_sentence7);
	ReleaseSentence(&m_sentence8);
	ReleaseSentence(&m_sentence9);
	ReleaseSentence(&m_sentence10);
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, FontShaderClass* FontShader, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// Draw the sentences.
	bool result = RenderSentence(m_sentence1, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence2, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence3, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence4, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence5, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence6, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence7, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence8, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence9, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	result = RenderSentence(m_sentence10, deviceContext, FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	return true;
}


bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	// Create a new sentence object.
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for (int i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	// Store the color of the sentence.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// Get the number of letters in the sentence and check for possible buffer overflow.
	if ((int)strlen(text) > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	VertexType* vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	float drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	float drawY = (float)((m_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// Release the sentence vertex buffer.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *sentence;
		*sentence = 0;
	}
}


bool TextClass::RenderSentence(SentenceType* sentence, ID3D11DeviceContext* deviceContext, FontShaderClass* FontShader, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	return FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor);
}


bool TextClass::SetVideoCardInfo(const char* videoCardName, int videoCardMemory, ID3D11DeviceContext* deviceContext)
{
	char dataString[150] = { 0, };
	char tempString[16] = { 0, };
	char memoryString[32] = { 0, };

	// Setup the video card name string.
	strcpy_s(dataString, "Video Card: ");
	strcat_s(dataString, videoCardName);

	// Update the sentence vertex buffer with the new string information.
	if(!UpdateSentence(m_sentence1, dataString, 10, 10, 1.0f, 1.0f, 1.0f, deviceContext))
	{
		return false;
	}

	// Truncate the memory value to prevent buffer over flow.
	if(videoCardMemory > 9999999)
	{
		videoCardMemory = 9999999;
	}

	// Convert the video memory integer value to a string format.
	_itoa_s(videoCardMemory, tempString, 10);

	// Setup the video memory string.
	strcpy_s(memoryString, "Video Memory: ");
	strcat_s(memoryString, tempString);
	strcat_s(memoryString, " MB");

	// Update the sentence vertex buffer with the new string information.
	if(!UpdateSentence(m_sentence2, memoryString, 10, 30, 1.0f, 1.0f, 1.0f, deviceContext))
	{
		return false;
	}

	return true;
}


bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16] = { 0, };
	char fpsString[16] = { 0, };

	// Truncate the fps to prevent a buffer over flow.
	if(fps > 9999)
	{
		fps = 9999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// Update the sentence vertex buffer with the new string information.
	if(!UpdateSentence(m_sentence3, fpsString, 10, 70, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}


bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16] = { 0, };
	char cpuString[16] = { 0, };

	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// Update the sentence vertex buffer with the new string information.
	if(!UpdateSentence(m_sentence4, cpuString, 10, 90, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}


bool TextClass::SetCameraPosition(XMFLOAT3 pos, ID3D11DeviceContext* deviceContext)
{
	char tempString[16] = { 0, };
	char dataString[16] = { 0, };

	// Convert the position from floating point to integer.
	int positionX = (int)pos.x;
	int positionY = (int)pos.y;
	int positionZ = (int)pos.z;

	// Truncate the position if it exceeds either 9999 or -9999.
	if(positionX > 9999) { positionX = 9999; }
	if(positionY > 9999) { positionY = 9999; }
	if(positionZ > 9999) { positionZ = 9999; }

	if(positionX < -9999) { positionX = -9999; }
	if(positionY < -9999) { positionY = -9999; }
	if(positionZ < -9999) { positionZ = -9999; }

	// Setup the X position string.
	_itoa_s(positionX, tempString, 10);
	strcpy_s(dataString, "X: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence5, dataString, 10, 130, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}
	
	// Setup the Y position string.
	_itoa_s(positionY, tempString, 10);
	strcpy_s(dataString, "Y: ");
	strcat_s(dataString, tempString);
	
	if(!UpdateSentence(m_sentence6, dataString, 10, 150, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Setup the Z position string.
	_itoa_s(positionZ, tempString, 10);
	strcpy_s(dataString, "Z: ");
	strcat_s(dataString, tempString);
	
	if(!UpdateSentence(m_sentence7, dataString, 10, 170, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}


bool TextClass::SetCameraRotation(XMFLOAT3 rot, ID3D11DeviceContext* deviceContext)
{
	char tempString[16] = { 0, };
	char dataString[16] = { 0, };

	// Convert the rotation from floating point to integer.
	int rotationX = (int)rot.x;
	int rotationY = (int)rot.y;
	int rotationZ = (int)rot.z;

	// Setup the X rotation string.
	_itoa_s(rotationX, tempString, 10);
	strcpy_s(dataString, "rX: ");
	strcat_s(dataString, tempString);
	
	if(!UpdateSentence(m_sentence8, dataString, 10, 210, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Setup the Y rotation string.
	_itoa_s(rotationY, tempString, 10);
	strcpy_s(dataString, "rY: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence9, dataString, 10, 230, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	// Setup the Z rotation string.
	_itoa_s(rotationZ, tempString, 10);
	strcpy_s(dataString, "rZ: ");
	strcat_s(dataString, tempString);

	if(!UpdateSentence(m_sentence10, dataString, 10, 250, 0.0f, 1.0f, 0.0f, deviceContext))
	{
		return false;
	}

	return true;
}