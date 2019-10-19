#include "ApplicationClass.h"


ApplicationClass::ApplicationClass()
{
	m_Window = 0;
	m_Input = 0;
	m_Device = 0;
	m_Camera = 0;

	m_Light = 0;
	m_Timer = 0;
	m_Cpu = 0;
	m_Text = 0;
	m_ScreenFade = 0;
	m_Terrain = 0;
	m_ReflectionTexture = 0;
	m_RefractionTexture = 0;
	m_DomeModel = 0;
	m_SpecWaterModel = 0;

	m_TerrainShader = 0;
	m_TerrainClipShader = 0;
	m_DomeShader = 0;
	m_RefractionShader = 0;
	m_WaterShader = 0;
}

ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}

ApplicationClass::~ApplicationClass()
{
}

bool ApplicationClass::Initialize()
{
	if (!InitializeFramework())
		return false;

	if (!InitializeObjects())
		return false;

	if (!InitializeShaders())
		return false;

	// Set the height of the water.
	m_WaterHeight = 6.5f;

	// Initialize the position of the water.
	m_WaterTranslation = 0.0f;
	
	// Initialize the position of the sky.
	m_SkyTranslation = 0.0f;

	// initial fade alpha
	m_FadeAlpha = 1.0f;

	return true;
}

bool ApplicationClass::InitializeFramework()
{
	RECT windowRect;


	// Create window object
	m_Window = new WindowClass;
	if (!m_Window)
		return false;

	// Initialize window object
	if (!m_Window->Initialize(FULL_SCREEN))
		return false;

	// Create input object
	m_Input = new InputClass;
	if (!m_Input)
		return false;

	// Initialize window object
	m_Input->Initialize();

	GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

	m_Device = new DirectXClass;
	if (!m_Device)
		return false;

	if (!m_Device->Initialize(m_Window->GetWindowHandle(),
		(windowRect.right - windowRect.left), (windowRect.bottom - windowRect.top),
		VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
		return false;


	return true;
}

bool ApplicationClass::InitializeObjects()
{
	D3DXMATRIX viewMatrix2D;
	RECT windowRect;

	
	// Get the window rectangle
	GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

	// Create the camera object
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// Set the camera position
	m_Camera->SetPosition(-68.0f, 23.0f, -63.0f);
	m_Camera->SetRotation(0.0f, 45.0f, 0.0f);

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
		return false;

	// Initialize the light object.
	m_Light->SetDirection(0.0f, -1.0f, 0.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetAmbientColor(0.2f, 0.2f, 0.2f, 1.0f);
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(16.0f);

	// create the time object
	m_Timer = new TimerClass;
	if(!m_Timer)
		return false;

	// initialize timer object
	if(!m_Timer->Initialize(m_Window->GetWindowHandle()))
		return false;

	// Create cpu object
	m_Cpu = new CpuClass;
	if(!m_Cpu)
		return false;

	// Initialize cpu object
	m_Cpu->Initialize();

	//Get 2D view matrix
	m_Camera->Render2D();
	m_Camera->Get2DViewMatrix(viewMatrix2D);

	// Create the text object.
	m_Text = new TextClass;
	if(!m_Text)
		return false;

	// Initialize text object
	if(!m_Text->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle(), viewMatrix2D, "Data/Textures/Font.dds"))
		return false;

	m_ScreenFade = 0;

	// Create the terrain object.
	m_Terrain = new TerrainClass;
	if(!m_Terrain)
		return false;
	
	// Initialize terrain object.
	if(!m_Terrain->Initialize(m_Device->GetDevice(), "Data/Heightmaps/Heightmap2.bmp"))
		return false;
	
	// Create the reflection render texture object
	m_ReflectionTexture = new RenderTextureClass;
	if(!m_ReflectionTexture)
		return false;
	
	// Initialize reflection render texture object.
	if(!m_ReflectionTexture->Initialize(m_Device->GetDevice(), windowRect.right-windowRect.left, windowRect.bottom-windowRect.top))
		return false;
	
	// Create the refraction render texture object
	m_RefractionTexture = new RenderTextureClass;
	if(!m_RefractionTexture)
		return false;
	
	// Initialize refraction render texture object.
	if(!m_RefractionTexture->Initialize(m_Device->GetDevice(), windowRect.right-windowRect.left, windowRect.bottom-windowRect.top))
		return false;

	// Create the dome object.
	m_DomeModel = new ModelClass;
	if(!m_DomeModel)
		return false;
	
	// Initialize dome object.
	if(!m_DomeModel->Initialize(m_Device->GetDevice(), "Data/Models/Dome.txt", "Data/Textures/Sky 2.dds"))
		return false;

	// Create the specular water object.
	m_SpecWaterModel = new WaterModelClass;
	if(!m_SpecWaterModel)
		return false;
	
	// Initialize specular water object.
	if(!m_SpecWaterModel->Initialize(m_Device->GetDevice(), "Data/Models/Water1.txt", "Data/Bumpmaps/Waterbump1.dds"))
		return false;


	return true;
}

bool ApplicationClass::InitializeShaders()
{
	// Create the terrain shader object	
	m_TerrainShader = new TerrainShaderClass;
	if(!m_TerrainShader)
		return false;

	// Initialize the terrain shader object	
	if(!m_TerrainShader->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle()))
		return false;

	// Create the terrain cilp shader object	
	m_TerrainClipShader = new TerrainClipShaderClass;
	if(!m_TerrainClipShader)
		return false;

	// Initialize the terrain clip shader object	
	if(!m_TerrainClipShader->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle()))
		return false;

	// Create the dome shader object	
	m_DomeShader = new DomeShaderClass;
	if(!m_DomeShader)
		return false;

	// Initialize the dome shader object	
	if(!m_DomeShader->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle()))
		return false;

	// Create the refraction shader object	
	m_RefractionShader = new RefractionShaderClass;
	if(!m_RefractionShader)
		return false;

	// Initialize the refraction shader object	
	if(!m_RefractionShader->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle()))
		return false;
	 
	// Create the water shader object	
	m_WaterShader = new WaterShaderClass;
	if(!m_WaterShader)
		return false;

	// Initialize the water shader object	
	if(!m_WaterShader->Initialize(m_Device->GetDevice(), m_Window->GetWindowHandle()))
		return false;


	return true;
}

