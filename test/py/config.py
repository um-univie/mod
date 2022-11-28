post.disableInvokeMake()

# LabelType
assert LabelType() == LabelType.String
assert str(LabelType.String) == "string"
assert str(LabelType.Term) == "term"

# LabelRelation
assert LabelRelation() == LabelRelation.Isomorphism
assert str(LabelRelation.Isomorphism) == "isomorphism"
assert str(LabelRelation.Specialisation) == "specialisation"
assert str(LabelRelation.Unification) == "unification"

# LabelSettings
l = LabelSettings(LabelType.String, LabelRelation.Specialisation)
assert l.type == LabelType.String
assert l.relation == LabelRelation.Specialisation
assert not l.withStereo
assert l.stereoRelation == LabelRelation.Isomorphism
assert str(l) == "LabelSettings{string}"

l = LabelSettings(LabelType.Term, LabelRelation.Specialisation)
assert l.type == LabelType.Term
assert l.relation == LabelRelation.Specialisation
assert not l.withStereo
assert l.stereoRelation == LabelRelation.Isomorphism
assert str(l) == "LabelSettings{term(specialisation)}"

l = LabelSettings(LabelType.Term, LabelRelation.Specialisation, LabelRelation.Unification)
assert l.type == LabelType.Term
assert l.relation == LabelRelation.Specialisation
assert l.withStereo
assert l.stereoRelation == LabelRelation.Unification
assert str(l) == "LabelSettings{term(specialisation), stereo(unification)}"

l1a1 = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
l1a2 = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
l1b = LabelSettings(LabelType.Term, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
assert l1a1 == l1a2
assert l1a1 != l1b
l2a1 = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
l2a2 = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
l2b = LabelSettings(LabelType.String, LabelRelation.Unification, True, LabelRelation.Specialisation)
assert l2a1 == l2a2
assert l2a1 != l2b
l3a1 = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
l3a2 = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
l3b = LabelSettings(LabelType.String, LabelRelation.Specialisation, False, LabelRelation.Specialisation)
assert l3a1 == l3a2
assert l3a1 != l3b
l4a1 = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
l4a2 = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Specialisation)
l4b = LabelSettings(LabelType.String, LabelRelation.Specialisation, True, LabelRelation.Unification)
assert l4a1 == l4a2
assert l4a1 != l4b
arr = [l1b, l2b, l3b, l4b]
for i in range(len(arr)):
	for j in range(len(arr)):
		if i == j:
			assert arr[i] == arr[j]
		else:
			assert arr[i] != arr[j]

# IsomorphismPolicy
assert IsomorphismPolicy() == IsomorphismPolicy.Check
assert str(IsomorphismPolicy.Check) == "check"
assert str(IsomorphismPolicy.TrustMe) == "trustMe"

# SmilesClassPolicy
assert SmilesClassPolicy() == SmilesClassPolicy.NoneOnDuplicate
assert str(SmilesClassPolicy.NoneOnDuplicate) == "noneOnDuplicate"
assert str(SmilesClassPolicy.ThrowOnDuplicate) == "throwOnDuplicate"
assert str(SmilesClassPolicy.MapUnique) == "mapUnique"

# Action
assert Action() == Action.Error
assert str(Action.Error) == "error"
assert str(Action.Warn) == "warn"
assert str(Action.Ignore) == "ignore"
