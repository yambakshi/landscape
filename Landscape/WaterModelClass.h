#ifndef WATERMODELCLASS_H
#define WATERMODELCLASS_H

#include <D3DX10.h>
#include <fstream>

using namespace std;

class WaterModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};
public:
	WaterModelClass();
	WaterModelClass(const WaterModelClass&);
	~WaterModelClass();

	bool Initialize(ID3D10Device* device, char* modelFilename, LPCSTR bumpmapFilename);

	void Shutdown();
	void Render(ID3D10Device*);

	int GetIndexCount();
	unsigned int GetWidth();
	unsigned int GetHeight();
	ID3D10ShaderResourceView* GetNormalTexture();

private:
	// METHODS // 
	bool InitializeBuffers(ID3D10Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device*);

	bool LoadTextures(ID3D10Device*, LPCSTR);
	void ReleaseTextures();

	bool LoadModel(char*);
	void ReleaseModel();
	
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	// VARIABLES // 
	ID3D10Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	unsigned int m_Width, m_Height;
	ModelType* m_model;
	ID3D10ShaderResourceView* m_bumpTexture;
};

#endif