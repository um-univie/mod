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

# IsomorphismPolicy
assert IsomorphismPolicy() == IsomorphismPolicy.Check
assert str(IsomorphismPolicy.Check) == "check"
assert str(IsomorphismPolicy.TrustMe) == "trustMe"
