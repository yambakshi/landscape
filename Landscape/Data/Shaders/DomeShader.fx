/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;
Texture2D shaderTexture;
float skyTranslation;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType
{
	Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

//////////////////
// BLEND STATES //
//////////////////
BlendState AlphaBlendingOn
{
	BlendEnable[0] = TRUE;	
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
	float2 tex : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType DomeVertexShader(VertexInputType input)
{
    PixelInputType output; 

    
	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the input color for the pixel shader to use.
    output.tex = input.tex;
    
	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 DomePixelShader(PixelInputType input) : SV_Target
{	
	float4 textureColor;

	// Move the position the water normal is sampled from to simulate moving water.	
    input.tex.y += skyTranslation;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    return textureColor;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 DomeTechnique
{
    pass pass0
    {
		//SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
        SetVertexShader(CompileShader(vs_4_0, DomeVertexShader()));
        SetPixelShader(CompileShader(ps_4_0, DomePixelShader()));
        SetGeometryShader(NULL);
    }
}