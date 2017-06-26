CXX      := g++
CXXFLAGS := -fopenmp -fno-strict-aliasing -std=c++0x -pedantic -Wall
LDFLAGS  := -fopenmp -L/opt/local/lib
LIBS     := -lm
.PHONY: all release debian-release info debug clean debian-clean distclean 
NVCC     := /usr/local/cuda/bin/nvcc
NVCC_HOST_CXX := g++-4.6
NVCC_CXXFLAGS := -Xcompiler -fpic -I/usr/local/cuda-5.0/samples/common/inc/
DESTDIR := /
PREFIX := /usr/local
MACHINE := $(shell uname -m)

ifeq ($(MACHINE), x86_64)
  LIBDIR = lib64
endif
ifeq ($(MACHINE), i686)
  LIBDIR = lib
endif

ifdef JAVASCRIPT
CXX			 := em++
CXXFLAGS += -I/usr/local/include
WITHOUT_SERIALIZE=1
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -L/usr/lib -m32 
endif

ifdef STATIC
LDFLAGS += -static-libgcc -Wl,-Bstatic
endif

ifdef X86
CXXFLAGS += -m32
LDFLAGS += -L/usr/lib -static-libgcc -m32 -Wl,-Bstatic
endif 

ifdef WITHOUT_PROGRAM_OPTS
CXXFLAGS += -D_NO_PROGRAM_OPTIONS
else
LIBS += -lboost_system -lboost_program_options
endif

ifdef WITHOUT_SERIALIZE
CXXFLAGS += -D_NO_SERIALIZE
else
LIBS     += -lboost_system -lboost_serialization
endif

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
 LDFLAGS += -L/opt/X11/lib/
else
 CXXFLAGS += -march=native
endif

all: release

ifneq ($(UNAME_S), Darwin)
release: LDFLAGS += -s
endif
release: CXXFLAGS += -g0 -O3
release: dirs

info: CXXFLAGS += -g3 -O0
info: LDFLAGS += -Wl,--export-dynamic -rdynamic
info: dirs

debug: CXXFLAGS += -g3 -O0 -rdynamic
debug: LDFLAGS += -Wl,--export-dynamic -rdynamic
debug: dirs

profile: CXXFLAGS += -g3 -O1
profile: LDFLAGS += -Wl,--export-dynamic -rdynamic
profile: dirs

hardcore: CXXFLAGS += -g0 -Ofast -DNDEBUG
ifeq ($(UNAME_S), Darwin)
hardcore: LDFLAGS += -s
endif
hardcore: dirs

asan: CXXFLAGS += -stdlib=libstdc++ -g3 -O0 -fsanitize=address -fno-omit-frame-pointer
asan: LDFLAGS += -fsanitize=address
asan: CXX = clang++
asan: dirs

clean: dirs

export LDFLAGS
export CXXFLAGS
export LIBS

dirs:
	${MAKE} -C src/classifier ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" ${MAKECMDGOALS}
	
debian-release:
	${MAKE} -C src/classifier ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" release
			
debian-clean:
	${MAKE} -C src/classifier ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" clean
	
install: ${TARGET}
	${MAKE} -C src/classifier ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" install
			
distclean:
	${MAKE} -C src/classifier ${MAKEFLAGS} CXX=${CXX} NVCC="${NVCC}" NVCC_HOST_CXX="${NVCC_HOST_CXX}" NVCC_CXXFLAGS="${NVCC_CXXFLAGS}" distclean
	
