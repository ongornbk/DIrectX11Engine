#include "TaskQueue.h"
#include "GlobalUtilities.h"
#include "Unit.h"

using::GlobalUtilities::random;

XMFLOAT3 _vectorcall RandomizeXMFLOAT3(XMFLOAT3 object, float x, float y) noexcept
{
	float xx = object.x;
	float yy = object.y;
	xx += random(-x, x);
	yy += random(-y, y);
	return XMFLOAT3(xx, yy, 0.0f);
}

TaskQueue::TaskQueue()
{

}


TaskQueue::~TaskQueue()
{
	while (!m_tasks.empty())
	{
		m_tasks.front()->Release();
		m_tasks.pop();
}
}

void TaskQueue::Discard()
{
	while (!m_tasks.empty())
	{
		delete m_tasks.front();
		m_tasks.pop();
	}
}


bool TaskQueue::Update()
{
	if (!m_tasks.empty())
	{
		if (m_tasks.front()->Update())
		{
			return true;
		}
		else
		{
			m_tasks.front()->Release();
			m_tasks.pop();
		}
	}
		return false;

}
void TaskQueue::SetTask(Task* task)
{
	Discard();
	m_tasks.push(task);
}
void TaskQueue::QueueTask(Task * task)
{
	m_tasks.push(task);
}
void TaskQueue::Wander(Unit * unit)
{
	TaskGotoPoint* task = new TaskGotoPoint();
	task->destination = RandomizeXMFLOAT3(unit->GetPosition(), 600.0f, 600.0f);
	task->object = unit;
	SetTask(task);
}
