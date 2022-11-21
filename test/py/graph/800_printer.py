include("../xxx_helpers.py")

p = GraphPrinter()
assert p.edgesAsBonds
assert not p.collapseHydrogens
assert p.raiseIsotopes
assert p.raiseCharges
assert not p.simpleCarbons
assert not p.thick
assert not p.withColour
assert not p.withIndex
assert not p.withTexttt
assert not p.withRawStereo
assert not p.withPrettyStereo
assert p.rotation == 0
assert not p.mirror
assert not p.withGraphvizCoords
assert p.graphvizPrefix == ""

p.disableAll()
assert not p.edgesAsBonds
assert not p.collapseHydrogens
assert not p.raiseIsotopes
assert not p.raiseCharges
assert not p.simpleCarbons
assert not p.thick
assert not p.withColour
assert not p.withIndex
assert not p.withTexttt
assert not p.withRawStereo
assert not p.withPrettyStereo
assert p.rotation == 0
assert not p.mirror
assert not p.withGraphvizCoords
assert p.graphvizPrefix == ""

p.enableAll()
assert p.edgesAsBonds
assert p.collapseHydrogens
assert p.raiseIsotopes
assert p.raiseCharges
assert p.simpleCarbons
assert p.thick
assert p.withColour
assert p.withIndex
assert not p.withTexttt
assert not p.withRawStereo
assert not p.withPrettyStereo
assert p.rotation == 0
assert not p.mirror
assert not p.withGraphvizCoords
assert p.graphvizPrefix == ""

p.disableAll()
p.setMolDefault()
assert p.edgesAsBonds
assert p.collapseHydrogens
assert p.raiseIsotopes
assert p.raiseCharges
assert p.simpleCarbons
assert not p.thick
assert p.withColour
assert not p.withIndex
assert not p.withTexttt
assert not p.withRawStereo
assert not p.withPrettyStereo
assert p.rotation == 0
assert not p.mirror
assert not p.withGraphvizCoords
assert p.graphvizPrefix == ""

p.disableAll()
p.setReactionDefault()
assert p.edgesAsBonds
assert p.collapseHydrogens
assert p.raiseIsotopes
assert p.raiseCharges
assert not p.simpleCarbons
assert not p.thick
assert p.withColour
assert not p.withIndex
assert not p.withTexttt
assert not p.withRawStereo
assert not p.withPrettyStereo
assert p.rotation == 0
assert not p.mirror
assert not p.withGraphvizCoords
assert p.graphvizPrefix == ""
