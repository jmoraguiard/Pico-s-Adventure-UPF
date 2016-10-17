////////////////////////////////
// Filename: vertex_shader_3d.vs //
////////////////////////////////

/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 5

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer LightPositionBuffer
{
    float4 lightPosition[NUM_LIGHTS];
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float4 worldPos : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	float3 lightPos1 : TEXCOORD1;
    float3 lightPos2 : TEXCOORD2;
	float3 lightPos3 : TEXCOORD3;
	float3 lightPos4 : TEXCOORD4;
	float3 lightPos5 : TEXCOORD5;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VertexShader3D(VertexInputType input)
{
    PixelInputType output;
	float4 worldPosition;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
	// Store the input texel for the pixel shader to use.
	output.tex = input.tex;
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
	
	// Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

     // Determine the light positions based on the position of the lights and the position of the vertex in the world.
    output.lightPos1.xyz = lightPosition[0].xyz - worldPosition.xyz;
    output.lightPos2.xyz = lightPosition[1].xyz - worldPosition.xyz;
	output.lightPos3.xyz = lightPosition[2].xyz - worldPosition.xyz;
	output.lightPos4.xyz = lightPosition[3].xyz - worldPosition.xyz;
	output.lightPos5.xyz = lightPosition[4].xyz - worldPosition.xyz;
    
    return output;
}