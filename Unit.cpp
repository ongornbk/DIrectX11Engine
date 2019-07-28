#include "Unit.h"
#include "Engine.h"
#include "S_ModelPaths.h"
#include "RendererManager.h"
#include "IPP.h"
#include "Math.h"

Unit::Unit()
{
	m_floats[0] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_floats[1] = XMFLOAT3(0.0f, 0.0f, 0.0f);

	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	m_modelVariant.SetVariant(MS_TOWNNEUTRAL);
	m_vertexBuffer = nullptr;
	m_rotation = DEFAULT_ROTATION;
	m_isLooping = true;
	m_animationSpeed = 0.20f;
	m_framesPerSecond = 1.0f;
	m_currentSpeed = 0.0f;
	m_stop = false;
	m_rotations = 16.0f;

	m_tasks.SetOwner(this);
}


Unit::~Unit()
{
	if (m_vertexBuffer)
	{
		delete m_vertexBuffer;
		m_vertexBuffer = NULL;
	}
}

void Unit::Initialize(
	ID3D11Device * device,
	ID3D11DeviceContext * deviceContext,
	class Shader * shader,
	WCHAR* paths,
	const float modelsize,
	const float collision,
	const XMFLOAT3 position,
	const bool wander)
{
	wstring tmp0 = wstring(paths);
	string  tmp1 = string(tmp0.begin(), tmp0.end());
	ModelPaths* ptr = S_ModelPaths::GetModelPaths(tmp1);



	m_size = modelsize;
	m_lastSize = modelsize;
	m_boundingSphere.Radius = collision;
	m_boundingSphere.Center = position;
	m_boundingSphere.Center.x += ((((float)rand()) / (float)RAND_MAX) * 2.0f) - 1.0f;
	m_boundingSphere.Center.y += ((((float)rand()) / (float)RAND_MAX) * 2.0f) - 1.0f;

	InitializeModel(device, deviceContext, shader, ptr);
	m_wanderingFlag = wander;
	m_type = EObject::EObjectType::UNIT;
}

void Unit::Render(ID3D11DeviceContext * deviceContext, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, ShaderPackage &shader)
{
	if (m_flags.m_rendering)
	{

		if (m_flags.m_cast_shadow)
		{
			shader.standard->End(deviceContext);
			shader.shadow->Begin(deviceContext);

			const __m128 cameraPosition = Camera::GetCurrentCamera()->GetPosition();

			//__m128 distance{};
			//distance.m128_f32[0] = cameraPosition.m128_f32[1] - m_boundingSphere.Center.y;
			//distance.m128_f32[1] = cameraPosition.m128_f32[0] - m_boundingSphere.Center.x;

			//float rotation = atan2(distance.m128_f32[0], distance.m128_f32[1]);

			DirectX::XMMATRIX rotationMatrix = XMMatrixRotationZ(-0.8f);

			//float length = XMVector2Length(distance).m128_f32[0];

			rotationMatrix = rotationMatrix * XMLoadFloat4x4(&m_worldMatrix);
			XMFLOAT4X4 shadowMatrix;
			XMStoreFloat4x4(&shadowMatrix, rotationMatrix);
			shader.shadow->SetShaderParameters(deviceContext, m_modelVariant.GetTexture());
			shader.shadow->SetShaderParameters(deviceContext, shadowMatrix, viewMatrix, projectionMatrix);
			m_vertexBuffer->Render(deviceContext);

			shader.shadow->End(deviceContext);
			shader.standard->Begin(deviceContext);
		}

		if (m_flags.m_selectable && m_flags.m_selected)
		{
			shader.standard->End(deviceContext);
			shader.select->Begin(deviceContext);

			class Shader* const csh = shader.select;

			csh->SetShaderParameters(deviceContext, m_modelVariant.GetTexture());
			csh->SetShaderParameters(deviceContext, m_worldMatrix, viewMatrix, projectionMatrix);
			m_vertexBuffer->Render(deviceContext);

			shader.select->End(deviceContext);
			shader.standard->Begin(deviceContext);
		}

		class Shader* const csh = shader.standard;
		
		csh->SetShaderParameters(deviceContext, m_modelVariant.GetTexture());
		csh->SetShaderParameters(deviceContext, m_worldMatrix, viewMatrix, projectionMatrix);
		m_vertexBuffer->Render(deviceContext);
	}

}



