////////////////////////////////////////////////////////////////////////////////
// Filename: terrain_vs.hlsl
////////////////////////////////////////////////////////////////////////////////
 
 
/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
 
 
//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal: NORMAL;
};
 
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal: NORMAL;
};
 
 
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TerrainVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to 4 units for proper matrix calculation.
    input.position.w = 1.0f;
 
    // Calculate the position of the vertices with respect to the world, view and projection matrix.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Save the texture coordinates of the pixel shader.
    output.tex = input.tex;
 
    // Compute the normal vector for the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    
    // Normalize the normal vector.
    output.normal = normalize(output.normal);
 
    return output;
}