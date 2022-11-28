include("xxx_helpers.py")

# The tests here assume that the Graph DFS tests cover all details.
# Here we just test that the issues are caught in Rule DFS as well.

# Missing ring closure
dfsFail("[A]-1>>", "Ring closure ID 1 not found, in the left side.")
dfsFail(">>[A]-1", "Ring closure ID 1 not found, in the right side.")
dfsFail("[A]-1>>[A]-1", "Ring closure ID 1 not found, in the left side.")

# Loop edges
dfsFail("[A]1-1>>",
	"Loop edge in DFS on vertex in the left side with ID 1.")
dfsFail(">>[A]1-1",
	"Loop edge in DFS on vertex in the right side with ID 1.")
dfsFail("[A]1-1>>[A]1-1",
	"Loop edge in DFS on vertex in the left side with ID 1.")

# Parallel
dfsFail("[A]1[B]-1>>",
	"Parallel edge in DFS in the left side. Back-edge is to vertex with ID 1.")
dfsFail(">>[A]1[B]-1",
	"Parallel edge in DFS in the right side. Back-edge is to vertex with ID 1.")
dfsFail("[A]1[B]-1>>[A]1[B]-1",
	"Parallel edge in DFS in the left side. Back-edge is to vertex with ID 1.")

