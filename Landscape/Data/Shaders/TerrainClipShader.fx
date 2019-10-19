/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D texturesArray[4];
float4 diffuseColor;
float4 ambientColor;
float3 lightDirection;
float4 clipPlane;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float4 texWeight : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL;
	float4 texWeight : COLOR;
	
	float clip : SV_ClipDistance0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TerrainClipVertexShader(VertexInputType input)
{
    PixelInputType output;    
    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the input color for the pixel shader to use.
    output.texCoord = input.texCoord;

	output.normal = mul(input.normal,(float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	output.texWeight = input.texWeight;
	
	// Set the clipping plane.
    output.clip = dot(mul(input.position, worldMatrix), clipPlane);
    
	return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TerrainClipPixelShader(PixelInputType input) : SV_Target
{
    float4 textureColor;
	float3 lightDir;
    float lightIntensity;
    float4 color;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = texturesArray[0].Sample(SampleType, input.texCoord) * input.texWeight.x;
	textureColor += texturesArray[1].Sample(SampleType, input.texCoord) * input.texWeight.y;
	textureColor += texturesArray[2].Sample(SampleType, input.texCoord) * input.texWeight.z;
	textureColor += texturesArray[3].Sample(SampleType, input.texCoord) * input.texWeight.w;

	color = ambientColor;

	float depth = input.position.z/input.position.w;
	float blendDistance = 0.99f;
	float blendWidth = 0.005f;
	float blendFactor = clamp((depth-blendDistance)/blendWidth, 0, 1);
         
	float4 farColor = textureColor;     
	float4 nearColor;
	float2 nearTextureCoords = input.texCoord*6;
	nearColor = texturesArray[0].Sample(SampleType, nearTextureCoords) * input.texWeight.x;
	nearColor += texturesArray[1].Sample(SampleType, nearTextureCoords) * input.texWeight.y;
	nearColor += texturesArray[2].Sample(SampleType, nearTextureCoords) * input.texWeight.z;
	nearColor += texturesArray[3].Sample(SampleType, nearTextureCoords) * input.texWeight.w;
 
	textureColor = lerp(nearColor, farColor, blendFactor);

	// Invert the light direction for calculations.
	lightDir = -lightDirection;

	// Calculate the amount of light on this pixel.
	// The dot product is a value expressing the angular relationship between two vectors
	lightIntensity = saturate(dot(input.normal,lightDir));

	if(lightIntensity > 0.0f)
	{
		color += (diffuseColor*lightIntensity);
	}

	 // Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
	color = saturate(color);

	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color*(textureColor*0.8f);

    return color;
}


////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 TerrainClipTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TerrainClipVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, TerrainClipPixelShader()));
        SetGeometryShader(NULL);
    }
}