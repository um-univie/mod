include("common.py")
printGeometryGraph()

def gmlNode(i, edge="-"):
	return 'node [ id %d label "Z" ] edge [ source 0 target %d label "%s" ]' % (i, i, edge)

def check(f):
	try:
		a = f()
	except mod.InputError as e:
		print("Exception, as expected:")
		print(e)
		print()
	else:
		assert False

print('linear')
check(lambda: gGML('node [ id 0 label "C" stereo "linear" ]' + gmlNode(1)))
check(lambda: gGML('node [ id 0 label "C" stereo "linear" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3)))

print('trigonalPlanar')
check(lambda: gGML('node [ id 0 label "C" stereo "trigonalPlanar" ]' + gmlNode(1, "=") + gmlNode(2)))
check(lambda: gGML('node [ id 0 label "C" stereo "trigonalPlanar" ]' + gmlNode(1, "=") + gmlNode(2) + gmlNode(3) + gmlNode(4)))

print('tetrahedral')
check(lambda: gGML('node [ id 0 label "C" stereo "tetrahedral" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3)))
check(lambda: gGML('node [ id 0 label "C" stereo "tetrahedral" ]' + gmlNode(1) + gmlNode(2) + gmlNode(3) + gmlNode(4) + gmlNode(5)))

