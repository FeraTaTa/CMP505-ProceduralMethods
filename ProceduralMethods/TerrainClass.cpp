#include "stdafx.h"
#include "terrainclass.h"
#include <stdio.h>


TerrainClass::TerrainClass()
{
}


TerrainClass::TerrainClass(const TerrainClass& other)
{
}


TerrainClass::~TerrainClass()
{
}


bool TerrainClass::Initialize(ID3D11Device* device, const char* heightMapFilename)
{
	// Load in the height map for the terrain.
	if(!LoadHeightMap(heightMapFilename))
	{
		return false;
	}

	// Normalize the height of the height map.
	NormalizeHeightMap();

	// Calculate the normal of the terrain data.
	if(!CalculateNormals())
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	return InitializeBuffers(device);
}


void TerrainClass::Shutdown()
{
	// Release the vertex and index buffer.
	ShutdownBuffers();

	// Release the height map data.
	ShutdownHeightMap();
}


void TerrainClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);
}


int TerrainClass::GetIndexCount()
{
	return m_indexCount;
}


bool TerrainClass::LoadHeightMap(const char* filename)
{
	// Open the height map file in binary.
	FILE* filePtr = nullptr;
	if(fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	// Read in the file header.
	BITMAPFILEHEADER bitmapFileHeader;
	if(fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr) != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	BITMAPINFOHEADER bitmapInfoHeader;
	if(fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr) != 1)
	{
		return false;
	}

	// Save the dimensions of the terrain.
	m_terrainWidth = bitmapInfoHeader.biWidth;
	m_terrainHeight = bitmapInfoHeader.biHeight;

	// Calculate the size of the bitmap image data.
	int imageSize = m_terrainWidth * m_terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	unsigned char* bitmapImage = new unsigned char[imageSize];
	if(!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	if(fread(bitmapImage, 1, imageSize, filePtr) != imageSize)
	{
		return false;
	}

	// Close the file.
	if(fclose(filePtr) != 0)
	{
		return false;
	}

	// Create the structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if(!m_heightMap)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	int k = 0;

	// Read the image data into the height map.
	for(int j = 0; j < m_terrainHeight; j++)
	{
		for(int i = 0; i < m_terrainWidth; i++)
		{
			unsigned char height = bitmapImage[k];

			int index = (m_terrainHeight * j) + i;

			m_heightMap[index].x = (float)i;
			m_heightMap[index].y = (float)height;
			m_heightMap[index].z = (float)j;

			k += 3;
		}
	}

	// Release the bitmap image data.
	delete[] bitmapImage;
	bitmapImage = 0;

	return true;
}


void TerrainClass::NormalizeHeightMap()
{
	for(int j = 0; j < m_terrainHeight; j++)
	{
		for(int i = 0; i < m_terrainWidth; i++)
		{
			m_heightMap[(m_terrainHeight * j) + i].y /= 15.0f;
		}
	}
}


bool TerrainClass::CalculateNormals()
{
	int index1 = 0;
	int index2 = 0;
	int index3 = 0;
	int index = 0;
	int count = 0;
	float vertex1[3] = { 0.f, 0.f, 0.f };
	float vertex2[3] = { 0.f, 0.f, 0.f };
	float vertex3[3] = { 0.f, 0.f, 0.f };
	float vector1[3] = { 0.f, 0.f, 0.f };
	float vector2[3] = { 0.f, 0.f, 0.f };
	float sum[3] = { 0.f, 0.f, 0.f };
	double length = 0.0f;


	// Create a temporary array to hold the unnormalized normal vector.
	VectorType* normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if(!normals)
	{
		return false;
	}

	// Go through all sides of the mesh and calculate the normal.
	for(int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for(int i = 0; i < (m_terrainWidth - 1); i++)
		{
			index1 = (j * m_terrainHeight) + i;
			index2 = (j * m_terrainHeight) + (i + 1);
			index3 = ((j + 1) * m_terrainHeight) + i;

			// Get the three vertices from the surface.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Compute the two vectors of the surface.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainHeight - 1)) + i;

			// Compute the cross product of the two vectors to get the unnormalized values ​​for these two normals.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take the average of each side.     
	// A vertex touches and gets the average average value for that vertex.
	for(int j = 0; j < m_terrainHeight; j++)
	{
		for(int i = 0; i < m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left side.
			if(((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right side.
			if((i < (m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Top left side.
			if(((i - 1) >= 0) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Top right side.
			if((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainHeight - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get the index of the vertex position in the height map array.
			index = (j * m_terrainHeight) + i;

			// Normalize the final shared normal of this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normal.
	delete[] normals;
	normals = 0;

	return true;
}


void TerrainClass::ShutdownHeightMap()
{
	if(m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}
}


bool TerrainClass::InitializeBuffers(ID3D11Device* device)
{
	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	unsigned long* indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	int index = 0;

	// Load the vertex and index array with the terrain data.
	for(int j = 0; j < (m_terrainHeight - 1); j++)
	{
		for(int i = 0; i < (m_terrainWidth - 1); i++)
		{
			int index1 = (m_terrainHeight * j) + i;          // Bottom left.
			int index2 = (m_terrainHeight * j) + (i + 1);      // Bottom right.
			int index3 = (m_terrainHeight * (j + 1)) + i;      // Upper left.
			int index4 = (m_terrainHeight * (j + 1)) + (i + 1);  // Upper right.

			// Upper left.
			vertices[index].position = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indices[index] = index;
			index++;

			// Upper right.
			vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Upper right.
			vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom right.
			vertices[index].position = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			vertices[index].normal = XMFLOAT3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the static vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	if(FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
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
	if(FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}


void TerrainClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}
}


void TerrainClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered in this vertex buffer. In this case, it is a triangle.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
