////////////////////////////////////////////////////////////////////////////////
// Filename: terrain_ps.hlsl
////////////////////////////////////////////////////////////////////////////////
 
 
/////////////
// GLOBALS //
/////////////
SamplerState SampleType;
 
cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float padding;
};
 
 
//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal: NORMAL;
};
 
 
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
    float3 lightDir;
    float lightIntensity;
    float4 color;
 
 
    // Set the default output color to the ambient light value for all pixels.
    color =  ambientColor;
 
    // Reverse the light direction for calculation.
    lightDir = -lightDirection;
 
    // Calculate the amount of light for this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));
 
    if(lightIntensity > 0.0f)
    {
        // Determine the final diffuse color based on the amount of diffuse color and light intensity.
        color += (diffuseColor * lightIntensity);
    }
 
    // Fill the final light color.
    color = saturate(color);
 
    return color;
}