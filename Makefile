# CXX      := /usr/local/tanowrt-x86_64/sysroots/x86_64-tano-linux/usr/bin/aarch64-oe-linux/aarch64-oe-linux-g++
# CXXFLAGS := --sysroot=/usr/local/tanowrt-x86_64/sysroots/aarch64-oe-linux
# CPPFLAGS := -L ./lib -I ./libgpiod-1.4.3/include -I ./libgpiod-1.4.3/bindings/cxx

CXX      := mipsel-openwrt-linux-g++
CXXFLAGS :=
CPPFLAGS :=
CPPFLAGS += -L /home/user/wolfberry/openwrt/staging_dir/target-mipsel_24kc_musl/usr/lib
CPPFLAGS += -I /home/user/wolfberry/openwrt/staging_dir/target-mipsel_24kc_musl/usr/include

LDLIBS   := -lgpiod -lgpiodcxx

CXXFLAGS += -Wall -std=c++17

EXECUTABLE = inkvis

SOURCES = \
	main.cpp \
	converter.cpp \
	\
	display.cpp \

OBJECTS=$(subst .cpp,.o,$(SOURCES))

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o $(EXECUTABLE) $(OBJECTS) $(LDLIBS)

depend: .depend

.depend: $(SOURCES)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $^ >> ./.depend;

clean:
	rm -f $(OBJECTS)
	rm -f $(EXECUTABLE)

dist-clean: clean
	rm -f *~ .depend

include .depend
