def check(res, gml):
	try:
		p = ruleGMLString(gml)
	except InputError as e:
		if res:
			raise e
		else:
			print("Yay, error:\n%s" % str(e), end="")
	else:
		if not res:
			print("Expected InputError.")
			assert False
		else:
			print("Yay, no error.")
check(False, """rule [ left [ node [ id 0 ] ] ]""")
check(False, """rule [ context [ node [ id 0 ] ] ]""")
check(False, """rule [ right [ node [ id 0 ] ] ]""")
check(False, """rule [
	right [ node [ id 0 ] ]
	context [ node [ id 0 ] ]
	left [ node [ id 0 ] ]
]""")
check(True, """rule [
	right [ node [ id 0 ] ]
	context [ node [ id 0 label "A" ] ]
	left [ node [ id 0 ] ]
]""")
