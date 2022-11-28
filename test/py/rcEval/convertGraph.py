include("common.py")

def convert(name, cls, f):
	post.summarySection(name)
	print(name + ", explicit -------------------------------------------------")
	handleExp(cls(formaldehyde))
	print(name + ", implicit -------------------------------------------------")
	handleExp(f(formaldehyde))
	print(name + ", multi ----------------------------------------------------")
	handleExp(f(inputGraphs), True)
	print()
convert("Bind", RCExpBind, rcBind)
convert("Id", RCExpId, rcId)
convert("Unbind", RCExpUnbind, rcUnbind)
