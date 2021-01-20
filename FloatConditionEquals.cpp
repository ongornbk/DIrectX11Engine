#include "FloatConditionEquals.h"
#include <cassert>

FloatConditionEquals::FloatConditionEquals(IFloatVariable* const A, IFloatVariable* const B) : m_A(A), m_B(B)
{

}

bool FloatConditionEquals::CheckCondition()
{
	return (m_A->get() == m_B->get());
}
