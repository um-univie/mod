_gmlStereoExamples = {}

def foreachStereoExample(f):
	for conf, confExamples in _gmlStereoExamples.items():
		print("#" * 80)
		print(conf)
		print("#" * 80)
		post.summaryChapter(conf)
		for group, examples in confExamples.items():
			print("=" * 80)
			print(group)
			print("=" * 80)
			post.summarySection(group)
			for title, gml in examples:
				print(title)
				print("-" * 80)
				f(conf, group, title, gml)

# =========================================================================

offset = """
	node [ id 0 label "ox" ]
	node [ id 1 label "oy" ]
	edge [ source 0 target 1 label "oq" stereo "*" ]
"""

# =========================================================================
# Tetrahedral
# =========================================================================

tetrahedralPattern =  """
	{}
	node [ id 101 label "r" ]
	node [ id 102 label "s" ]
	node [ id 103 label "t" ]
	edge [ source 100 target 101 label "-" stereo "*" ]
	edge [ source 100 target 102 label "-" stereo "*" ]
	edge [ source 100 target 103 label "-" stereo "*" ]
"""
tetrahedral = tetrahedralPattern.format(
	'node [ id 100 label "q" stereo "tetrahedral[101, 102, 103, e]!" ]')
tetrahedralAnti = tetrahedralPattern.format(
	'node [ id 100 label "q" stereo "tetrahedral[101, 102, e, 103]!" ]')

_gmlStereoExamples["Tetrahedral" ] = {
	"K": [
		("context", "rule [ context [ {} ] ]".format(tetrahedral)),
		("left + right", "rule [ left [ {} ] right [ {} ] ]".format(
			tetrahedral, tetrahedralAnti)),
	],
	"L": [
		("left", "rule [ left [ {} ] ]".format(tetrahedral)),
	],
	"R": [
		("right", "rule [ right [ {} ] ]".format(tetrahedral)),
	],
	"Combinations K": [
		# offset a side to provoke index out of bounds problems
		("context, offset left", """rule [
			left [ {} ]
			context [ {} ]
		]""".format(offset, tetrahedral)),
		("context, offset right", """rule [
			right [ {} ]
			context [ {} ]
		]""".format(offset, tetrahedral)),
	],
}