void Unit::Update(const float dt)
{

	
	m_flags.m_rendering = validateRendering(m_boundingSphere.Center);

	if (!m_flags.m_blocked)
	{

		if (m_tasks.Update())
		{

		}
		else
		{
			if (m_wanderingFlag)
			{
				m_tasks.Wander(this);
			}
			else
			{
				SetAnimation(ModelStance::MS_TOWNNEUTRAL);
				SetVelocity(0.0f, 0.0f, 0.0f);
				Unit::EndRunning();
			}
		}
		
			

		if (!m_stop)
		{
			m_flags.m_collided = TileMap::CollisionAt(m_boundingSphere.Center);
			if (m_flags.m_collided)
			{
				m_boundingSphere.Center = m_floats[1];
			}
			else
			{
				m_floats[1] = m_boundingSphere.Center;
			}
				XMFLOAT3 niuPos = XMFloat3Sum(m_boundingSphere.Center, XMFloat3Multiply(m_floats[0], dt));

				m_flags.m_collided = TileMap::CollisionAt(niuPos);

				if (!m_flags.m_collided)
				{
					m_boundingSphere.Center = niuPos;
				}
		}

		if (m_flags.m_rendering)
		{

			XMStoreFloat4x4(
				&m_worldMatrix,
				XMMatrixTranslation(
					m_boundingSphere.Center.x,
					m_boundingSphere.Center.y + (m_size / 1.5f),
					m_boundingSphere.Center.z - (m_size / 1.5f)
				)
			);


			if (m_modelVariant.GetMaxFrames() == 1.0f) return;
			if (m_currentFrame < m_modelVariant.GetMaxFrames())
			{
				m_currentSpeed += m_animationSpeed - dt;

				if (m_currentSpeed > m_framesPerSecond)
				{
					m_currentFrame++;
					m_currentSpeed = 0.0f;
					if (m_currentFrame >= m_modelVariant.GetMaxFrames())
					{
						if (m_isLooping)
						{
							m_currentFrame = 0.0f;
						}
						else
						{
							m_stop = false;
							m_isLooping = true;
							m_currentFrame = 0.0f;
							m_previousFrame = -1.0f;
							m_modelVariant.SetVariant(m_stopped);
						}
					}

				}
			}
			if (m_currentFrame == m_previousFrame) return;





			D3D11_MAPPED_SUBRESOURCE mappedResource;
			struct SpriteVertexType* vertices = m_vertexBuffer->GetVertices();

			vertices[0].uv.x = m_currentFrame / m_modelVariant.GetMaxFrames();
			vertices[0].uv.y = (m_rotation + 1.0f) / m_rotations;

			vertices[1].uv.x = m_currentFrame / m_modelVariant.GetMaxFrames();
			vertices[1].uv.y = m_rotation / m_rotations;

			vertices[2].uv.x = (m_currentFrame + 1.0f) / m_modelVariant.GetMaxFrames();
			vertices[2].uv.y = m_rotation / m_rotations;

			vertices[3].uv.x = (m_currentFrame + 1.0f) / m_modelVariant.GetMaxFrames();
			vertices[3].uv.y = (m_rotation + 1.0f) / m_rotations;



			HRESULT result = m_deviceContext->Map(m_vertexBuffer->GetVertexBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result))
			{
				return;
			}

			struct SpriteVertexType* verticesPtr = (SpriteVertexType*)mappedResource.pData;
			memcpy(verticesPtr, (void*)vertices, sizeof(SpriteVertexType) * m_vertexBuffer->GetVertexCount());
			m_deviceContext->Unmap(m_vertexBuffer->GetVertexBuffer(), 0);

			m_previousFrame = m_currentFrame;

		}




		int32 mousePosition[2];
		UserInterface::GetMousePosition(mousePosition[0], mousePosition[1]);
		FXMVECTOR point = XMVectorSet((float)mousePosition[0], (float)mousePosition[1], 0.0f, 0.0f);
		if (m_boundingSphere.Contains(point))
		{
			m_flags.m_selected = true;
			GLOBAL m_lastSelectedUnit = this;
		}
		else
		{
			m_flags.m_selected = false;
		}
	}
}	


void Unit::SetZ(const float z)
{
	m_boundingSphere.Center.z = z;
}

void Unit::SetTask(class Task* task)
{
	m_tasks.SetTask(task);
}

void Unit::GiveTask(class Task * task)
{
	m_tasks.QueueTask(task);
}

void Unit::Release()
{
	delete this;
}

float Unit::GetCollisionRadius() const noexcept
{
	return m_boundingSphere.Radius;
}


XMFLOAT3 Unit::GetPosition() const noexcept
{
	return m_boundingSphere.Center;
}

float Unit::GetSpeed() const noexcept
{
	return m_speed[0];
}

float Unit::GetZ() const noexcept
{
	return m_boundingSphere.Center.z;
}

void Unit::SetSpeed(const float speed)
{
	m_speed[0] = speed;
	m_speed[1] = speed;
}

Unit::WalkingStance Unit::GetWalkingStance() const noexcept
{
	return m_walkingStance;
}

void Unit::SetWalkingStance(const enum WalkingStance stance)
{
	this->m_walkingStance = stance;
	switch (stance)
	{
	case WS_WALK:
		m_speed[0] = m_speed[1] / 2.0f;
		break;
	case WS_RUN: 
		m_speed[0] = m_speed[1];
		break;
	}
}

