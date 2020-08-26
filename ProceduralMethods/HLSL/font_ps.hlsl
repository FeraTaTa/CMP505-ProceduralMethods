////////////////////////////////////////////////////////////////////////////////
// Filename: font.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture;
SamplerState SampleType;

cbuffer PixelBuffer
{
	float4 pixelColor;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FontPixelShader(PixelInputType input) : SV_TARGET
{
	// �� ��ġ���� �ؽ�ó �ȼ��� ���ø��մϴ�.
	float4 color = shaderTexture.Sample(SampleType, input.tex);
	
	// �ؽ�ó�� ������ ���� ���̸��� �ȼ��� �������� ó���մϴ�.
	if(color.r == 0.0f)
	{
		color.a = 0.0f;
	}
	
	// �ؽ�ó�� ���� ���� �ƴ� ��� �۲��� �ȼ��̹Ƿ� �۲� �ȼ� ������ ����Ͽ� �׸��ϴ�.
	else
	{
		color.a = 1.0f;
		color = color * pixelColor;
	}

	return color;
}
