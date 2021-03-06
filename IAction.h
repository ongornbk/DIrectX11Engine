#pragma once
#include "ActionStance.h"
#include "ActionBehavior.h"

class IAction
{
protected:
	//enum class ActionStance m_stance;
public:
	virtual void execute() = 0;
	virtual const enum class ActionBehavior execute_in_array() = 0;
};

