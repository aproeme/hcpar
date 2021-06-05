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
CXXFLAGS := -std=c++11 $(GITREV) -MD
LDFLAGS := -L $(GEODECOMP_DIR)/lib -L $(BOOST_DIR)/lib 
LIBS := -lgeodecomp -lboost_date_time

ifdef $(MPI_DIR)
IFLAGS += -I $(MPI_DIR)/include 
LDFLAGS += -L $(MPI_DIR)/lib
LIBS += -lmpi
endif

BUILD_DIR_DEBUG := build/debug
BUILD_DIR := build
EXE_DEBUG := bin/debug/hc
EXE := bin/hc
OBJECTS_DEBUG := $(addprefix $(BUILD_DIR_DEBUG)/, $(OBJECTS)) 
OBJECTS := $(addprefix $(BUILD_DIR)/, $(OBJECTS))
CXXFLAGS_DEBUG := $(CXXFLAGS) -Wfatal-errors -g -Og
CXXFLAGS := $(CXXFLAGS) -Wfatal-errors -O3

########################
# Build targets & rules
########################

hc: $(EXE)

debug: $(EXE_DEBUG)


# Link
$(EXE): $(OBJECTS) $(LSDTOPOTOOLS_OBJECTS)
	@echo  " \n Linking... \n"
	@echo " $(CXX) $(LDFLAGS) $(IFLAGS) $(LIBS) $^ -o $(EXE)"; $(CXX) $(LDFLAGS) $(IFLAGS) $(LIBS) $^ -o $(EXE)

$(EXE_DEBUG): $(OBJECTS_DEBUG) $(LSDTOPOTOOLS_OBJECTS)
	@echo  " \n Linking... \n"
	@echo " $(CXX) $(LDFLAGS) $(IFLAGS) $(LIBS) $^ -o $(EXE_DEBUG)"; $(CXX) $(LDFLAGS) $(IFLAGS) $(LIBS) $^ -o $(EXE_DEBUG)


# Compile
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p bin $(BUILD_DIR)
	@echo " $(CXX) $(CXXFLAGS) $(IFLAGS) -c -o $@ $<"; $(CXX) $(CXXFLAGS) $(IFLAGS) -c -o $@ $<

-include $(OBJECTS:.o=.d)  # ensures rebuild after any header (.hpp) or template (.tpp) files change (only works with GCC)

$(BUILD_DIR_DEBUG)/%.o: $(SOURCE_DIR)/%.cpp
	@mkdir -p bin/debug $(BUILD_DIR_DEBUG)
	@echo " $(CXX) $(CXXFLAGS_DEBUG) $(IFLAGS) -c -o $@ $<"; $(CXX) $(CXXFLAGS_DEBUG) $(IFLAGS) -c -o $@ $<

-include $(OBJECTS_DEBUG:.o=.d)  # ensures rebuild after any header (.hpp) or template (.tpp) files change (only works with GCC)


# LDSTopoTools build rules
LSDTopoTools: $(LSDTOPOTOOLS_OBJECTS)

$(LSDTOPOTOOLS_BUILD_DIR)/%.o: $(LSDTOPOTOOLS_SOURCE_DIR)/%.cpp
	@mkdir -p $(LSDTOPOTOOLS_BUILD_DIR)
	@echo " $(CXX) $(CXXFLAGS) $(IFLAGS) -c -o $@ $<"; $(CXX) $(CXXFLAGS) $(IFLAGS) -c -o $@ $<


# Other rules
.PHONY: prep

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



