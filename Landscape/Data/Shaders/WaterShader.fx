/////////////
// GLOBALS //
/////////////
matrix worldMatrix;
matrix viewMatrix;
matrix projectionMatrix;

matrix reflectionMatrix;
float reflectRefractScale;

Texture2D reflectionTexture;
Texture2D refractionTexture;
Texture2D normalTexture;

float waterTranslation;

float3 cameraPosition;
float3 lightDirection;
float4 specularColor;
float specularPower;


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
    float2 tex : TEXCOORD0;
	float3 normal :NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal :NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float4 reflectionPosition : TEXCOORD1;
	float4 refractionPosition : TEXCOORD2;
	float3 viewDirection : TEXCOORD3;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType WaterVertexShader(VertexInputType input)
{
	PixelInputType output;
    matrix reflectProjectWorld;
    matrix viewProjectWorld;
	float4 worldPosition;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Create the reflection projection world matrix.
    reflectProjectWorld = mul(reflectionMatrix, projectionMatrix);
    reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

    // Calculate the input position against the reflectProjectWorld matrix.
    output.reflectionPosition = mul(input.position, reflectProjectWorld);

	// Create the view projection world matrix for refraction.
    viewProjectWorld = mul(viewMatrix, projectionMatrix);
    viewProjectWorld = mul(worldMatrix, viewProjectWorld);
   
    // Calculate the input position against the viewProjectWorld matrix.
    output.refractionPosition = mul(input.position, viewProjectWorld);
	
	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	
	output.tangent = mul(input.tangent, (float3x3)worldMatrix);
	output.tangent = normalize(output.tangent);
	
	output.binormal = mul(input.binormal, (float3x3)worldMatrix);
	output.binormal = normalize(output.binormal);

	worldPosition = mul(input.position, worldMatrix);
	output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	output.viewDirection = normalize(output.viewDirection);


    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 WaterPixelShader(PixelInputType input) : SV_Target
{
    float2 reflectTexCoord;
    float2 refractTexCoord;
    float4 normalMap;
    float3 normal;
    float4 reflectionColor;
    float4 refractionColor;
    float4 color;
	float fresnelTerm;
	float4 combinedColor;
	float4 dullColor;
	float lightIntensity;
	float3 bumpNormal;
	float3 reflection;
	float4 specular;

	// Move the position the water normal is sampled from to simulate moving water.	
    input.tex.y += waterTranslation;

	// Calculate the projected reflection texture coordinates.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

    // Calculate the projected refraction texture coordinates.
    refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
    refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

	// Sample the normal from the normal map texture.
    normalMap = normalTexture.Sample(SampleType, input.tex*6);

    // Expand the range of the normal from (0,1) to (-1,+1).
    normal = (normalMap.xyz * 2.0f) - 1.0f;

	// Re-position the texture coordinate sampling position by the normal map value to simulate the rippling wave effect.
    reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
    refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);

	// Sample the texture pixels from the textures using the updated texture coordinates.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
    refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);

	// calculates the angle between the normal and the view direction.
	fresnelTerm = dot(input.viewDirection, float3(0.0f, 1.0f, 0.0f));

	// lerps the reflected and refracted color by the fresnel term
	combinedColor = lerp(reflectionColor, refractionColor, fresnelTerm);

	// creates a dull color to add to the water final color
	dullColor = float4(0.3f, 0.3f, 0.5f, 1.0f);	

	// Combine the reflection and refraction results for the final color.
    color = lerp(combinedColor, dullColor, 0.2f);

	// Calculate the normal from the data in the bump map.
    bumpNormal = input.normal + normal.x * input.tangent + normal.y * input.binormal;

	// Normalize the resulting bump normal.
    bumpNormal = normalize(bumpNormal);

	// Calculate the amount of light on this pixel based on the bump map normal value.
    lightIntensity = saturate(dot(bumpNormal, -lightDirection));

	if(lightIntensity > 0.0f)
    {
		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
        //reflection = normalize(2 * lightIntensity * bumpNormal + lightDirection); 
		reflection = -reflect(lightDirection,bumpNormal);

        // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
        //specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
		specular = dot(normalize(reflection),normalize(input.viewDirection));

		// Use the specular map to determine the intensity of specular light at this pixel.
        //specular = specular * specularPower;
		specular = pow(specular,256);

		// Adds the specular power color
		specular *= specularColor;
		
        // Add the specular component last to the output color.
        color = saturate(color + specular);
    }
	
    return color;
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 WaterTechnique
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_4_0, WaterVertexShader()));
		SetPixelShader(CompileShader(ps_4_0, WaterPixelShader()));
		SetGeometryShader(NULL);
	}
}