void Unit::ChangeWalkingStance()
{
	switch (m_walkingStance)
	{
	case Unit::WalkingStance::WS_RUN:
			m_walkingStance = Unit::WalkingStance::WS_WALK;
			m_speed[0] = m_speed[1] / 2.0f;
			break;
	case Unit::WalkingStance::WS_WALK:
		m_walkingStance = Unit::WalkingStance::WS_RUN;
		m_speed[0] = m_speed[1];
		break;
	}
}

void Unit::SetRotations(const int32 rotations)
{
	(this->m_rotation) = (float)(rotations % (int32_t)m_rotations);
}

void Unit::SetRotation(float rotation)
{
	m_rotation = (float)(( int32)rotation % ( int32)m_rotations);
}

void Unit::SetVelocity(const float x,const float y,const float z)
{
	m_floats[0] = { x,y,z };
}

void Unit::DiscardTasks()
{
	m_tasks.Discard();
}

void Unit::SetPosition(const struct XMFLOAT3 position)
{
	m_boundingSphere.Center = position;
}

void Unit::GoBack()
{
	m_boundingSphere.Center = m_floats[1];
}

void Unit::SetFootstepsSound(class Sound * sound)
{
	m_footstepsSound = sound;
}

void Unit::BeginRunning()
{
	if (m_footstepsSound)
	{
		if (m_footstepsHandle)
		{
			//m_footstepsHandle->stop();
			//m_footstepsHandle = m_footstepsSound->StartPlaying();
		}
		else
		{
			m_footstepsHandle = m_footstepsSound->StartPlaying();
		}
	}
}

void Unit::EndRunning()
{
	if (m_footstepsHandle)
	{
		m_footstepsHandle->stop();
		m_footstepsHandle = nullptr;
	}
}

class Sound * Unit::GetFootstepsSound() const noexcept
{
	return m_footstepsSound;
}

void Unit::Resize(ID3D11Device * device, Shader * shader,const float resize)
{
	m_size *= resize;
	float sizexy[2] = { m_size,m_size };
	(void)m_vertexBuffer->Initialize(device, shader, sizexy, true);
}

void Unit::PlayAnimation(const enum ModelStance animation)
{
	if (m_flags.m_rendering&&!m_stop)
	{
		m_currentFrame = 0.0f;
		m_previousFrame = -1.0f;
		m_stopped = m_modelVariant.GetVariant();
		m_modelVariant.SetVariant(animation);
		m_isLooping = false;
		m_stop = true;
	}
}

void Unit::SetAnimation(const enum ModelStance animation)
{
	if (m_flags.m_rendering && !m_stop&&(animation!=m_modelVariant.GetVariant()))
	{
		m_currentFrame = 0.0f;
		m_previousFrame = -1.0f;
		m_modelVariant.SetVariant(animation);
	}
}

void Unit::SetAnimationSpeed(const float speed)
{
	m_previousSpeed = m_animationSpeed;
	m_animationSpeed = speed;
}

void Unit::InitializeModel(ID3D11Device * device, ID3D11DeviceContext * deviceContext, Shader * shader, ModelPaths * paths)
{
	m_vertexBuffer = new VertexBuffer();
	float sizexy[2] = { m_size,m_size };
	(void)m_vertexBuffer->Initialize(device, shader, sizexy, true);


	if (paths->TOWNWALK != NULL)
	{
		wstring tmp0 = wstring(paths->TOWNWALK);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[0] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}

	if (paths->TOWNNEUTRAL != NULL)
	{
		wstring tmp0 = wstring(paths->TOWNNEUTRAL);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[1] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}

	if (paths->SPECIALCAST != NULL)
	{
		wstring tmp0 = wstring(paths->SPECIALCAST);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[2] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}

	if (paths->GETHIT != NULL)
	{
		wstring tmp0 = wstring(paths->GETHIT);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[3] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->KICK != NULL)
	{
		wstring tmp0 = wstring(paths->KICK);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[4] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->SPECIAL_1 != NULL)
	{
		wstring tmp0 = wstring(paths->SPECIAL_1);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[5] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->WALK != NULL)
	{
		wstring tmp0 = wstring(paths->WALK);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[6] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->ATTACK_1 != NULL)
	{
		wstring tmp0 = wstring(paths->ATTACK_1);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[7] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->NEUTRAL != NULL)
	{
		wstring tmp0 = wstring(paths->NEUTRAL);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[8] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->ATTACK_2 != NULL)
	{
		wstring tmp0 = wstring(paths->ATTACK_2);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[9] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->RUN != NULL)
	{
		wstring tmp0 = wstring(paths->RUN);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[10] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->SPECIAL_3 != NULL)
	{
		wstring tmp0 = wstring(paths->SPECIAL_3);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[11] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}
	if (paths->SPECIAL_4 != NULL)
	{
		wstring tmp0 = wstring(paths->SPECIAL_4);
		string tmp1 = string(tmp0.begin(), tmp0.end());
		m_modelVariant.m_textures[12] = ResourceManager::GetInstance()->GetTextureByName((char*)tmp1.c_str());
	}



	m_deviceContext = deviceContext;
	m_currentFrame = 0.0f;
	m_previousFrame = -1.0f;
}



