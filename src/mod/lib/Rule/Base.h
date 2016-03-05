#ifndef MOD_LIB_RULE_BASE_H
#define MOD_LIB_RULE_BASE_H

#include <memory>
#include <string>

namespace mod {
class Rule;
namespace lib {
namespace Rule {

class Base {
protected:
	Base();
public:
	virtual ~Base();
	unsigned int getId() const;
	std::shared_ptr<mod::Rule> getAPIReference() const;
	void setAPIReference(std::shared_ptr<mod::Rule> r);
	const std::string &getName() const;
	void setName(std::string name);
	virtual bool isReal() const = 0;
private:
	const unsigned int id;
	std::weak_ptr<mod::Rule> apiReference;
	std::string name;
};

struct Less {

	bool operator()(const Base *r1, const Base *r2) const {
		return r1->getId() < r2->getId();
	}
};

} // namespace Rule
} // namespace lib
} // namespace mod

#endif /* MOD_LIB_RULE_BASE_H */