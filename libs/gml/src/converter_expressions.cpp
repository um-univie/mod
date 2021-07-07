#include <gml/converter_expressions.hpp>

#include <boost/lexical_cast.hpp>

namespace gml::converter::detail {

bool ExpressionBase::checkKey(const std::string &key) const noexcept {
	return key == this->key;
}


void ExpressionBase::errorOnKey(const ast::KeyValue &kv) const {
	throw error("Error at " + std::to_string(kv.line) + ":" + std::to_string(kv.column) + "."
	            + " Expected key '" + this->key + "', got key '" + kv.key + "'.");
}

void ExpressionBase::checkAndErrorOnKey(const ast::KeyValue &kv) const {
	const bool res = checkKey(kv.key);
	if(!res) errorOnKey(kv);
}

void ExpressionBase::checkAndErrorOnType(const ast::Value &value, ValueType expected) const {
	ValueType vt = boost::apply_visitor(ValueTypeVisitor(), value);
	if(vt != expected)
		throw error("Error at " + std::to_string(value.line) + ":" + std::to_string(value.column) + "."
		            + " Expected " + boost::lexical_cast<std::string>(expected) + " value, got "
		            + boost::lexical_cast<std::string>(vt) + " value.");
}

} // namespace gml::converter::detail