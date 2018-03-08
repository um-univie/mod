PKG_CONFIG_PATH := ${PKG_CONFIG_PATH}:<installation path>/lib/pkgconfig
CPPFLAGS        := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --cflags mod)
CXXFLAGS        := -std=c++11
LDLIBS          := $(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs mod)

test: test.o
    $(CXX) -o test test.o $(LDLIBS)
