#include "RendererManager.h"
#include "IPP.h"
#include "SettingsC.h"
#include "Defines.h"
#include <future>
#include <mutex>
#include <stack>

#pragma region
class Unit;
#define UnitPtr Unit*;
#pragma endregion

namespace
{
	RendererManager*              m_instance;
	Engine*                       m_engine;
	static XMVECTOR               m_cameraPosition;
	static float                  m_ranges[2];
	static atomic<int>            m_coord[2];
	atomic<uint8_t>               m_async;
	static RenderContainerVector  g_units;
}

RendererManager::RendererManager(Engine* engine,Shader* units,Shader* ui)
{
	this->m_renderingStyle = RendererManager::RenderingStyle::REVERSE;
	this->m_engine = engine;
	this->m_unitsShader = units;
	this->m_shader = ui;




	m_instance = this;
	
	Tile::SetGlobals(Engine::GetEngine()->GetGraphics()->GetDevice(), GETSHADER "tile.fx" CLOSE,this);
	m_map = new TileMap(1.0f,1.0f,0.2f,true);
	m_map->Initialize();
	TileMap::SetCurrentTileMap(m_map);
	Tile::SetDeviceContext(Engine::GetEngine()->GetGraphics()->GetDeviceContext());
	m_ui = new UserInterface();
	m_ranges[0] = ((float)*(SETTINGS RESOLUTION_X) / 2.0f)+300.0f;
	m_ranges[1] = ((float)*(SETTINGS RESOLUTION_Y) / 2.0f) + 300.0f;

}


RendererManager::~RendererManager()
{
	g_units.Clear();
	if (m_ui)
	{
		delete m_ui;
		m_ui = NULL;
	}
	if (m_map)
	{
		delete m_map;
		m_map = nullptr;
	}
}



void RendererManager::PushUnit(Unit * unit)
{
	g_units.Push(unit);
}

void RendererManager::PushDoodads(Doodads * doodads)
{
	g_units.Push(doodads);
}

void RendererManager::PushAnimatedDoodads(AnimatedDoodads * doodads)
{
	g_units.Push(doodads);
}

void RendererManager::PushTree(Tree * doodads)
{
	g_units.Push(doodads);
}

