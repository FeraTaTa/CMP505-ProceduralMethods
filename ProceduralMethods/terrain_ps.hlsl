////////////////////////////////////////////////////////////////////////////////
// Filename: terrain_ps.hlsl
////////////////////////////////////////////////////////////////////////////////
 
 
/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
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
    float2 tex : TEXCOORD0;
    float3 normal: NORMAL;
};
 
 
////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TerrainPixelShader(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;
 
 
    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);
 
    // Set the default output color to the ambient light value for all pixels.
    color  =  ambientColor;
 
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
 
    // Multiply the texture pixel by the final bright color to get the result.
    color = color * textureColor;
 
    return color;
}
