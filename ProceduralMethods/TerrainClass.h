#pragma  once

#include "NoiseClass.h"

/////////////
// GLOBALS //
/////////////
const  int  TEXTURE_REPEAT = 8;

class  TextureClass;


class  TerrainClass
{
private:
	struct  VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct  HeightMapType
	{
		float  x, y, z;
		double nx, ny, nz;
		float  tu, tv;
	};

	struct  VectorType
	{
		float  x, y, z;
	};

public:
	TerrainClass();
	TerrainClass(const  TerrainClass &);
	~TerrainClass();

	bool  Initialize(ID3D11Device *, const  char *, const  WCHAR *);
	void  Shutdown();

	ID3D11ShaderResourceView *  GetTexture();

	int  GetVertexCount();
	void  CopyVertexArray(void *);

private:
	bool  LoadHeightMap(const  char *);
	bool  GenerateHeightMap();
	void  NormalizeHeightMap();
	bool  CalculateNormals();
	void  ShutdownHeightMap();

	void  CalculateTextureCoordinates();
	bool  LoadTexture(ID3D11Device *, const  WCHAR *);
	void  ReleaseTexture();

	bool  InitializeBuffers(ID3D11Device *);
	void  ShutdownBuffers();

private:
	NoiseClass noise;
	int  m_terrainWidth = 0;
	int  m_terrainHeight = 0;
	HeightMapType *  m_heightMap = nullptr;
	TextureClass *  m_Texture = nullptr;
	int  m_vertexCount = 0;
	VertexType *  m_vertices = nullptr;
};