void ApplicationClass::Run()
{
	MSG msg;


	// Initialziethe message object
	ZeroMemory(&msg, sizeof(MSG));
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;
		else
		{
			Update(msg);
			Render();
		}
	}


	return;
}

void ApplicationClass::Update(MSG &msg)
{
	RECT windowRect;

	m_Timer->Update();
	m_Cpu->Update();
	m_Input->Update(msg);

	if (m_Input->IsKeyDown((unsigned int)'F'))
	{
		m_Input->KeyUp((unsigned int)'F');
		
		// Toggle full screen and windowed mode
		if (FULL_SCREEN)
		{
			FULL_SCREEN = false;
			m_Window->FullScreenOFF();
		}
		else
		{
			FULL_SCREEN = true;
			m_Window->FullScreenON();
		}

		// Get window rectangle
		GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

		m_Device->ResizeBackBuffer((windowRect.right - windowRect.left), (windowRect.bottom - windowRect.top),
			FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	}

	if (m_Input->IsKeyDown((unsigned int)'C'))
	{
		// Get window rectangle
		GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

		m_Input->KeyUp((unsigned int)'C');

		if (SHOW_CURSOR)
			SHOW_CURSOR = false;
		else
			SHOW_CURSOR = true;

		SetCursorPos(windowRect.left + (windowRect.right - windowRect.left) / 2.0f, windowRect.top + (windowRect.bottom - windowRect.top) / 2.0f);
		ShowCursor(SHOW_CURSOR);
	}


	return;
}

void ApplicationClass::Render()
{	
	static float rotation = 0.0f;
	RECT windowRect;

		
	// Update the position of the water to simulate motion.
	m_WaterTranslation += 0.0005f;
	if(m_WaterTranslation > 1.0f)
		m_WaterTranslation -= 1.0f;

	m_SkyTranslation += 0.00005f;
	if(m_SkyTranslation > 1.0f)
		m_SkyTranslation -= 1.0f;

	m_FadeAlpha -= 0.005f;
	if(m_FadeAlpha < 0.0f)
		m_FadeAlpha = 0.0f;
		
	// Render the refraction of the scene to a texture
	RenderRefractionToTexture();
	
	// Render the reflection of the scene to a texture
	RenderReflectionToTexture();
	
	// Render the scene as normal to the back buffer
	RenderScene();


	return;
}

void ApplicationClass::RenderRefractionToTexture()
{
	D3DXVECTOR3 lightrot;
	D3DXMATRIX translate, rotation;
	float angle = (D3DX_PI/180.0f) / 5.0f;
	D3DXVECTOR4 clipPlane;
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix;
	RECT windowRect;


	// Get window rectangle
	GetWindowRect(m_Window->GetWindowHandle(), &windowRect);
		
	if (!SHOW_CURSOR)
	{
		// Create the view matrix
		m_Camera->Render(m_Input,
			windowRect.left + (windowRect.right - windowRect.left) / 2.0f, windowRect.top + (windowRect.bottom - windowRect.top) / 2.0f);
	}

	// Setup a clipping plane based on the height of the water to clip everything above it.
	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, m_WaterHeight + 0.1f);
	
	// Rotate the light direction.	
	lightrot = m_Light->GetDirection();
	D3DXMatrixRotationZ(&rotation,angle);
	D3DXVec3TransformCoord(&lightrot,&lightrot,&rotation);
	m_Light->SetDirection(lightrot.x, lightrot.y, lightrot.z);

	// Set the render target to be the refraction render to texture.
	m_RefractionTexture->SetRenderTarget(m_Device->GetDevice(), m_Device->GetDepthStencilView());

	// Clear the refraction render to texture.
	m_RefractionTexture->ClearRenderTarget(m_Device->GetDevice(), m_Device->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 0.0f);

	// Get the world and projection matrices.
	m_Device->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Device->GetProjectionMatrix(projectionMatrix);
		
	// translates the terrain to the center-axis
	D3DXMatrixTranslation(&translate, m_Terrain->GetWidth()/(-2.0f), 0.0f, m_Terrain->GetHeight()/(-2.0f));
	worldMatrix *= translate;
	
	// render the terrain
	m_Terrain->Render(m_Device->GetDevice());

	m_RefractionShader->Render(
		m_Device->GetDevice(),
		m_Terrain->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_Terrain->GetTexturesArray()[0],
		m_Light->GetDirection(),
		m_Light->GetAmbientColor(),
		m_Light->GetDiffuseColor(),
		clipPlane);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Device->SetBackBufferRenderTerget();


	return;
}

