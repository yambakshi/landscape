#ifndef BITMAPCLASS_H
#define BITMAPCLASS_H

#include "TextureClass.h"

class BitmapClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D10Device* device, LPCSTR filename, unsigned int bitmapWidth, unsigned int bitmapHeight, unsigned int screenWidth, unsigned int screenHeight);
	bool Render(ID3D10Device* device, int positionX, int positionY);
	void Shutdown();

	unsigned long GetIndexCount();
	ID3D10ShaderResourceView* GetTexture();

private:
	// METHODS // 
	bool InitializeBuffers(ID3D10Device*);
	bool LoadTexture(ID3D10Device*, LPCSTR);
	bool UpdateBuffers(int, int);
	void RenderBuffers(ID3D10Device*);
	void ReleaseTexture();
	void ShutdownBuffers();

	// VARIABLES // 
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	TextureClass* m_Texture;
	unsigned int 
		m_vertexCount, m_indexCount,
		m_screenWidth, m_screenHeight,
		m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;
};

#endif 
