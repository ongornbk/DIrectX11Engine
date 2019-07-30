#pragma once
#include "gdef.h"
#include "TaskQueue.h"
#include "ShaderPackage.h"
#include <DirectXCollision.h>

enum EObjectAnchor
{
	BOTTOM_CENTRE,
	BOTTOM_LEFT,
	BOTTOM_RIGHT,
	TOP_LEFT,
	TOP_CENTRE,
	TOP_RIGHT,
	RIGHT,
	LEFT,
	CENTRE
};

struct EObjectFlags
{
	bool m_rendering = false;
	bool m_selected = false;
	bool m_pushable = false;
	bool m_blocked = false;
	bool m_collided = false;
	bool m_selectable = false;
	bool m_cast_shadow = false;
	bool m_hide = false;
};

class EObject
{
public:

	virtual void            Render(
		struct ID3D11DeviceContext* const deviceContext,
		const struct XMFLOAT4X4& viewMatrix,
		const struct XMFLOAT4X4& projectionMatrix,
		const struct ShaderPackage &shader
	) = 0;
	virtual void            SetZ(const float z = 0.f) = 0;
	virtual void            Update(const float dt = 0.f) = 0;
	virtual void            Release() = 0;


public:
	uint32           m_index;
	uint32           m_vector;



	int32 collisionPriority = 0;

	DirectX::BoundingSphere m_boundingSphere;

	EObjectFlags m_flags;

	enum EObjectType
	{
		UNIT,
		DOODADS,
		ANIMATED_DOODADS,
		TREE
	}m_type;
};

