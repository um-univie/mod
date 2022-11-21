include("common.py")

print("Missing ring closure")
fail(lambda: check("[A]-1"), "Ring closure ID 1 not found.",
	err=InputError)

print("Loop edges")
fail(lambda: check("[A]1-1"), "Loop edge in DFS on vertex with ID 1.",
	err=InputError)
check("[A]1.1")

fail(lambda: check("[A][B]1-1"), "Loop edge in DFS on vertex with ID 1.",
	err=InputError)
check("[A][B]1.1")

fail(lambda: check("[A]1(-1)"), "Loop edge in DFS on vertex with ID 1.",
	err=InputError)
check("[A]1(.1)")

fail(lambda: check("[A]([B]1-1)"), "Loop edge in DFS on vertex with ID 1.",
	err=InputError)
check("[A]([B]1.1)")


print("Parallel Edges")
def bad(s):
	fail(lambda: check(s), "Parallel edge in DFS. Back-edge is to vertex with ID 1.", err=InputError)

print("="*80)
print("without extra ring closures")
for first in (
	"[A]1",  # chain
	"[Q][A]1", # tail
	"[Q]([A]1", # branch
	):
	for second in (
	"{}[B]", # tail
	"({}[B]", # branch
	):
		suffix = ''
		suffix += ')' if '(' in first else ''
		suffix += ')' if '(' in second else ''
		print("-"*80)
		print("Bad:", first + second)
		for third in ("-1", "(1)", "1"): # tail, branch, ring-closure
			bad(first + second.format('') + third + suffix)
			check(first + second.format('.') + third + suffix)
		for third in (".1", "(.1)"):
			check(first + second.format('') + third + suffix)
			checkMulti(first + second.format('.') + third + suffix)

print("="*80)
print("with extra ring closures")
for first in (
	"[X]2[Y][Z][A]1", # tail
	"[X]2[Y][Z]([A]1", # branch
	):
	for second in (
	"{}[B]", # tail
	"({}[B]", # branch
	):
		suffix = ''
		suffix += ')' if '(' in first else ''
		suffix += ')' if '(' in second else ''
		print("-"*80)
		print("Bad:", first + second)
		for third in ("-1", "(1)", "1"): # tail, branch, ring-closure
			bad(first + second.format('-2') + third + suffix)
			check(first + second.format('-2.') + third + suffix)
		for third in ("-2-1", "-2(1)"): # tail, branch
			bad(first + second.format('') + third + suffix)
			check(first + second.format('.') + third + suffix)
			bad(first + second.format('-2') + third + suffix)
			check(first + second.format('-2.') + third + suffix)
		for third in (".1", "(.1)"):
			check(first + second.format('-2') + third + suffix)
			checkMulti(first + second.format('-2.') + third + suffix)
		for third in ("-2.1", "-2(.1)"):
			check(first + second.format('') + third + suffix)
			checkMulti(first + second.format('.') + third + suffix)
