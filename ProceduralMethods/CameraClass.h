#pragma once

class CameraClass : public AlignedAllocationPolicy<16>
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(XMFLOAT3);
	void SetRotation(XMFLOAT3);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX&);

private:
	XMFLOAT3 m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMMATRIX m_viewMatrix;
};