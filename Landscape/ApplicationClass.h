#ifndef APPLICATIONCLASS_H
#define APPLICATIONCLASS_H

// MY INCLUDES //
#include "WindowClass.h"
#include "InputClass.h"
#include "DirectXClass.h"
#include "CameraClass.h"

#include "LightClass.h"
#include "TimerClass.h"
#include "CpuClass.h"
#include "TextClass.h"
#include "BitmapClass.h"
#include "TerrainClass.h"
#include "RenderTextureClass.h"
#include "ModelClass.h"
#include "WaterModelClass.h"

#include "DomeShaderClass.h"
#include "RefractionShaderClass.h"
#include "WaterShaderClass.h"
#include "TerrainShaderClass.h"
#include "TerrainClipShaderClass.h"

// GLOBALS //
static bool FULL_SCREEN = true;
static bool SHOW_CURSOR = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize();
	void Run();
	void Shutdown();

private:
	// Methods
	bool InitializeFramework();
	bool InitializeObjects();
	bool InitializeShaders();
	void Update(MSG&);
	void Render();
	void RenderRefractionToTexture();
	void RenderReflectionToTexture();
	void RenderScene();
	void RenderText(D3DXMATRIX&, D3DXMATRIX&);

	// Members
	WindowClass *m_Window;
	InputClass *m_Input;
	DirectXClass *m_Device;
	CameraClass *m_Camera;

	float 
		m_FadeAlpha,
		m_SkyTranslation,
		m_WaterTranslation,
		m_WaterHeight;
	LightClass* m_Light;
	TimerClass* m_Timer;
	CpuClass* m_Cpu;
	TextClass *m_Text;
	BitmapClass *m_ScreenFade;
	TerrainClass* m_Terrain;
	RenderTextureClass
		*m_ReflectionTexture,
		*m_RefractionTexture;
	ModelClass *m_DomeModel;
	WaterModelClass* m_SpecWaterModel;

	TerrainShaderClass* m_TerrainShader;
	TerrainClipShaderClass* m_TerrainClipShader;
	DomeShaderClass* m_DomeShader;
	RefractionShaderClass* m_RefractionShader;
	WaterShaderClass* m_WaterShader;
};

#endif