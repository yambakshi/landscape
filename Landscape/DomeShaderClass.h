#ifndef DOMESHADERCLASS_H
#define DOMESHADERCLASS_H


#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

class DomeShaderClass
{
public:
	DomeShaderClass();
	DomeShaderClass(const DomeShaderClass&);
	~DomeShaderClass();

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device* device, long indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				ID3D10ShaderResourceView* texture, float skyTranslation);

private:
	// METHODS // 
	bool InitializeShader(ID3D10Device*, HWND, LPCSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCSTR);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*,float);
	void RenderShader(ID3D10Device*, long);

	// VARIABLES // 
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;
	ID3D10EffectShaderResourceVariable* m_texturePtr;
	ID3D10EffectScalarVariable* m_skyTranslationPtr;
};

#endif


