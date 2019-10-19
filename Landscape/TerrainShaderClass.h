#ifndef TERRAINSHADERCLASS_H
#define TERRAINSHADERCLASS_H


#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

class TerrainShaderClass
{
public:
	TerrainShaderClass();
	TerrainShaderClass(const TerrainShaderClass&);
	~TerrainShaderClass();

	bool Initialize(ID3D10Device*, HWND);
	void Shutdown();
	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				ID3D10ShaderResourceView** texturesArray,
				D3DXVECTOR3 lightDirection, D3DXCOLOR diffuseColor, D3DXCOLOR ambientColor);

private:
	// METHODS // 
	bool InitializeShader(ID3D10Device*, HWND, LPCSTR);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, LPCSTR);

	void SetShaderParameters(D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
		ID3D10ShaderResourceView**, 
		D3DXVECTOR3, D3DXCOLOR, D3DXCOLOR);
	void RenderShader(ID3D10Device*, int);

	// VARIABLES // 
	ID3D10Effect* m_effect;
	ID3D10EffectTechnique* m_technique;
	ID3D10InputLayout* m_layout;

	ID3D10EffectMatrixVariable* m_worldMatrixPtr;
	ID3D10EffectMatrixVariable* m_viewMatrixPtr;
	ID3D10EffectMatrixVariable* m_projectionMatrixPtr;
	ID3D10EffectShaderResourceVariable* m_TexturesArrayPtr;
	ID3D10EffectVectorVariable* m_lightDirectionPtr;
	ID3D10EffectVectorVariable* m_ambientColorPtr;
	ID3D10EffectVectorVariable* m_diffuseColorPtr;
};

#endif