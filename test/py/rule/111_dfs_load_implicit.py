include("xxx_helpers.py")

msg = "Vertices with implicit hydrogen atoms currently not supported."
dfsFail("C>>", msg)
dfsFail(">>C", msg)
dfsFail("C1>>C1", msg)
