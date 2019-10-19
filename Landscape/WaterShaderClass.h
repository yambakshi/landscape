#ifndef WATERSHADERCLASS_H
#define WATERSHADERCLASS_H

//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10.h>
#include <fstream>

using namespace std;
class WaterShaderClass
{
public:
	WaterShaderClass();
	WaterShaderClass(const WaterShaderClass&);
	~WaterShaderClass();

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, 
			    D3DXMATRIX projectionMatrix, D3DXMATRIX reflectionMatrix, 
			    ID3D10ShaderResourceView* reflectionTexture, ID3D10ShaderResourceView* refractionTexture,
			    ID3D10ShaderResourceView* normalTexture, float waterTranslation, float reflectRefractScale,
				D3DXVECTOR3 cameraPosition, D3DXVECTOR3 lightDirection, D3DXCOLOR specularColor, float specularPower);
	
private:
	// METHODS // 
	bool InitializeShader(ID3D10Device*, HWND, LPCSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCSTR);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D10ShaderResourceView*, 
							 ID3D10ShaderResourceView*, ID3D10ShaderResourceView*, float, float,
							 D3DXVECTOR3, D3DXVECTOR3, D3DXCOLOR, float);
	void RenderShader(ID3D10Device*, int);

	// VARIABLES // 
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;

	ID3D10EffectMatrixVariable* m_reflectionMatrixPtr;
	ID3D10EffectShaderResourceVariable* m_reflectionTexturePtr;
	ID3D10EffectShaderResourceVariable* m_refractionTexturePtr;
	ID3D10EffectShaderResourceVariable* m_normalTexturePtr;
	ID3D10EffectScalarVariable* m_translationPtr;
	ID3D10EffectScalarVariable* m_reflectRefractScalePtr;
	
	ID3D10EffectVectorVariable* m_cameraPositionPtr;
	ID3D10EffectVectorVariable* m_lightDirectionPtr;
	ID3D10EffectVectorVariable* m_specularColorPtr;
	ID3D10EffectScalarVariable* m_specularPowerPtr;
	
};

#endif