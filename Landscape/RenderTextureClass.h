#ifndef RENDERTEXTURECLASS_H
#define RENDERTEXTURECLASS_H

#include <D3D10.h>

class RenderTextureClass
{
public:
	RenderTextureClass();
	RenderTextureClass(const RenderTextureClass&);
	~RenderTextureClass();

	bool Initialize(ID3D10Device* device, int textureWidth, int textureHeight);
	void SetRenderTarget(ID3D10Device* , ID3D10DepthStencilView*);
	void ClearRenderTarget(ID3D10Device* , ID3D10DepthStencilView*, float, float,float, float);
	ID3D10ShaderResourceView* GetShaderResourceView();
	void Shutdown();

private:
	ID3D10Texture2D* m_renderTargetTexture;
	ID3D10RenderTargetView* m_renderTargetView;
	ID3D10ShaderResourceView* m_shaderResourceView;
};

#endif 