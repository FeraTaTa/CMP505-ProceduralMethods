#pragma once

class TerrainClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
	    XMFLOAT4 color;
	};

	struct HeightMapType 
	{ 
		float x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, const char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	bool LoadHeightMap(const char*);
	void NormalizeHeightMap();
	void ShutdownHeightMap();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	
private:
	int m_terrainWidth = 0;
	int m_terrainHeight = 0;
	int m_vertexCount = 0;
	int m_indexCount = 0;
	ID3D11Buffer* m_vertexBuffer = nullptr;
	ID3D11Buffer* m_indexBuffer = nullptr;
	HeightMapType* m_heightMap = nullptr;
};