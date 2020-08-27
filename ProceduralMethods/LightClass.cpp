#include "stdafx.h"
#include "LightClass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetAmbientColor(XMFLOAT4 color)
{
	m_ambientColor = color;
}


void LightClass::SetDiffuseColor(XMFLOAT4 color)
{
	m_diffuseColor = color;
}


void LightClass::SetDirection(XMFLOAT3 direction)
{
	m_direction = direction;
}


XMFLOAT4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}


XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


XMFLOAT3 LightClass::GetDirection()
{
	return m_direction;
}