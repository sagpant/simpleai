#pragma once

#include "conditions/ICondition.h"

namespace ai {

/**
 * @brief This condition just always evaluates to @c false
 */
class False: public ICondition {
public:
	CONDITION_CLASS_SINGLETON(False)

	bool evaluate(const AIPtr& /*entity*/) override {
		return false;
	}

	std::ostream& print(std::ostream& stream, int level) const override {
		stream << _name;
		return stream;
	}
};

}
