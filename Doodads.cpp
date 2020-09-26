#include "Doodads.h"
#include "RendererManager.h"
#include "IPP.h"

using ipp::memory_cast;

Doodads::Doodads()
{
	m_vertexBuffer = nullptr;
	m_texture = nullptr;
	m_deviceContext = nullptr;

	m_boundingSphere.Center = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_boundingSphere.Radius = 0.0f;

	m_destroyed = false;

	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

Doodads::~Doodads()
{
	if (m_vertexBuffer)
	{
		delete m_vertexBuffer;
		m_vertexBuffer = nullptr;
	}
}

void Doodads::Initialize(
	ID3D11Device * device,
	ID3D11DeviceContext * deviceContext,
	class Shader * shader,
	WCHAR * paths,
	const float size,
	const float collision,
	const XMFLOAT3 position
	)
{

	m_size = size;

	m_vertexBuffer = new class VertexBuffer();
	float sizexy[2] = { m_size,m_size };
	(void)m_vertexBuffer->Initialize(device, shader, sizexy, true);

	if (paths != NULL)
	{
		std::wstring tmp0 = std::wstring(paths);
		std::string tmp1 = std::string(tmp0.begin(), tmp0.end());
		m_texture = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	
	m_deviceContext = deviceContext;

	m_boundingSphere.Radius = collision;
	m_boundingSphere.Center = position;
	m_boundingSphere.Center.x += ((((float)rand()) / (float)RAND_MAX) * 2.0f) - 1.0f;
	m_boundingSphere.Center.y += ((((float)rand()) / (float)RAND_MAX) * 2.0f) - 1.0f;


	m_type = EObject::EObjectType::DOODADS;

	

}

void Doodads::Render(
	struct ID3D11DeviceContext * const deviceContext,
	const struct XMFLOAT4X4& viewMatrix,
	const struct XMFLOAT4X4& projectionMatrix,
	const struct ShaderPackage &shader
)
{
	
	if (m_flags.m_rendering)
	{
		//if (m_flags.m_selectable && m_flags.m_selected)
		//{
		//
		//	shader.BeginSelect();
		//
		//	class Shader* const csh = shader.select;
		//
		//	csh->SetShaderParameters(deviceContext, m_texture->GetTexture());
		//	csh->SetShaderParameters(deviceContext, m_worldMatrix, viewMatrix, projectionMatrix);
		//	m_vertexBuffer->Render(deviceContext);
		//
		//	//shader.End();
		//	shader.BeginStandard();
		//}

		class Shader* const csh = shader.standard;

		csh->SetShaderParameters(deviceContext, m_texture->GetTexture());
		csh->SetShaderParameters(deviceContext, m_worldMatrix, viewMatrix, projectionMatrix);
		//csh->SetShaderColorParameters(deviceContext, m_colors);
		m_vertexBuffer->Render(deviceContext);
	}
}

void Doodads::PreRender(
	struct ID3D11DeviceContext * const deviceContext,
	const struct DirectX::XMFLOAT4X4 & viewMatrix,
	const struct DirectX::XMFLOAT4X4 & projectionMatrix,
	const struct ShaderPackage & shader
)
{
	if (m_flags.m_rendering)
	{
		if (m_flags.m_cast_shadow)
		{
			//shader.standard->End(deviceContext);

			//shader.shadow->Begin(deviceContext);
			const __m128 cameraPosition = Camera::GetCurrentCamera()->GetPosition();

			__m128 distance{};
			distance.m128_f32[0] = cameraPosition.m128_f32[1] - m_boundingSphere.Center.y;
			distance.m128_f32[1] = cameraPosition.m128_f32[0] - m_boundingSphere.Center.x;

			float rotation = atan2(distance.m128_f32[0], distance.m128_f32[1]);

			DirectX::XMMATRIX rotationMatrix = XMMatrixRotationZ(rotation + XM_PIDIV2);

			float length = XMVector2Length(distance).m128_f32[0];

			rotationMatrix = rotationMatrix * XMLoadFloat4x4(&m_worldMatrix);
			XMFLOAT4X4 shadowMatrix;
			XMStoreFloat4x4(&shadowMatrix, rotationMatrix);
			shader.shadow->SetShaderParameters(deviceContext, m_texture->GetTexture());
			shader.shadow->SetShaderParameters(deviceContext, shadowMatrix, viewMatrix, projectionMatrix);
			m_vertexBuffer->Render(deviceContext);
			//shader.shadow->End(deviceContext);

			//shader.standard->Begin(deviceContext);
		}
	}
}

void Doodads::Update(float dt)
{

	m_flags.m_rendering = validateRendering(m_boundingSphere.Center);
	if (m_flags.m_rendering)
	{
		XMStoreFloat4x4(&m_worldMatrix, XMMatrixTranslation(m_boundingSphere.Center.x, m_boundingSphere.Center.y + (m_size / 1.5f), m_boundingSphere.Center.z - (m_size / 1.5f)));
	}

}

void Doodads::SetZ(float z)
{
	m_boundingSphere.Center.z = z;
}

void Doodads::Release()
{
	if (m_vertexBuffer)
	{
		delete m_vertexBuffer;
		m_vertexBuffer = nullptr;
	}
}

int32 Doodads::isReleased() const noexcept
{
	if (m_vertexBuffer)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void _stdcall Doodads::Intersect(class EObject* const other)
{
}

const RenderLayerType Doodads::GetLayerType() const noexcept
{
	if (m_destroyed)
		return RenderLayerType::ENUM_OBJECT_TYPE;
	return RenderLayerType::ENUM_OBJECT_TYPE;
}
