#include "stdafx.h"
#include "inputclass.h"
#include "d3dclass.h"
#include "cameraclass.h"
#include "terrainclass.h"
#include "colorshaderclass.h"
#include "timerclass.h"
#include "positionclass.h"
#include "fpsclass.h"
#include "cpuclass.h"
#include "fontshaderclass.h"
#include "textclass.h"
#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialise the input object
	bool result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create D3D object
	m_Direct3D = new D3DClass;
	if(!m_Direct3D)
	{
		return false;
	}

	//Init the D3D obj
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize DirectX 11.", L"Error", MB_OK);
		return false;
	}

	//create the camera object
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	XMMATRIX baseViewMatrix;
	m_Camera->SetPosition(XMFLOAT3(0.0f, 0.0f, -1.0f));
	m_Camera->Render();
	m_Camera->GetViewMatrix(baseViewMatrix);

	//set initial position of the camera
	XMFLOAT3 camera = XMFLOAT3(50.0f, 2.0f, -7.0f);
	m_Camera->SetPosition(camera);

	//Create the terrain object
	m_Terrain = new TerrainClass;
	if(!m_Terrain)
	{
		return false;
	}

	//Init the terrain object
	result = m_Terrain->Initialize(m_Direct3D->GetDevice(), "../ProceduralMethods/data/heightmap01.bmp");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	//create colour shader object
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return false;
	}

	//init the colour shader obj
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	//create the timer obj
	m_Timer = new TimerClass;
	if(!m_Timer)
	{
		return false;
	}

	//init the timer object
	result = m_Timer->Initialize();
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	//create a position object
	m_Position = new PositionClass;
	if(!m_Position)
	{
		return false;
	}

	//set the initial position of the viewer to be the same as the initial camera pos
	m_Position->SetPosition(camera);

	//create an fps object
	m_Fps = new FpsClass;
	if(!m_Fps)
	{
		return false;
	}

	//init the fps object
	m_Fps->Initialize();

	//create the CPU object
	m_Cpu = new CpuClass;
	if(!m_Cpu)
	{
		return false;
	}

	//init the CPU object
	m_Cpu->Initialize();

	//create the font shader object
	m_FontShader = new FontShaderClass;
	if(!m_FontShader)
	{
		return false;
	}

	//init the font shader object
	result = m_FontShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	//create the text object
	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}

	//init the text object
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	//get GPU info
	char videoCard[128] = { 0, };
	int videoMemory = 0;
	m_Direct3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	result = m_Text->SetVideoCardInfo(videoCard, videoMemory, m_Direct3D->GetDeviceContext());
	if(!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void ApplicationClass::Shutdown()
{

	// Release the text object.
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	//release the font shader object
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	//release the cpu object
	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	//release the fps object
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	//release the position object
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	//release the timer object
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	//release the colour shader object
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	//release the terrain object
	if(m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	//release the camera object
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	//release the D3D object
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	//release the input object
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
}


bool ApplicationClass::Frame()
{
	//read user input
	bool result = m_Input->Frame();
	if(!result)
	{
		return false;
	}
	
	// check if the application will be closed when ESC is pressed
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// update timer, fps, and cpu
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	//update the FPS value in the text object
	result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}
	
	//update the CPU usage in the text object
	result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	//perform frame input processing
	result = HandleInput(m_Timer->GetTime());
	if(!result)
	{
		return false;
	}

	//render the graphics
	result = RenderGraphics();
	if(!result)
	{
		return false;
	}

	return result;
}


bool ApplicationClass::HandleInput(float frameTime)
{
	XMFLOAT3 pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rot = XMFLOAT3(0.0f, 0.0f, 0.0f);


	//set the frame time to calculate the updated position
	m_Position->SetFrameTime(frameTime);

	//process the inputs
	m_Position->TurnLeft(m_Input->IsLeftPressed());
	m_Position->TurnRight(m_Input->IsRightPressed());
	m_Position->MoveForward(m_Input->IsUpPressed());
	m_Position->MoveBackward(m_Input->IsDownPressed());
	m_Position->MoveUpward(m_Input->IsAPressed());
	m_Position->MoveDownward(m_Input->IsZPressed());
	m_Position->LookUpward(m_Input->IsPgUpPressed());
	m_Position->LookDownward(m_Input->IsPgDownPressed());
	
	//get the starting position and rotation
	m_Position->GetPosition(pos);
	m_Position->GetRotation(rot);

	//set the camera's position
	m_Camera->SetPosition(pos);
	m_Camera->SetRotation(rot);

	//update the position value on the text object
	if(!m_Text->SetCameraPosition(pos, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	//update the rotation value on the text object
	if(!m_Text->SetCameraRotation(rot, m_Direct3D->GetDeviceContext()))
	{
		return false;
	}

	return true;
}


bool ApplicationClass::RenderGraphics()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;

	//erase the scene
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//create a view matrix based on the position of the camera
	m_Camera->Render();

	// get world, view, projection and ortho matricies from camera and D3D objects
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(orthoMatrix);

	//render the terrain buffer
	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	//render the model using a colour shader
	if(!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

	//turn off the Z buffer to start all 2D rendering
	m_Direct3D->TurnZBufferOff();
		
	//turn on the alpha blending before rendering the text
	m_Direct3D->TurnOnAlphaBlending();

	//render the text user interface element
	if(!m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix))
	{
		return false;
	}

	//turn off the alpha blending after rendering the text
	m_Direct3D->TurnOffAlphaBlending();

	// since all 2D rendering is complete, turn the Z buffer on
	m_Direct3D->TurnZBufferOn();

	//display the rendered scene 
	m_Direct3D->EndScene();

	return true;
}