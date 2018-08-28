#pragma once
#include "Sprite.h"

class AnimatedSprite : public Sprite
{
public:
	AnimatedSprite(float size,float framesPerSecond,float animationSpeed = 1.0f,bool isLooping = true);

	virtual ~AnimatedSprite(void);

	void Initialize(ID3D11Device* device,ID3D11DeviceContext* deviceContext, Shader* shader, WCHAR* textureFileName);

	void Update();
	void Render(ID3D11DeviceContext* deviceContext, XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);
	void SetAnimationSpeed(float speed = 1.0f) override;

private:
	ID3D11DeviceContext* m_deviceContext;
	float m_currentFrame;
	float m_previousFrame;
	float m_maxFrames;
	float m_animationSpeed;
	float m_currentSpeed;
	float m_framesPerSecond;
	bool m_isLooping;
};

