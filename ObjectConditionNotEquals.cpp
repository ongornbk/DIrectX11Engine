#include "ObjectConditionNotEquals.h"

ObjectConditionNotEquals::ObjectConditionNotEquals(class IObjectVariable* const A, class IObjectVariable* const B) : m_A(A), m_B(B)
{
	assert(A && B);
}

bool ObjectConditionNotEquals::CheckCondition()
{
	return (m_A->get() != m_B->get());
}
