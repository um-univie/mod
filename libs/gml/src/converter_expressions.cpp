#include <gml/converter_expressions.hpp>

namespace gml::converter::detail {

bool ExpressionBase::checkKey(const std::string &key) const {
	return key == this->key;
}


void ExpressionBase::errorOnKey(const ast::KeyValue &kv, std::ostream &err) const {
	err << "Error at " << kv.line << ":" << kv.column << ".";
	err << " Expected key '" << this->key << "', got key '" << kv.key << "'.";
}

bool ExpressionBase::checkAndErrorOnKey(const ast::KeyValue &kv, std::ostream &err) const {
	bool res = checkKey(kv.key);
	if(!res) errorOnKey(kv, err);
	return res;
}

bool ExpressionBase::checkAndErrorOnType(const ast::Value &value, std::ostream &err, ValueType expected) const {
	ValueType vt = boost::apply_visitor(ValueTypeVisitor(), value);
	if(vt != expected) {
		err << "Error at " << value.line << ":" << value.column << ".";
		err << " Expected " << expected << " value, got " << vt << " value.";
		return false;
	}
	return true;
}

} // namespace gml::converter::detail