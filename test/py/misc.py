include("xxx_helpers.py")

print("magicLibraryValue:", magicLibraryValue())
print("version:", version())
rngReseed(42)
print("rngUniformReal:", rngUniformReal())

assert prefixFilename('a/').endswith('a/')

pushFilePrefix('prefix/')
assert prefixFilename('a/').endswith('prefix/a/')
assert prefixFilename('/a') == '/a'
assert prefixFilename('') == ''
assert prefixFilename(CWDPath('a/')) == 'a/'

popFilePrefix()
assert prefixFilename('a/').endswith('a/')
assert not prefixFilename('a/').endswith('prefix/a/')

pushFilePrefix('prefix/')