void ApplicationClass::RenderReflectionToTexture()
{
	D3DXVECTOR3 lightrot;
	D3DXMATRIX translate, rotation, scale;
	float angle = (D3DX_PI/180.0f) / 5.0f;
	D3DXMATRIX reflectionViewMatrix, worldMatrix, projectionMatrix;
	D3DXVECTOR4 clipPlane;


	// Setup a clipping plane based on the height of the water to clip everything above it.
	clipPlane = D3DXVECTOR4(0.0f, -1.0f, 0.0f, m_WaterHeight - 0.1f);

	// Rotate the light direction.	
	lightrot = m_Light->GetDirection();
	D3DXMatrixRotationZ(&rotation,angle);
	D3DXVec3TransformCoord(&lightrot,&lightrot,&rotation);
	m_Light->SetDirection(lightrot.x, lightrot.y, lightrot.z);

	// Set the render target to be the reflection render to texture.
	m_ReflectionTexture->SetRenderTarget(m_Device->GetDevice(), m_Device->GetDepthStencilView());

	// Clear the reflection render to texture.
	m_ReflectionTexture->ClearRenderTarget(m_Device->GetDevice(), m_Device->GetDepthStencilView(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Use the camera to render the reflection and create a reflection view matrix.
	m_Camera->RenderReflection(m_WaterHeight);

	// Get the camera reflection view matrix instead of the normal view matrix.
	m_Camera->GetReflectionViewMatrix(reflectionViewMatrix);

	// Get the world and projection matrices from the d3d object.
	m_Device->GetWorldMatrix(worldMatrix);
	m_Device->GetProjectionMatrix(projectionMatrix);
	
	// Translate and scales the dome model befroe rendering.
	D3DXMatrixTranslation(&translate, 0.0f,-80.0f, 0.0f);
	D3DXMatrixScaling(&scale, 500.0f, 500.0f, 500.0f);
	worldMatrix *= scale*translate;

	// Put the dome model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_DomeModel->Render(m_Device->GetDevice());

	// Render the dome model using the texture shader and the reflection view matrix.
	m_DomeShader->Render(
		m_Device->GetDevice(), m_DomeModel->GetIndexCount(), 
		worldMatrix, reflectionViewMatrix, projectionMatrix,
		m_DomeModel->GetTexture(), m_SkyTranslation);

	// reset world matrix.
	m_Device->GetWorldMatrix(worldMatrix);

	// translates the terrain to the center-axis
	D3DXMatrixTranslation(&translate, m_Terrain->GetWidth()/(-2.0f), 0.0f, m_Terrain->GetHeight()/(-2.0f));
	worldMatrix *= translate;

	// render the terrain
	m_Terrain->Render(m_Device->GetDevice());

	m_TerrainClipShader->Render(
		m_Device->GetDevice(),
		m_Terrain->GetIndexCount(),
		worldMatrix, reflectionViewMatrix, projectionMatrix,
		m_Terrain->GetTexturesArray(),
		m_Light->GetDirection(),
		m_Light->GetDiffuseColor(),
		m_Light->GetAmbientColor(),
		-clipPlane);
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Device->SetBackBufferRenderTerget();


	return;
}

void ApplicationClass::RenderScene()
{
	D3DXVECTOR3 lightrot;	
	D3DXMATRIX rotation, translate, scale;
	float angle = (D3DX_PI/180.0f) / 5.0f;
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix, reflectionMatrix;
	RECT windowRect;

	
	// Get window rectangle
	GetWindowRect(m_Window->GetWindowHandle(), &windowRect);
		
	if (!SHOW_CURSOR)
	{
		// Create the view matrix
		m_Camera->Render(m_Input,
			windowRect.left + (windowRect.right - windowRect.left) / 2.0f, windowRect.top + (windowRect.bottom - windowRect.top) / 2.0f);
	}

	// Rotate the light direction.	
	lightrot = m_Light->GetDirection();
	D3DXMatrixRotationZ(&rotation,angle);
	D3DXVec3TransformCoord(&lightrot,&lightrot,&rotation);
	m_Light->SetDirection(lightrot.x, lightrot.y, lightrot.z);

	// Clear the buffers to begin the scene.
	m_Device->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Device->GetWorldMatrix(worldMatrix);
	m_Device->GetProjectionMatrix(projectionMatrix);

	// Translates the terrain to the center-axis
	D3DXMatrixTranslation(&translate, m_Terrain->GetWidth()/(-2.0f), 0.0f, m_Terrain->GetHeight()/(-2.0f));
	worldMatrix *= translate;

	// Render the terrain
	m_Terrain->Render(m_Device->GetDevice());

	m_TerrainShader->Render(
		m_Device->GetDevice(),
		m_Terrain->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_Terrain->GetTexturesArray(),
		m_Light->GetDirection(),
		m_Light->GetDiffuseColor(),
		m_Light->GetAmbientColor());

	// Reset the world matrix
	m_Device->GetWorldMatrix(worldMatrix);

	// Translate and scales the dome model befroe rendering.
	D3DXMatrixTranslation(&translate, 0.0f,-80.0f, 0.0f);
	D3DXMatrixScaling(&scale, 500.0f, 500.0f, 500.0f);
	worldMatrix *= scale*translate;
	
	m_DomeModel->Render(m_Device->GetDevice());

	m_DomeShader->Render(
		m_Device->GetDevice(),
		m_DomeModel->GetIndexCount(),
		worldMatrix, viewMatrix, projectionMatrix,
		m_DomeModel->GetTexture(), m_SkyTranslation);

	// Reset the world matrix
	m_Device->GetWorldMatrix(worldMatrix);

	// Translate and scales the water model befroe rendering.
	D3DXMatrixRotationY(&rotation, 0.0174532925f*90.0f);
	D3DXMatrixScaling(&scale,
		m_Terrain->GetWidth() / 2.0f, 
		m_Terrain->GetWidth() / 2.0f,
		m_Terrain->GetHeight() / 2.0f);
	D3DXMatrixTranslation(&translate, 0.0f, m_WaterHeight, 0.0f);
	worldMatrix = rotation*scale*translate;

	// Get the camera reflection view matrix.
	m_Camera->GetReflectionViewMatrix(reflectionMatrix);

	// Put the water model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SpecWaterModel->Render(m_Device->GetDevice());

	// Render the water model using the water shader.
	m_WaterShader->Render(
		m_Device->GetDevice(), m_SpecWaterModel->GetIndexCount(), 
		worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix,
		m_ReflectionTexture->GetShaderResourceView(), m_RefractionTexture->GetShaderResourceView(),
		m_SpecWaterModel->GetNormalTexture(), m_WaterTranslation, 0.05f,
		m_Camera->GetPosition(), m_Light->GetDirection(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower());
	
	// 2D rendering
	m_Device->TurnZBufferOff();
	
	m_Device->GetWorldMatrix(worldMatrix);
	m_Camera->Get2DViewMatrix(viewMatrix);
	m_Device->GetOrthoMatrix(orthoMatrix);

	RenderText(worldMatrix, orthoMatrix);
	/*
	// Fade into the scene.
	if(m_FadeAlpha > 0.0f)
	{
		m_ScreenFade->Render(m_Device->GetDevice(), 0, 0);

		m_ColorShader->Render(m_Device->GetDevice(), m_ScreenFade->GetIndexCount(),
			worldMatrix, viewMatrix, orthoMatrix, D3DXCOLOR(0.0f, 0.0f, 0.0f, m_FadeAlpha));
	}	
	*/
	m_Device->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Device->EndScene();


	return;
}

void ApplicationClass::RenderText(D3DXMATRIX& worldMatrix, D3DXMATRIX& orthoMatrix)
{
	char str1[30], str2[20];
	char time[9], sec[3], min[3], hour[3];
	unsigned int currTime, fps, cpu;
	float frameTime, fontSize;
	RECT windowRect;


	// Get window rectangle
	GetWindowRect(m_Window->GetWindowHandle(), &windowRect);

	// Set the font size
	fontSize = (windowRect.bottom-windowRect.top) * 0.05f;

	// camera position //
	m_Text->Render(m_Device->GetDevice(), "Camera Position:", fontSize, 10, -15, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top));
	
	sprintf_s(str2, "%f", m_Camera->GetPosition().x);
	strcpy_s(str1,"X: ");
	strcat_s(str1, str2);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 10, -35 + fontSize, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top)); // x

	sprintf_s(str2, "%f", m_Camera->GetPosition().y);
	strcpy_s(str1,"Y: ");
	strcat_s(str1, str2);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 10, -55 + fontSize*2, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top)); // y

	sprintf_s(str2, "%f", m_Camera->GetPosition().z);
	strcpy_s(str1,"Z: ");
	strcat_s(str1, str2);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 10, -75 + fontSize*3, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top)); // z

	
	// camera rotation //
	m_Text->Render(m_Device->GetDevice(), "Camera Rotation:", fontSize, 255, -15, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top));
	
	sprintf_s(str2, "%f", m_Camera->GetRotation().x);
	strcpy_s(str1,"X: ");
	strcat_s(str1, str2);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 255, -35 + fontSize, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top)); // x

	sprintf_s(str2, "%f", m_Camera->GetRotation().y);
	strcpy_s(str1,"Y: ");
	strcat_s(str1, str2);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 255, -55 + fontSize*2, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top)); // y

	sprintf_s(str2, "%f", m_Camera->GetRotation().z);
	strcpy_s(str1,"Z: ");
	strcat_s(str1, str2);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 255, -75 + fontSize*3, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top)); // z

	// Time //

	// Get current time
	currTime = m_Timer->GetElapsedTime();

	// HOURS
	_itoa_s(currTime/3600, hour, 10);
	if(currTime/3600 < 10)
	{
		strcpy_s(time, "0");
		strcat_s(time, hour);
	}
	else
	{
		strcpy_s(time, hour);
	}

	// MINUTES
	_itoa_s((currTime/60)%60, min, 10);
	if((currTime/60)%60 < 10)
	{
		strcat_s(time, ":0");
		strcat_s(time, min);
	}
	else
	{
		strcat_s(time, ":");
		strcat_s(time, min);
	}

	// SECONDS
	_itoa_s(currTime%60, sec, 10);
	if(currTime%60 < 10)
	{
		strcat_s(time, ":0");
		strcat_s(time, sec);
	}
	else
	{
		strcat_s(time, ":");
		strcat_s(time, sec);
	}
	
	strcpy_s(str1,"Elapsed Time: ");
	strcat_s(str1,time);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 515, -15, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top));
	
	// FPS //
	fps = m_Timer->GetFPS();
	strcpy_s(str1,"FPS: ");
	sprintf_s(str2, "%d", fps);
	strcat_s(str1,str2);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 515, -35 + fontSize, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top));

	// Frame time //
	frameTime = m_Timer->GetFrameTime();
	strcpy_s(str1,"Frame Time: ");
	sprintf_s(str2, "%f", frameTime);
	strcat_s(str1,str2);
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 515, -55 + fontSize*2, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top));

	// CPU //
	cpu = m_Cpu->GetCpuPercentage();
	strcpy_s(str1,"CPU: ");
	_itoa_s(cpu, str2, 10);
	strcat_s(str1,str2);
	strcat_s(str1, "%");
	m_Text->Render(m_Device->GetDevice(), str1, fontSize, 515, -75 + fontSize*3, D3DXCOLOR(1.0f,1.0f,1.0f,1.0f),
		worldMatrix, orthoMatrix, (windowRect.right-windowRect.left), (windowRect.bottom-windowRect.top));;
	

	return;
}

