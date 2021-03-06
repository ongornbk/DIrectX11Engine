#include "BooleanConditionNotEquals.h"
#include <cassert>

BooleanConditionNotEquals::BooleanConditionNotEquals(IBooleanVariable* const A, IBooleanVariable* const B) : m_A(A), m_B(B)
{
	assert(A && B);
}

bool BooleanConditionNotEquals::CheckCondition()
{
	return (m_A->get() != m_B->get());
}