#pragma once
#include "VertexBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Font.h"

class LetterSprite
{
public:

	LetterSprite(TextFont* font, char letter, float size,Shader* shader);

	virtual ~LetterSprite(void);

	void Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Shader* shader, TextFont* font);

	void Render(
		struct ID3D11DeviceContext* const deviceContext,
		DirectX::XMFLOAT4X4& worldMatrix,
		DirectX::XMFLOAT4X4& viewMatrix,
		DirectX::XMFLOAT4X4& projectionMatrix
	);

	void Update();

private:
	ID3D11DeviceContext * m_deviceContext;

	class VertexBuffer* m_vertexBuffer;
	class Texture*      m_texture;
	class Shader*       m_shader;
	float         m_size;
	char          m_char;
};

