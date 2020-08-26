#pragma once

class FontClass;
class FontShaderClass;

class TextClass : public AlignedAllocationPolicy<16>
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, FontShaderClass*, XMMATRIX, XMMATRIX);

	bool SetVideoCardInfo(const char*, int, ID3D11DeviceContext*);
	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);
	bool SetCameraPosition(XMFLOAT3, ID3D11DeviceContext*);
	bool SetCameraRotation(XMFLOAT3, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, const char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(SentenceType*, ID3D11DeviceContext*, FontShaderClass*, XMMATRIX, XMMATRIX);

private:
	int m_screenWidth = 0;
	int m_screenHeight = 0;
	
	XMMATRIX m_baseViewMatrix;
	
	FontClass* m_Font = nullptr;

	SentenceType* m_sentence1 = nullptr;
	SentenceType* m_sentence2 = nullptr;
	SentenceType* m_sentence3 = nullptr;
	SentenceType* m_sentence4 = nullptr;
	SentenceType* m_sentence5 = nullptr;
	SentenceType* m_sentence6 = nullptr;
	SentenceType* m_sentence7 = nullptr;
	SentenceType* m_sentence8 = nullptr;
	SentenceType* m_sentence9 = nullptr;
	SentenceType* m_sentence10 = nullptr;
};