#pragma once
#include "IParameter.h"
class PointerParameter : public IParameter
{
	void* m_object;
public:
	PointerParameter(void* const object);
	~PointerParameter();

	void* const get();
	const enum class ParameterType get_type() const noexcept override;
};

