include("../xxx_helpers.py")

def ruleFail(s, pattern):
	fail(lambda: ruleGMLString("rule [ %s ]" % s),
		pattern=pattern, err=InputError, isSubstring=True)
