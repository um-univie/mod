post("disableSummary")

def fail(f, suffix, err=LogicError):
	try:
		f()
		assert False
	except err as e:
		if not str(e).endswith(suffix):
			print("Err:", err)
			print("Expected suffix:", suffix)
			print("str(e):         ", str(e), "<<<")
			raise
