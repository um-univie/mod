export PKG_CONFIG_PATH := ${PKG_CONFIG_PATH}:<mod installation path>/lib/pkgconfig
CPPFLAGS                = $$(pkg-config --cflags mod)
CPPFLAGS               += -I<Boost installation path>/include
CPPFLAGS               += $$(python3-config --includes)
CXXFLAGS                = -std=c++11 -fpic
LDFLAGS                 = -Wl,--no-undefined -L<Boost installation path>/lib
LDLIBS                  = $$(pkg-config --libs mod) -lboost_python3
LDLIBS                 += $$(python3-config --libs)

OBJECTS                 = pyModule.o stuff.o

awesome.so: $(OBJECTS)
	$(CXX) -shared $(LDFLAGS) -o awesome.so $(OBJECTS) $(LDLIBS)

clean:
	rm -f awesome.so $(OBJECTS)
