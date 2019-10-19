#include "TerrainClass.h"


TerrainClass::TerrainClass()
{
	m_texturesArray[0] = 0;
	m_texturesArray[1] = 0;
	m_texturesArray[2] = 0;
	m_texturesArray[3] = 0;

	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

TerrainClass::TerrainClass(const TerrainClass& other)
{
}

TerrainClass::~TerrainClass()
{
}

bool TerrainClass::Initialize(ID3D10Device* device, char* heightmapFilename)
{
	if(!LoadTextures(device))
		return false;

	if(!InitializeBuffers(device, heightmapFilename))
		return false;

	return true;
}

bool TerrainClass::LoadTextures(ID3D10Device* device)
{
	// Load Textures

	if(FAILED(D3DX10CreateShaderResourceViewFromFile(device, "Data/Textures/Sand.dds", NULL, NULL, &m_texturesArray[0], NULL)))
		return false;

	if(FAILED(D3DX10CreateShaderResourceViewFromFile(device, "Data/Textures/Grass 1.dds", NULL, NULL, &m_texturesArray[1], NULL)))
		return false;

	if(FAILED(D3DX10CreateShaderResourceViewFromFile(device, "Data/Textures/Rock.dds", NULL, NULL, &m_texturesArray[2], NULL)))
		return false;

	if(FAILED(D3DX10CreateShaderResourceViewFromFile(device, "Data/Textures/Snow.dds", NULL, NULL, &m_texturesArray[3], NULL)))
		return false;
	
	return true;
}

bool TerrainClass::InitializeBuffers(ID3D10Device* device, char* heightmapFilename)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	short unsigned const int offset = SetOffset(heightmapFilename);
	short unsigned const int width  = SetWidth(heightmapFilename);
	short unsigned const int height = SetHeight(heightmapFilename);

	m_Width = width;
	m_Height = height;

	m_verticesCount = width * height;
	m_indicesCount = (width-1) * (height-1) * 6;

	vertices = new VertexType[m_verticesCount];
	if(!vertices)
	{
		return false;
	}

	indices = new unsigned long[m_indicesCount];
	if(!indices)
	{
		return false;
	}

	std::ifstream f_DataFile;
	f_DataFile.open(heightmapFilename, std::ios::binary);
	if (f_DataFile.is_open())
	{
		short unsigned int dummy;
		for (int i = 0; i<(offset);i++)
			dummy = f_DataFile.get();
		
		D3DXCOLOR* pixels = new D3DXCOLOR[m_verticesCount];
		for(int i=0;i<m_verticesCount; i++)
		{
			pixels[i].b = f_DataFile.get();
			pixels[i].g = f_DataFile.get();
			pixels[i].r = f_DataFile.get();
			if((i+1)%width == 0)
			{
				for(int j=0; j<width%4; j++)
					dummy = f_DataFile.get();
			}
		}
		
		float scale = 6.0f;
		float maxHeight = 255/scale;
		float blend = 8*(maxHeight/(width-1)); // the number of vertices that the textures are changing across
		float gap = maxHeight/6.0f + blend/2;
		float total; // sums up the texture weight for each vertex

		for (int z=0; z<height; z++) 
		{
			for (int x=0; x<width; x++)  
			{
				long index = x + z*width;
				float y = (pixels[index].r + pixels[index].g + pixels[index].b) / (3*scale);

				vertices[index].position = D3DXVECTOR3(x,y,z);
				vertices[index].texture = D3DXVECTOR2(
					x/(float)width,
					z/(float)height);

				vertices[index].texWeights = D3DXVECTOR4(
					1.0f - Clamp((y/gap),0.0f,1.0f),
					Clamp(((y-gap+blend)/gap),0.0f,1.0f) - Clamp((y-maxHeight/3)/gap,0.0f,1.0f),
					Clamp(((y-gap-(maxHeight/3)+blend)/gap),0.0f,1.0f) - Clamp((y-2*(maxHeight/3))/gap,0.0f,1.0f),
					Clamp(((y-gap-2*(maxHeight/3)+blend)/gap),0.0f,1.0f));


				total = 0;
				total += vertices[index].texWeights.x;
				total += vertices[index].texWeights.y;
				total += vertices[index].texWeights.z;
				total += vertices[index].texWeights.w;

				vertices[index].texWeights.x /= total;
				vertices[index].texWeights.y /= total;
				vertices[index].texWeights.z /= total;
				vertices[index].texWeights.w /= total;
			}
		}
		delete [] pixels;
	}
	else
	{
		MessageBox(NULL,"HeighData file not found!","FillVertices()",MB_OK);
	}

	f_DataFile.close();

	D3DXVECTOR3 side1, side2, normal1, normal2;

	// fill the indices array
	for(int i=0; i<(width-1) * (height-1); i++)
	{
		indices[i*6]     = i + i/(width-1);
		indices[(i*6)+1] = i + i/(width-1) + width;
		indices[(i*6)+2] = i + i/(width-1) + width+1;
		
		// calculates the normal of the LEFT-UPPER triangel
		side1 = vertices[i + i/(width-1)].position - vertices[i + i/(width-1) + width].position;
		side2 = vertices[i + i/(width-1)].position - vertices[i + i/(width-1) + width+1].position;
		D3DXVec3Cross(&normal1, &side1,&side2);

		// sets the normal to the LEFT-UPPER vertex
		// (the vertex that is not shared by the two traingles)
		vertices[i + i/(width-1) + width].normal = normal1;
		
		indices[(i*6)+3] = i + i/(width-1);
		indices[(i*6)+4] = i + i/(width-1) + width+1;
		indices[(i*6)+5] = i + i/(width-1) + 1;

		// calculates the normal of the RIGHT-DOWN triangel
		side1 = vertices[i + i/(width-1)].position - vertices[i + i/(width-1) + width+1].position;
		side2 = vertices[i + i/(width-1)].position - vertices[i + i/(width-1) + 1].position;
		D3DXVec3Cross(&normal2,&side1,&side2);

		// sets the normal to the RIGHT-DOWN vertex
		// (the vertex that is not shared by the two traingles)
		vertices[i + i/(width-1) + 1].normal = normal2;
		
		// calculates the average of the two normals
		// and sets the avarege normal to the shared vertices
		normal1 = (normal1+normal2)/2;
		vertices[i + i/(width-1)].normal = normal1;
		vertices[i + i/(width-1) + width+1].normal = normal1;
	}

	// Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_verticesCount;
    vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indicesCount;
    indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void TerrainClass::Render(ID3D10Device* device)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(device);

	return;
}

