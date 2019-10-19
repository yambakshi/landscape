#include "ModelClass.h"


ModelClass::ModelClass()
{
	m_Model = 0;
	m_Texture = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D10Device *device, char *modelFilename, char *textureFilename)
{
	// If its an OBJ model type
	if (!strcmp(&modelFilename[(unsigned int)strlen(modelFilename) - 3], "obj"))
	{
		if (!LoadModelFromOBJ(modelFilename))
			return false;
	}
	// If its custom model type
	else if (!strcmp(&modelFilename[(unsigned int)strlen(modelFilename) - 3], "txt"))
	{
		if (!LoadModelFromTXT(modelFilename))
			return false;
	}
	else
		return false;
	
	if (!InitializeBuffers(device))
		return false;

	if (!LoadTexture(device, textureFilename))
		return false;


	return true;
}

bool ModelClass::LoadModelFromOBJ(char* modelFilename)
{
	char input;
	unsigned int 
		verticesCount, 
		texCoordsCount,
		normalsCount, 
		facesCount;
	ifstream file;	
	D3DXVECTOR2
		*texCoords;
	D3DXVECTOR3
		*vertices,
		*normals,
		*faces;


	// Count the vertices, texCoords, normals and faces
	file.open(modelFilename, ios::binary);
	if (file.is_open())
	{
		// Initialize the counters
		verticesCount = texCoordsCount = normalsCount = facesCount = 0;

		file.get(input);
		while (!file.eof())
		{
			if (input == 'v')
			{
				file.get(input);
				if (input == ' ') verticesCount++;
				else if (input == 't') texCoordsCount++;
				else if (input == 'n') normalsCount++;
			}
			else if (input == 'f')
			{
				file.get(input);
				if (input == ' ') facesCount++;
			}

			while (input != '\n')
				file.get(input);

			file.get(input);
		}
	}
	else return false;

	file.close();
	
	// Fill the vertices, texCoords, normals and faces arrays
	file.open(modelFilename, ios::binary);
	if (file.is_open())
	{
		// Create the objects
		vertices = new D3DXVECTOR3[verticesCount];
		if (!vertices) return false;

		texCoords = new D3DXVECTOR2[texCoordsCount];
		if (!texCoords) return false;

		normals = new D3DXVECTOR3[normalsCount];
		if (!normals) return false;

		faces = new D3DXVECTOR3[3 * facesCount];
		if (!faces) return false;


		// Initialize the counters
		verticesCount = texCoordsCount = normalsCount = facesCount = 0;

		file.get(input);
		while (!file.eof())
		{
			if (input == 'v')
			{
				file.get(input);
				if (input == ' ')
				{
					file >> vertices[verticesCount].x >> vertices[verticesCount].y >> vertices[verticesCount].z;

					// Invert the Z vertex to change to left hand system.
					vertices[verticesCount].z = vertices[verticesCount].z * -1.0f;
					verticesCount++;
				}
				else if (input == 't')
				{
					file >> texCoords[texCoordsCount].x >> texCoords[texCoordsCount].y;

					texCoords[texCoordsCount].y = 1.0f - texCoords[texCoordsCount].y;
					texCoordsCount++;
				}
				else if (input == 'n')
				{
					file >> normals[normalsCount].x >> normals[normalsCount].y >> normals[normalsCount].z;

					// Invert the Z vertex to change to left hand system.
					normals[normalsCount].z = normals[normalsCount].z * -1.0f;
					normalsCount++;
				}
			}
			else if (input == 'f')
			{
				file.get(input);
				if (input == ' ')
				{
					file >>
						faces[facesCount + 2].x >> input >> faces[facesCount + 2].y >> input >> faces[facesCount + 2].z >>
						faces[facesCount + 1].x >> input >> faces[facesCount + 1].y >> input >> faces[facesCount + 1].z >>
						faces[facesCount].x >> input >> faces[facesCount].y >> input >> faces[facesCount].z;

					facesCount += 3;
				}
			}

			while (input != '\n')
				file.get(input);

			file.get(input);
		}
	}
	else return false;

	file.close();

	// Set the vertices and indecis count
	m_VertexCount = m_IndexCount = facesCount;

	// Create the model
	m_Model = new ModelType[facesCount];
	if (!m_Model)
		return false;

	//Fill the model
	for (int i = 0; i < facesCount; i++)
	{
		m_Model[i].position = vertices[(int)faces[i].x - 1];
		m_Model[i].texture = texCoords[(int)faces[i].y - 1];
		m_Model[i].normal = normals[(int)faces[i].z - 1];
	}

	// Release arrays
	delete[] vertices;
	vertices = 0;

	delete[] texCoords;
	texCoords = 0;

	delete[] normals;
	normals = 0;

	delete[] faces;
	faces = 0;


	return true;
}

bool ModelClass::LoadModelFromTXT(char* modelFilename)
{
	char input;
	int i;
	ifstream file;

	
	// Count the vertices, texCoords, normals and faces
	file.open(modelFilename, ios::binary);
	if (file.fail())
		return false;

	// Go to the vertex count
	file.get(input);
	while (input != ':')
		file.get(input);

	// Get the vertex count
	file >> m_VertexCount;

	// Set the index count
	m_IndexCount = m_VertexCount;

	// Create the model
	m_Model = new ModelType[m_VertexCount];
	if (!m_Model)
		return false;

	// Go to the data
	file.get(input);
	while (input != ':')
		file.get(input);

	// Fill the model array
	i = 0;
	while (!file.eof())
	{
		file >>
			m_Model[i].position.x >> m_Model[i].position.y >> m_Model[i].position.z >>
			m_Model[i].texture.x >> m_Model[i].texture.y >>
			m_Model[i].normal.x >> m_Model[i].normal.y >> m_Model[i].normal.z;

		i++;
	}

	file.close();
	

	return true;
}

bool ModelClass::InitializeBuffers(ID3D10Device *device)
{
	HRESULT result;
	unsigned long *indices;
	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;


	// Create the indices array
	indices = new unsigned long[m_IndexCount];
	if (!indices)
		return false;

	// Fill the indices array
	for (int i = 0; i < m_IndexCount; i++)
		indices[i] = i;

	// Set up the description of the vertex buffer.
	ZeroMemory(&vertexBufferDesc, sizeof(D3D10_BUFFER_DESC));
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ModelType)* m_VertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_Model;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
		return false;

	// Set up the description of the vertex buffer.
	ZeroMemory(&indexBufferDesc, sizeof(D3D10_BUFFER_DESC));
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_IndexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	indexData.pSysMem = indices;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
		return false;

	delete[] indices;
	indices = 0;


	return true;
}

bool ModelClass::LoadTexture(ID3D10Device *device, char *textureFilename)
{
	// Create the texture object
	m_Texture = new TextureClass;
	if (!m_Texture)
		return false;

	// Initialize the texture object
	if (!m_Texture->Initialize(device, textureFilename))
		return false;


	return true;
}

void ModelClass::Render(ID3D10Device *device)
{
	unsigned int stride, offset;

	// Set stride and offset
	stride = sizeof(ModelType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it could be rendered
	device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it could be rendered
	device->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	return;
}

void ModelClass::Shutdown()
{
	ReleaseModel();

	ShutdownBuffers();

	ReleaseTexture();


	return;
}

void ModelClass::ReleaseModel()
{
	if (m_Model)
	{
		delete[] m_Model;
		m_Model = 0;
	}


	return;
}

void ModelClass::ShutdownBuffers()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}


	return;
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

unsigned long ModelClass::GetIndexCount()
{
	return m_IndexCount;
}

ID3D10ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}