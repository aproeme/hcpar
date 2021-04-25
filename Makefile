include make.inc  # all machine-specific locations should be set in make.inc

# LSDTopoTools2 build setting
LSDTOPOTOOLS_SOURCE_DIR := $(LSDTOPOTOOLS_DIR)/src
LSDTOPOTOOLS_INCLUDE_DIR := $(LSDTOPOTOOLS_DIR)/src
LSDTOPOTOOLS_SOURCES := $(notdir $(shell ls $(LSDTOPOTOOLS_SOURCE_DIR)/*.cpp))
LSDTOPOTOOLS_BUILD_DIR := build/LSDTopoTools2
LSDTOPOTOOLS_OBJECTS := $(addprefix $(LSDTOPOTOOLS_BUILD_DIR)/, $(LSDTOPOTOOLS_SOURCES:.cpp=.o))

# Parallel HAIL-CAESAR base build settings
SOURCE_DIR := src
INCLUDE_DIR := src
SOURCES := $(notdir $(shell ls $(SOURCE_DIR)/*.cpp))
OBJECTS := $(SOURCES:.cpp=.o)

IFLAGS := -I $(INCLUDE_DIR) -I $(LSDTOPOTOOLS_INCLUDE_DIR) -I $(GEODECOMP_DIR)/include -I $(BOOST_DIR)/include -I $(PNETCDF_DIR)/include -I ./ -I lib -I lib/TNT
CFLAGS := -std=c++11 $(GITREV) -MD
LDFLAGS := -L $(GEODECOMP_DIR)/lib -L $(BOOST_DIR)/lib 
LIBS := -lgeodecomp -lboost_date_time

ifdef $(MPI_DIR)
IFLAGS += -I $(MPI_DIR)/include 
LDFLAGS += -L $(MPI_DIR)/lib
LIBS += -lmpi
endif

# hc build settings
HC_BUILD_DIR := build
HC_EXE := bin/hc
HC_OBJECTS := $(addprefix $(HC_BUILD_DIR)/, $(OBJECTS))
HC_CFLAGS := $(CFLAGS) -Wfatal-errors -O3

# hc_debug build settings 
DEBUG_BUILD_DIR = build/debug
DEBUG_EXE = bin/hc_debug
DEBUG_OBJECTS = $(addprefix $(DEBUG_BUILD_DIR)/, $(OBJECTS))
DEBUG_CFLAGS := $(CFLAGS) -Wfatal-errors -g -Og -DDEBUG


########################
# Build targets & rules
########################

all: prep hc debug


# Standard (hc) build rules
hc: $(HC_EXE)

$(HC_EXE): $(HC_OBJECTS) $(LSDTOPOTOOLS_OBJECTS)
	@echo  " \n Linking... \n"
	@echo " $(CXX) $(LDFLAGS) $(IFLAGS) $(LIBS) $^ -o $(HC_EXE)"; $(CXX) $(LDFLAGS) $(IFLAGS) $(LIBS) $^ -o $(HC_EXE)

$(HC_BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@echo " $(CXX) $(HC_CFLAGS) $(IFLAGS) -c -o $@ $<"; $(CXX) $(HC_CFLAGS) $(IFLAGS) -c -o $@ $<

-include $(HC_OBJECTS:.o=.d)  # ensures rebuild after any header (.hpp) or template (.tpp) files change (only works with GCC)


# Debug (hc_debug) build rules
debug: $(DEBUG_EXE)

$(DEBUG_EXE): $(DEBUG_OBJECTS) $(LSDTOPOTOOLS_OBJECTS)
	@echo  " \n Linking... \n"
	@echo " $(CXX) $(LDFLAGS) $(IFLAGS) $(LIBS) $^ -o $(DEBUG_EXE)"; $(CXX) $(LDFLAGS) $(IFLAGS) $(LIBS) $^ -o $(DEBUG_EXE)

$(DEBUG_BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@echo " $(CXX) $(DEBUG_CFLAGS) $(IFLAGS) -c -o $@ $<"; $(CXX) $(DEBUG_CFLAGS) $(IFLAGS) -c -o $@ $<

-include $(DEBUG_OBJECTS:.o=.d)  # ensures rebuild after any header (.hpp) or template (.tpp) files change (only works with GCC)


# LDSTopoTools build rules
LSDTopoTools: $(LSDTOPOTOOLS_OBJECTS)

$(LSDTOPOTOOLS_BUILD_DIR)/%.o: $(LSDTOPOTOOLS_SOURCE_DIR)/%.cpp
	@mkdir -p $(LSDTOPOTOOLS_BUILD_DIR)
	@echo " $(CXX) $(HC_CFLAGS) $(IFLAGS) -c -o $@ $<"; $(CXX) $(HC_CFLAGS) $(IFLAGS) -c -o $@ $<


# Other rules
prep:
	@mkdir -p bin $(HC_BUILD_DIR) $(DEBUG_BUILD_DIR)


typemaps: # only necessary to generate new MPI typemaps run if the model has been changed, not needed during normal build process
	@echo " Generating xml using doxygen..."; echo "doxygen ./make/doxygen.conf"; doxygen ./make/doxygen.conf
	@echo " Generating MPI typemaps...";
	@mkdir typemaps
	@$(GEODECOMP_SRC_DIR)/typemapgenerator/generate.rb -S typemaps-doxygen-docs/xml typemaps
	@sed -i '' 's/CellType/int/g' typemaps/typemaps.cpp  # correct mistake in LibGeoDecomp typemap generation script for enums, see https://github.com/gentryx/libgeodecomp/issues/72
	@mv typemaps/typemaps.* $(SOURCE_DIR)/
	@echo " New typemaps saved in src/"
	@rm -rf typemaps
	@rm -rf typemaps-doxygen-docs

clean: cleanbin cleantypemaps
	@echo " Cleaning build base directory: rm -rf build"; rm -rf build

cleantypemaps:
	@echo " Cleaning typemap generation intermediates: rm -rf typemaps typemaps-doxygen-docs"; rm -rf typemaps typemaps-doxygen-docs

cleanbin:
	@echo " Cleaning executables: rm -rf bin"; rm -rf bin



