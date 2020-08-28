#pragma  once
 
/////////////
// GLOBALS //
/////////////
const  bool  FULL_SCREEN  =  false ;
const  bool  VSYNC_ENABLED  =  true ;
const  float  SCREEN_DEPTH  =  1000.0f;
const  float  SCREEN_NEAR  =  0.1f;
 
 
class  D3DClass;
class  InputClass;
class  CameraClass;
class  TerrainClass;
 
class  TimerClass;
class  PositionClass;
class  FpsClass;
class  CpuClass;
class  FontShaderClass;
class  TextClass;
class  TerrainShaderClass;
class  LightClass;
class  FrustumClass;
class  QuadTreeClass;
 
 
class  ApplicationClass
{
public :
    ApplicationClass();
    ApplicationClass( const  ApplicationClass & );
    ~ApplicationClass();
 
    bool  Initialize(HINSTANCE, HWND,  int ,  int );
    void  Shutdown();
    bool  Frame();
 
private :
    bool  HandleInput( float );
    bool  RenderGraphics();
 
private :
    InputClass *  m_Input  =  nullptr;
    D3DClass *  m_Direct3D  =  nullptr;
    CameraClass *  m_Camera  =  nullptr;
    TerrainClass *  m_Terrain  =  nullptr;
    TimerClass *  m_Timer  =  nullptr;
    PositionClass *  m_Position  =  nullptr;
    FpsClass *  m_Fps  =  nullptr;
    CpuClass *  m_Cpu  =  nullptr;
    FontShaderClass *  m_FontShader  =  nullptr;
    TextClass *  m_Text  =  nullptr;
    TerrainShaderClass *  m_TerrainShader  =  nullptr;
    LightClass *  m_Light  =  nullptr;
    FrustumClass *  m_Frustum  =  nullptr;
    QuadTreeClass *  m_QuadTree  =  nullptr;
};
