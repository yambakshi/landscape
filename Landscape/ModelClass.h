#ifndef MODELCLASS_H
#define MODELCLASS_H

// INCLUDES //
#include <D3DX10.h>
#include <fstream>
using namespace std;

// MY INCLUDES //
#include "TextureClass.h"

class ModelClass
{
private:
	struct ModelType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D10Device *device, char* modelFilename, char *textureFilename);
	void Render(ID3D10Device *device);
	void Shutdown();

	ID3D10ShaderResourceView* GetTexture();
	unsigned long GetIndexCount();

private:
	// Methods
	bool LoadModelFromOBJ(char*);
	bool LoadModelFromTXT(char*);
	bool InitializeBuffers(ID3D10Device*);
	bool LoadTexture(ID3D10Device*, char*);
	void ReleaseModel();
	void ShutdownBuffers();
	void ReleaseTexture();

	// Members
	unsigned long m_VertexCount, m_IndexCount;
	ModelType *m_Model;
	TextureClass *m_Texture;
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
};

#endif