void ApplicationClass::Shutdown()
{
	// Framework
	if (m_Window)
	{
		m_Window->Shutdown();
		delete m_Window;
		m_Window = 0;
	}

	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	if (m_Device)
	{
		m_Device->Shutdown();
		delete m_Device;
		m_Device = 0;
	}

	// Objects
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	if (m_ScreenFade)
	{
		m_ScreenFade->Shutdown();
		delete m_ScreenFade;
		m_ScreenFade = 0;
	}

	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (m_ReflectionTexture)
	{
		m_ReflectionTexture->Shutdown();
		delete m_ReflectionTexture;
		m_ReflectionTexture = 0;
	}

	if (m_RefractionTexture)
	{
		m_RefractionTexture->Shutdown();
		delete m_RefractionTexture;
		m_RefractionTexture = 0;
	}

	if (m_DomeModel)
	{
		m_DomeModel->Shutdown();
		delete m_DomeModel;
		m_DomeModel = 0;
	}

	if (m_SpecWaterModel)
	{
		m_SpecWaterModel->Shutdown();
		delete m_SpecWaterModel;
		m_SpecWaterModel = 0;
	}

	// Shaders
	if (m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}
	
	if (m_TerrainClipShader)
	{
		m_TerrainClipShader->Shutdown();
		delete m_TerrainClipShader;
		m_TerrainClipShader = 0;
	}

	if (m_DomeShader)
	{
		m_DomeShader->Shutdown();
		delete m_DomeShader;
		m_DomeShader = 0;
	}

	if (m_RefractionShader)
	{
		m_RefractionShader->Shutdown();
		delete m_RefractionShader;
		m_RefractionShader = 0;
	}

	if (m_WaterShader)
	{
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}


	return;
}