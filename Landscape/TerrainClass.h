#ifndef TERRAINCLASS_H
#define TERRAINCLASS_H

//////////////
// INCLUDES //
//////////////
#include <d3d10.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

/////////////////
// MY INCLUDES //
/////////////////
#include "TextureClass.h"

class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR4 texWeights;
	};
public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D10Device*, char* heightmapFilename);
	void Render(ID3D10Device*);
	void Shutdown();

	bool InitializeBuffers(ID3D10Device*, char*);
	bool LoadTextures(ID3D10Device*);
	void RenderBuffers(ID3D10Device*);
	void ShutdownBuffers();
	void ReleaseTextures();

	long GetIndexCount();
	unsigned int GetWidth();
	unsigned int GetHeight();
	ID3D10ShaderResourceView** GetTexturesArray();

private:	
	// METHODS // 
	int SetOffset(const char*);
	int SetHeight(const char*);
	int SetWidth(const char*);
	inline float Clamp(float,float,float);

	ID3D10ShaderResourceView* m_texturesArray[4];

	// VARIABLES // 
	ID3D10Buffer 
		*m_vertexBuffer, 
		*m_indexBuffer;

	unsigned int 
		m_Width,
		m_Height;
	long
		m_verticesCount,
		m_indicesCount;
};

#endif