extern "C"
{
	struct __SortByY {
		bool operator()(RenderContainer* a, RenderContainer* b) const noexcept {

			BoundingSphere* bsa = a->GetBoundingSphere();
			BoundingSphere* bsb = b->GetBoundingSphere();

			bool af2 = a->Flag(2u);
			bool bf2 = b->Flag(2u);

			float radius[3];
			float ax, ay, bx, by, az, bz;

			radius[0] = bsa->Radius;
			radius[1] = bsb->Radius;
			ax = bsa->Center.x;
			ay = bsa->Center.y;
			az = bsa->Center.z;

			bx = bsb->Center.x;
			by = bsb->Center.y;
			bz = bsb->Center.z;

			if (radius[0] == 0.0f||radius[1] == 0.0f) goto RETURN;
			radius[2] = radius[0] + radius[1];
			float distance = 0.0f;
			float distanceX = 0.0f, distanceY = 0.0f;
			float collision = 0.0f;





				



			distanceX = ax - bx;
			distanceY = ay - by;
			distance = XMVector2Length({ distanceX,distanceY }).m128_f32[0];
			if (distance < radius[2])
			{
				collision = distance - radius[2];
				if (af2)
				{
					if (bf2)
					{
						collision /= 2.0f;
						if (ax < bx)
						{
							bsa->Center.x += collision;
							bsb->Center.x -= collision;
						}
						else
						{
							bsa->Center.x -= collision;
							bsa->Center.x += collision;

						}
						//a->UpdatePosition();
						//b->UpdatePosition();
					}
					else
					{
						if (ax < bx) bsa->Center.x += collision;
						else         bsa->Center.x -= collision;

						//a->UpdatePosition();
					}
				}
				else
				{
					if (bf2)
					{
						if (ax < bx) bsb->Center.x -= collision;
						else         bsb->Center.x += collision;


						//b->UpdatePosition();
					}

				}

			}
		RETURN:
			if (az != bz)
			{
				return az > bz;
			}
			return ay > by;
		}
	};

	struct  __SortByX {
		bool operator()(RenderContainer *a, RenderContainer *b) const noexcept {

			BoundingSphere* bsa = a->GetBoundingSphere();
			BoundingSphere* bsb = b->GetBoundingSphere();

			bool af2 = a->Flag(2u);
			bool bf2 = b->Flag(2u);

			float radius[3];
			float ax, ay, bx, by, az, bz;

			radius[0] = bsa->Radius;
			radius[1] = bsb->Radius;

			ax = bsa->Center.x;
			ay = bsa->Center.y;
			az = bsa->Center.z;

			bx = bsb->Center.x;
			by = bsb->Center.y;
			bz = bsb->Center.z;

			if (radius[0] == 0.0f|| radius[1] == 0.0f) goto RETURN;
			radius[2] = radius[0] + radius[1];
			float distance = 0.0f;
			float distanceX = 0.0f, distanceY = 0.0f;
			float collision = 0.0f;

			distanceX = ax - bx;
			distanceY = ay - by;
			distance = XMVector2Length({ distanceX,distanceY }).m128_f32[0];
			if (distance < radius[2])
			{
				collision = distance - radius[2];
				if (af2)
				{
					if (bf2)
					{
						collision /= 2.0f;
						if (ay < by)
						{
							bsa->Center.y += collision;
							bsb->Center.y -= collision;
						}
						else
						{
							bsa->Center.y -= collision;
							bsb->Center.y += collision;
						}
					}
					else
					{
						if (ay < by) bsa->Center.y += collision;
						else         bsa->Center.y -= collision;

					}
				}
				else
				{
					if (bf2)
					{
						if (ay < by) bsb->Center.y -= collision;
						else         bsb->Center.y += collision;
					}

				}

			}
		RETURN:
			if (az != bz)
			{
				return az > bz;
			}
			return ax > bx;
		}
	};

	void SortByY(std::vector<RenderContainer*> &vec) noexcept
	{
		std::sort(vec.begin(), vec.end(), __SortByY());
		m_async--;
	}

	void SortByX(std::vector<RenderContainer*> &vec) noexcept
	{
		std::sort(vec.begin(), vec.end(), __SortByX());
		m_async--;
	}

	inline void DoNothing() noexcept
	{
		//ipp::Console::Print("!");
	}



	bool _vectorcall validateRendering(XMFLOAT3 object) noexcept
	{
		float f[2] = { abs((object.x) - (m_cameraPosition.m128_f32[0])),abs((object.y) - (m_cameraPosition.m128_f32[1])) };

		if ((f[0] > m_ranges[0]) || (f[1] > m_ranges[1]))
		{
			return false;
		}
		else
		{
			return true;
		}

	}

}

	void RendererManager::Render(ID3D11DeviceContext * deviceContext, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
	{
		

			m_map->Render(deviceContext, viewMatrix, projectionMatrix, m_cameraPosition);

			GRAPHICS EnableAlphaBlending(true);

			m_unitsShader->Begin(deviceContext);
			
			g_units.Render(deviceContext, viewMatrix, projectionMatrix, m_unitsShader);
			
			m_unitsShader->End(deviceContext);


			m_shader->Begin(deviceContext);

			m_ui->Render(deviceContext, viewMatrix, projectionMatrix);


			m_shader->End(deviceContext);

			GRAPHICS EnableAlphaBlending(false);
}

	extern "C"
	{

		void _vectorcall UpdatePart(std::vector<RenderContainer*> vec,float dt) noexcept
		{
			RenderContainer** m_array = vec.data();
			for (uint32_t i = 0u; i < vec.size(); ++i)
			{
				m_array[i]->Update(dt);
			}
			m_async--;

		}
	}

void RendererManager::Update()
{
#pragma region
	using std::async;
	using std::vector;
	using std::advance;
	using std::sort;
#pragma endregion
	m_cameraPosition = CAMERA GetPosition();
	m_ui->Update(m_cameraPosition);
	float dt = ipp::Timer::GetDeltaTime();
	m_map->Update(dt);

	if (m_engine->GetGameStance() == false)
	{
		g_units.Update(dt);
	}

			
	g_units.Sort();	

}



void RendererManager::SetRenderingStyle(RenderingStyle render)
{
	m_renderingStyle = render;
}

void RendererManager::SetInterface(uint32_t type,Shader* shader)
{
	m_ui->SetScene(type, shader);
}

void RendererManager::SetTile(XMFLOAT2 position, int32_t tile)
{
	m_map->SetTile(position, tile);
}

void RendererManager::SaveInstanceToFile(std::string filename)
{
	m_map->SaveToFile(filename);
}

void RendererManager::LoadInstanceToFile(std::string filename)
{
	m_map->LoadFromFile(filename);
}

RendererManager * RendererManager::GetInstance()
{
	return m_instance;
}

size_t RendererManager::GetNumberOfObjects()
{
	return g_units.m_objects.size();
}

RenderContainerVector::RenderContainerVector()
{
	m_objects.reserve(20000u);
}

void RenderContainerVector::Update(float dt)
{
	//m_async = 4u;
	//size_t middle = m_objects.size() / 4u;
	//vector<RenderContainer*>::const_iterator onefour(m_objects.cbegin());
	//vector<RenderContainer*>::const_iterator twofour(m_objects.cbegin());
	//vector<RenderContainer*>::const_iterator threefour(m_objects.cbegin());
	//advance(onefour, middle);
	//advance(twofour, middle * 2u);
	//advance(threefour, middle * 3u);
	//vector<RenderContainer*> t1(m_objects.cbegin(), onefour);
	//vector<RenderContainer*> t2(onefour, twofour);
	//vector<RenderContainer*> t3(twofour, threefour);
	//vector<RenderContainer*> t4(threefour, m_objects.cend());
	//thread tt1(UpdatePart, t1, dt);
	//thread tt2(UpdatePart, t2, dt);
	//thread tt3(UpdatePart, t3, dt);
	//thread tt4(UpdatePart, t4, dt);
	////async(launch::async, UpdatePart, t1, dt);
	////async(launch::async, UpdatePart, t2, dt);
	////async(launch::async, UpdatePart, t3, dt);
	////async(launch::async, UpdatePart, t4, dt);
	////while (m_async)
	////{
	////	DoNothing();
	////}
	//tt1.join();
	//tt2.join();
	//tt3.join();
	//tt4.join();
	UpdatePart(m_objects, dt);
}

void RenderContainerVector::Sort()
{
	//m_async = 2u;
	//std::async(std::launch::async,SortByX,(m_objects));
	//std::async(std::launch::async,SortByY,(m_objects));
	//SortByX(m_objects);
   // SortByY(m_objects);
	//while (m_async)
	//{
	//	DoNothing();
	//}

	std::sort(m_objects.begin(), m_objects.end(), __SortByX());
	std::sort(m_objects.begin(), m_objects.end(), __SortByY());
}

static uint32_t sizeg = 0u;

void _vectorcall RenderContainerVector::Render(ID3D11DeviceContext * deviceContext, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, Shader * shader) noexcept
{
	RenderContainer** objects = m_objects.data();
	for (uint32_t i = 0; i < (uint32_t)m_objects.size(); i++)
	{
		objects[i]->Render(deviceContext, viewMatrix, projectionMatrix, shader);
		objects[i]->m_index = i;
	}
}

void RenderContainerVector::Clear()
{
	for (auto &&object : m_objects)
	{
		if (object)
		{
		//	delete object;
			object->Release();
			object = nullptr;
		}
	}
	m_objects.clear();

}

void RenderContainerVector::Push(Unit * unit)
{
	m_objects.push_back(unit);
}

void RenderContainerVector::Push(Doodads * doodads)
{
	m_objects.push_back(doodads);
}

void RenderContainerVector::Push(AnimatedDoodads* animated)
{
	m_objects.push_back(animated);
}

void RenderContainerVector::Push(Tree * tree)
{
	m_objects.push_back(tree);
}

Boolean _stdcall CheckDistance(RenderContainer* a, RenderContainer* b, float range) noexcept
{

	BoundingSphere* bsa = a->GetBoundingSphere();
	BoundingSphere* bsb = b->GetBoundingSphere();

float distanceX = bsa->Center.x - bsb->Center.x;
float distanceY = bsa->Center.y - bsb->Center.y;
float distance = XMVector2Length({ distanceX,distanceY }).m128_f32[0];
if (distance < range)
{
	return 2;
}
else
{
	if (distanceY < range)
		return 1; 
	else return 0;
}
}

void _stdcall PushUnitsInRange(std::vector<RenderContainer*>* vec,atomic<std::stack<Unit*>*>* sa, RenderContainer* object, float range) noexcept
{
	for (auto unit : *vec)
	{
		if (unit&&unit != object&&(unit->m_type == RenderContainer::RenderContainerType::UNIT))
		{
			switch (CheckDistance(unit, object, range))
			{
			case 2:
				sa->load()->push((Unit*)unit);
				break;
			case 1:
				break;
			case 0:
				goto ENDLOOP;
			}
		}
	}
	ENDLOOP:
	m_async--;
	return;
}

std::stack<Unit*> _vectorcall RenderContainerVector::GetUnitsInRange(Unit* object, float range) noexcept
{
	std::stack<Unit*> units;
	atomic<std::stack<Unit*>*> sa = &units;
	std::vector<RenderContainer*>* upv = &g_units.m_objects;
	size_t index = (size_t)object->m_index;
	
	std::vector<RenderContainer*> fv(upv->begin(), upv->begin() + index);
	std::reverse(fv.begin(), fv.end());
	std::vector<RenderContainer*> sv(upv->begin() + index, upv->end());
	m_async = 2;
	std::async(std::launch::async, PushUnitsInRange, &fv, &sa, object, range);
	std::async(std::launch::async, PushUnitsInRange, &sv, &sa, object, range);
	while (m_async)
	{
		DoNothing();
	}
			return units;
}

