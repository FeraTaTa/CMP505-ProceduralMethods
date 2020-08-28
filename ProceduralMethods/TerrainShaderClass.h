#pragma  once
 
class  TerrainShaderClass:  public  AlignedAllocationPolicy < 16 >
{
private :
    struct  MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };
 
    struct  LightBufferType
    {
        XMFLOAT4 ambientColor;
        XMFLOAT4 diffuseColor;
        XMFLOAT3 lightDirection;
        float  padding;
    };
 
public :
    TerrainShaderClass();
    TerrainShaderClass( const  TerrainShaderClass & );
    ~ TerrainShaderClass();
 
    bool  Initialize(ID3D11Device * , HWND);
    void  Shutdown();
    bool  SetShaderParameters(ID3D11DeviceContext * , XMMATRIX, XMMATRIX, XMMATRIX, XMFLOAT4, XMFLOAT4, XMFLOAT3,
                             ID3D11ShaderResourceView * );
    void  RenderShader(ID3D11DeviceContext * ,  int );
 
private :
    bool  InitializeShader(ID3D11Device * , HWND,  const  WCHAR * ,  const  WCHAR * );
    void  ShutdownShader();
    void  OutputShaderErrorMessage(ID3D10Blob * , HWND,  const  WCHAR * );
 
private :
    ID3D11VertexShader *  m_vertexShader  =  nullptr;
    ID3D11PixelShader *  m_pixelShader  =  nullptr;
    ID3D11InputLayout *  m_layout  =  nullptr;
    ID3D11SamplerState *  m_sampleState  =  nullptr;
    ID3D11Buffer *  m_matrixBuffer  =  nullptr;
    ID3D11Buffer *  m_lightBuffer  =  nullptr;
};