void TerrainClass::RenderBuffers(ID3D10Device* device)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
    stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
    device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
    device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void TerrainClass::Shutdown()
{
	ReleaseTextures();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void TerrainClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void TerrainClass::ReleaseTextures()
{
	if(m_texturesArray)
	{
		m_texturesArray[0]->Release();
		m_texturesArray[0] = 0;

		m_texturesArray[1]->Release();
		m_texturesArray[1] = 0;

		m_texturesArray[2]->Release();
		m_texturesArray[2] = 0;

		m_texturesArray[3]->Release();
		m_texturesArray[3] = 0;
	}
}

long TerrainClass::GetIndexCount()
{
	return m_indicesCount;
}

ID3D10ShaderResourceView** TerrainClass::GetTexturesArray()
{
	return m_texturesArray;
}

int TerrainClass::SetOffset(const char* filename)
{
    std::ifstream f_DataFile;
    short unsigned int offset;
    short dummy;
 
    f_DataFile.open(filename, std::ios::binary);
     
     
    for (int i = 0; i < 10; i++)
    {
        dummy = f_DataFile.get();
    }
 
    offset = f_DataFile.get();
    offset += f_DataFile.get()*256;
    offset += f_DataFile.get()*256*256;
    offset += f_DataFile.get()*256*256*256;
 
    f_DataFile.close();
     
    return offset;
}

int TerrainClass::SetHeight(const char* filename)
{
    std::ifstream f_DataFile;
    short unsigned int HEIGHT;
    short dummy;
 
    f_DataFile.open(filename, std::ios::binary);
 
    if (f_DataFile.is_open())
    {
        for (int i = 0; i < 22; i++)
        {
            dummy = f_DataFile.get();
        }
 
        HEIGHT = f_DataFile.get();
        HEIGHT += f_DataFile.get()*256;
        HEIGHT += f_DataFile.get()*256*256;
        HEIGHT += f_DataFile.get()*256*256*256;
 
        f_DataFile.close();
    }
    return HEIGHT;
}

int TerrainClass::SetWidth(const char* filename)
{
    std::ifstream f_DataFile;
    short unsigned int WIDTH;
    short dummy;
 
    f_DataFile.open(filename, std::ios::binary);
 
    if (f_DataFile.is_open())
    {
 
        for (int i = 0; i < 18; i++)
        {
            dummy = f_DataFile.get();
        }
 
        WIDTH = f_DataFile.get();
        WIDTH += f_DataFile.get()*256;
        WIDTH += f_DataFile.get()*256*256;
        WIDTH += f_DataFile.get()*256*256*256;
 
        f_DataFile.close();
    }
    return WIDTH;
}

unsigned int TerrainClass::GetWidth()
{
	return m_Width;
}

unsigned int TerrainClass::GetHeight()
{
	return m_Height;
}

inline float TerrainClass::Clamp(float num, float min,float max)
{
	if(num <= min) return min;
	else if(num >= max) return max;
	else return num;
}