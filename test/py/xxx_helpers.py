post("disableSummary")

def fail(f, pattern, err=LogicError, isSubstring=False):
	try:
		f()
		assert False, "Expected an {} exception.".format(err)
	except err as e:
		if isSubstring:
			res = pattern in str(e)
		else:
			res = str(e).endswith(pattern)
		if not res:
			print("Err:", err)
			print("Expected suffix:", pattern)
			print("str(e):         ", str(e), "<<<")
			raise


def _compareFiles(f1, f2):
	import difflib
	with open(f1, 'rb') as file1, open(f2, 'rb') as file2:
		lines1 = file1.readlines()
		lines2 = file2.readlines()
	differ = difflib.Differ()
	cand = list(line for line in differ.compare(lines1, lines2) if line[0] != ' ')
	if len(cand) != 0:
		msg = "Files differ: %s vs. %s\n" % (f1, f2)
		msg += "Diff is:\n"
		msg += "".join(differ.compare(lines1, lines2))
		raise Exception(msg)
