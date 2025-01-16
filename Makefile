
# Makefile to build Trick

# Make Targets
#-------------------------------------------------------------------------------
# default   - Compile Trick-core and Trick Data-products Libraries and Applications.
# 'no_dp'   - Compile Trick-core only.
# 'dp'      - Compile Data-products only.
# 'java'    - Compile Java GUI Applications
# 'javadoc' - Generate Java Documentation.
# 'doxygen' - Generate HTML User's Guide.
# 'test'    - Run Unit-tests and Simulation Tests.

export TRICK_HOME = $(CURDIR)

# Include the build configuration information.
include $(TRICK_HOME)/share/trick/makefiles/Makefile.common

#-------------------------------------------------------------------------------
# Specify the contents of: libtrick.a
#-------------------------------------------------------------------------------
TRICK_LIB = $(TRICK_LIB_DIR)/libtrick.a
SIM_SERV_DIRS = \
	${TRICK_HOME}/trick_source/sim_services/ExternalApplications \
	${TRICK_HOME}/trick_source/sim_services/Clock \
	${TRICK_HOME}/trick_source/sim_services/CheckPointAgent \
	${TRICK_HOME}/trick_source/sim_services/CheckPointRestart \
	${TRICK_HOME}/trick_source/sim_services/Collect \
	${TRICK_HOME}/trick_source/sim_services/CommandLineArguments \
	${TRICK_HOME}/trick_source/sim_services/DataRecord \
	${TRICK_HOME}/trick_source/sim_services/DebugPause \
	${TRICK_HOME}/trick_source/sim_services/EchoJobs \
	${TRICK_HOME}/trick_source/sim_services/Environment \
	${TRICK_HOME}/trick_source/sim_services/EventManager \
	${TRICK_HOME}/trick_source/sim_services/Executive \
	${TRICK_HOME}/trick_source/sim_services/FrameLog \
	${TRICK_HOME}/trick_source/sim_services/JITInputFile \
	${TRICK_HOME}/trick_source/sim_services/JSONVariableServer \
	${TRICK_HOME}/trick_source/sim_services/Integrator \
	${TRICK_HOME}/trick_source/sim_services/UnitTest \
	${TRICK_HOME}/trick_source/sim_services/MasterSlave \
	${TRICK_HOME}/trick_source/sim_services/MemoryManager \
	${TRICK_HOME}/trick_source/sim_services/Message \
	${TRICK_HOME}/trick_source/sim_services/MonteCarlo \
	${TRICK_HOME}/trick_source/sim_services/MonteCarloGeneration \
	${TRICK_HOME}/trick_source/sim_services/RealtimeInjector \
	${TRICK_HOME}/trick_source/sim_services/RealtimeSync \
	${TRICK_HOME}/trick_source/sim_services/ScheduledJobQueue \
	${TRICK_HOME}/trick_source/sim_services/Scheduler \
	${TRICK_HOME}/trick_source/sim_services/Sie \
	${TRICK_HOME}/trick_source/sim_services/SimObject \
	${TRICK_HOME}/trick_source/sim_services/SimTime \
	${TRICK_HOME}/trick_source/sim_services/ThreadBase \
	${TRICK_HOME}/trick_source/sim_services/Timer \
	${TRICK_HOME}/trick_source/sim_services/UdUnits \
	${TRICK_HOME}/trick_source/sim_services/UnitsMap \
	${TRICK_HOME}/trick_source/sim_services/VariableServer \
	${TRICK_HOME}/trick_source/sim_services/Zeroconf \
	${TRICK_HOME}/trick_source/sim_services/include \
	${TRICK_HOME}/trick_source/sim_services/mains
ifeq ($(USE_ER7_UTILS), 0)
SIM_SERV_DIRS += ${TRICK_HOME}/trick_source/sim_services/Integrator/trick_algorithms
endif

SIM_SERV_OBJS = $(addsuffix /object_$(TRICK_HOST_CPU)/*.o ,$(SIM_SERV_DIRS))
SIM_SERV_OBJS := $(filter-out ${TRICK_HOME}/trick_source/sim_services/MemoryManager/%, $(SIM_SERV_OBJS))

ER7_UTILS_DIRS = \
	${ER7_UTILS_HOME}/integration/abm4 \
	${ER7_UTILS_HOME}/integration/beeman \
	${ER7_UTILS_HOME}/integration/core \
	${ER7_UTILS_HOME}/integration/euler \
	${ER7_UTILS_HOME}/integration/mm4 \
	${ER7_UTILS_HOME}/integration/nl2 \
	${ER7_UTILS_HOME}/integration/position_verlet \
	${ER7_UTILS_HOME}/integration/rk2_heun \
	${ER7_UTILS_HOME}/integration/rk2_midpoint \
	${ER7_UTILS_HOME}/integration/rk4 \
	${ER7_UTILS_HOME}/integration/rkf45 \
	${ER7_UTILS_HOME}/integration/rkf78 \
	${ER7_UTILS_HOME}/integration/rkg4 \
	${ER7_UTILS_HOME}/integration/rkn4 \
	${ER7_UTILS_HOME}/integration/symplectic_euler \
	${ER7_UTILS_HOME}/integration/velocity_verlet \
	${ER7_UTILS_HOME}/interface \
	${ER7_UTILS_HOME}/math \
	${ER7_UTILS_HOME}/trick/integration
ifeq ($(USE_ER7_UTILS_CHECKPOINTHELPER), 1)
ER7_UTILS_DIRS += ${ER7_UTILS_HOME}/CheckpointHelper
endif
ER7_UTILS_OBJS = $(addsuffix /object_$(TRICK_HOST_CPU)/*.o ,$(ER7_UTILS_DIRS))

UTILS_DIRS := \
	${TRICK_HOME}/trick_source/trick_utils/compareFloatingPoint \
	${TRICK_HOME}/trick_source/trick_utils/interpolator \
	${TRICK_HOME}/trick_source/trick_utils/trick_adt \
	${TRICK_HOME}/trick_source/trick_utils/comm \
	${TRICK_HOME}/trick_source/trick_utils/connection_handlers \
	${TRICK_HOME}/trick_source/trick_utils/shm \
	${TRICK_HOME}/trick_source/trick_utils/math \
	${TRICK_HOME}/trick_source/trick_utils/units \
	${TRICK_HOME}/trick_source/trick_utils/unicode \
	${TRICK_HOME}/trick_source/trick_utils/var_binary_parser

UTILS_OBJS := $(addsuffix /object_$(TRICK_HOST_CPU)/*.o ,$(UTILS_DIRS))

# filter out the directories that make their own libraries
UTILS_OBJS := $(filter-out ${TRICK_HOME}/trick_source/trick_utils/comm/%, $(UTILS_OBJS))
UTILS_OBJS := $(filter-out ${TRICK_HOME}/trick_source/trick_utils/connection_handlers/%, $(UTILS_OBJS))
UTILS_OBJS := $(filter-out ${TRICK_HOME}/trick_source/trick_utils/math/%, $(UTILS_OBJS))
UTILS_OBJS := $(filter-out ${TRICK_HOME}/trick_source/trick_utils/units/%, $(UTILS_OBJS))
UTILS_OBJS := $(filter-out ${TRICK_HOME}/trick_source/trick_utils/var_binary_parser/%, $(UTILS_OBJS))

#-------------------------------------------------------------------------------
# Specify the contents of: libtrick_pyip.a
# This library contains the SWIG generated interface-code between Trick and Python.
#-------------------------------------------------------------------------------
TRICK_SWIG_LIB = $(TRICK_LIB_DIR)/libtrick_pyip.a

SWIG_DIRS = \
	${TRICK_HOME}/trick_source/sim_services/InputProcessor \
	${TRICK_HOME}/trick_source/trick_swig

SWIG_OBJS = $(addsuffix /object_$(TRICK_HOST_CPU)/*.o ,$(SWIG_DIRS))

#-------------------------------------------------------------------------------
# Specify where to find units tests.
#-------------------------------------------------------------------------------
UNIT_TEST_DIRS := \
    $(wildcard ${TRICK_HOME}/trick_source/sim_services/*/test) \
    $(wildcard ${TRICK_HOME}/trick_source/trick_utils/*/test)
ifeq ($(USE_ER7_UTILS), 0)
  UNIT_TEST_DIRS := $(filter-out %Integrator/test,$(UNIT_TEST_DIRS))
endif

# DPX test excluded from releases because of size
DPX_UNIT_TEST_DIR = ${TRICK_HOME}/trick_source/data_products/DPX/test/unit_test

# The name of the ICG executable indicates the operating system, and the machine
# hardware on which it is built. This allows pre-build ICG binaries to be
# distributed in the installation package. The reason for distributing pre-built
# ICG binaries is because the user's machine may not have the requisite clang
# libraries.
ICG_EXE := ${TRICK_HOME}/bin/trick-ICG

################################################################################
#                                   RULES
################################################################################
# DEFAULT TARGET
# 1 Build Trick-core and Trick Data-products.
all: no_dp dp
	@ echo ; echo "[32mTrick compilation complete:[00m" ; date

ifeq ($(USE_JAVA), 1)
all: java
endif

ifeq ($(USE_CIVETWEB), 1)
all: civetweb
endif

#-------------------------------------------------------------------------------
# 1.1 Build Trick-core
no_dp: $(TRICK_LIB) $(TRICK_SWIG_LIB)
	@ echo ; echo "Trick libs compiled:" ; date

# 1.1.1 Build libTrick.a
$(TRICK_LIB): $(SIM_SERV_DIRS) $(UTILS_DIRS) | $(TRICK_LIB_DIR)
	ar crs $@ $(SIM_SERV_OBJS) $(UTILS_OBJS)

ifeq ($(USE_ER7_UTILS), 1)
ER7_UTILS_LIB = $(TRICK_LIB_DIR)/liber7_utils.a
no_dp: $(ER7_UTILS_LIB)

$(ER7_UTILS_LIB): $(ER7_UTILS_DIRS) | $(TRICK_LIB_DIR)
	ar crs $@ $(ER7_UTILS_OBJS)
endif

# 1.1.1.1 Compile the objects in the specified sim_services directories.
.PHONY: $(SIM_SERV_DIRS)
$(SIM_SERV_DIRS): icg_sim_serv $(TRICK_LIB_DIR)
	@ $(MAKE) -C $@ trick

# 1.1.1.2 Compile the objects in the specified utils directories.
.PHONY: $(UTILS_DIRS)
$(UTILS_DIRS): icg_sim_serv
	@ $(MAKE) -C $@ trick

# 1.1.1.3 Compile the objects in the specified er7_utils directories.
.PHONY: $(ER7_UTILS_DIRS)
$(ER7_UTILS_DIRS): TRICK_CXXFLAGS += -Wno-unused-parameter
$(ER7_UTILS_DIRS): icg_sim_serv
	@ $(MAKE) -C $@ trick

# 1.1.1.4 Generate interface code (using ICG) for the specified sim_services
# header files.
.PHONY: icg_sim_serv
# Replace -isystem with -I so ICG doesn't skip Trick headers
icg_sim_serv: TRICK_SYSTEM_CXXFLAGS := $(subst -isystem,-I,$(TRICK_SYSTEM_CXXFLAGS))
icg_sim_serv: $(ICG_EXE)
	${ICG_EXE} -sim_services -m ${TRICK_CXXFLAGS} ${TRICK_SYSTEM_CXXFLAGS} ${TRICK_HOME}/include/trick/files_to_ICG.hh

# 1.1.1.4.1 Build the Interface Code Generator (ICG) executable.
$(ICG_EXE) :
	$(MAKE) -C trick_source/codegen/Interface_Code_Gen

# 1.1.1.5 Create Trick Library directory.
$(TRICK_LIB_DIR):
	@ mkdir $@

# 1.1.2 Build libTrick_pyip.a (Swig Lib)
$(TRICK_SWIG_LIB): $(SWIG_DIRS) | $(TRICK_LIB_DIR)
	ar crs $@ $(SWIG_OBJS)

.PHONY: $(SWIG_DIRS)
$(SWIG_DIRS): icg_sim_serv $(TRICK_LIB_DIR)
	@ $(MAKE) -C $@ trick

#-------------------------------------------------------------------------------
# 1.2 Build Trick's Data-products Applications.
.PHONY: dp
dp: ${TRICK_HOME}/trick_source/trick_utils/units
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/data_products

#-------------------------------------------------------------------------------
#

#-------------------------------------------------------------------------------
# 1.2 Build Trick's CivetWeb webserver.

.PHONY: civetweb
civetweb: ${TRICK_LIB_DIR}/libtrickCivet.a

${TRICK_LIB_DIR}/libtrickCivet.a:
	$(MAKE) -C ${TRICK_HOME}/trick_source/web/CivetServer

#-------------------------------------------------------------------------------
# 1.3 Build Trick's Java Tools

JAVA_BUILD_DIR = ${TRICK_HOME}/libexec/trick/java/build

ifeq (${TRICK_OFFLINE}, 0)
java: ${JAVA_BUILD_DIR}

${JAVA_BUILD_DIR}: 
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java

.PHONY: javadoc
javadoc:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java $@
else
JAVA_SOURCE_DIR = ${TRICK_HOME}/trick-offline
JARS = DP Dre JXPlot MM MTV QP Sie SimControl SimSniffer TrickView trick-java-${TRICK_VERSION}
JAR_TARGETS = $(foreach JAR, $(JARS), ${JAVA_BUILD_DIR}/$(JAR).jar)

define JAR_FUN
$${JAVA_BUILD_DIR}/$(1).jar: $${JAVA_SOURCE_DIR}/$(1).jar | $${JAVA_BUILD_DIR}
	cp $$< $$@

endef

$(foreach JAR,$(JARS),$(eval $(call JAR_FUN,$(JAR))))

${JAVA_BUILD_DIR}: 
	mkdir -p ${TRICK_HOME}/libexec/trick/java/build

java: ${JAR_TARGETS} 
	@echo offline mode: java code copied successfully

endif

#-------------------------------------------------------------------------------
# 1.4 This target builds the Trick Documentation.
.PHONY: doxygen
doxygen:
	@ $(MAKE) -C $@

#-------------------------------------------------------------------------------
# 1.5 Some Trick source is auto-generated as part of the Trick's build process. When
# Trick is distributed to the user community, we can't be certain that everyone's
# machine will have the approriate versions of the code generations tool. So rather
# than just hope, we go ahead and pre-generate the necessary source files, and
# include those in the distribution package.
# This target pre-generates these source files, that are necessary for creating
# a distribution package.
premade:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/sim_services/MemoryManager premade
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/sim_services/CheckPointAgent premade
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java

################################################################################
#                                   TESTING
################################################################################

# This target runs Trick's Unit-tests and simulation-tests.
test: unit_test sim_test
	@ echo "All tests completed sucessfully"

test32: sim_test32

.PHONY: $(UNIT_TEST_DIRS)
$(UNIT_TEST_DIRS):
	@ $(MAKE) -C $@ test

unit_test: $(UNIT_TEST_DIRS) $(DPX_UNIT_TEST_DIR)

# DPX test excluded from releases because of size
.PHONY: $(DPX_UNIT_TEST_DIR)
$(DPX_UNIT_TEST_DIR):
	@ if [ -d ${DPX_UNIT_TEST_DIR} ]; then $(MAKE) -C $@ test; fi


sim_test:
	@ $(MAKE) -f test_overrides.mk sim_test

sim_test32:
	@ $(MAKE) -f test_overrides.mk sim_test32

pytest:
	make -C share/trick/pymods/trick

gui_test:
	@ $(MAKE) -C trick_source/java test

COVERAGE_DIRS = trick_source/sim_services \
				trick_source/trick_utils/var_binary_parser \
				trick_source/trick_utils/unicode \
				trick_source/trick_utils/units \
				trick_source/trick_utils/interpolator \
				trick_source/trick_utils/comm \
				trick_source/trick_utils/SAIntegrator

extra-coverage-builds:
	@ $(MAKE) test -C trick_source/trick_utils/SAIntegrator

code-coverage: test extra-coverage-builds
	lcov --capture $(addprefix --directory , $(COVERAGE_DIRS)) --output-file coverage_large.info
	lcov --remove coverage_large.info '/Library/*' '/usr/*' '*/io_src/*' '*/test/*' '*/unittest/*' -o coverage.info
	rm coverage_large.info
	lcov --list coverage.info

#requirements:
#	@ $(MAKE) -C trick_test/requirements_docs install

################################################################################
#                                 CLEAN Targets
################################################################################


clean: clean_sim_serv clean_utils clean_swig clean_dp clean_ICG clean_java clean_sim_serv_xml
	@/bin/rm -rf $(TRICK_BIN_DIR)
	@/bin/rm -rf $(TRICK_LIB_DIR)

ifeq ($(USE_ER7_UTILS), 1)
clean: clean_er7_utils
endif

clean_sim_serv:
	@for i in $(SIM_SERV_DIRS) ; do \
	   $(MAKE) -C $$i real_clean ; \
	done
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/sim_services/mains real_clean

clean_er7_utils: 
	@for i in $(ER7_UTILS_DIRS) ; do \
	   $(MAKE) -C $$i real_clean ; \
	done

clean_utils:
	@for i in $(UTILS_DIRS) ; do \
	   $(MAKE) -C $$i real_clean ; \
	done

clean_swig:
	@for i in $(SWIG_DIRS) ; do \
	   $(MAKE) -C $$i real_clean ; \
	done

clean_ICG :
	$(MAKE) -C ${TRICK_HOME}/trick_source/codegen/Interface_Code_Gen  clean

clean_unit_test:
	@/bin/rm -rf ${TRICK_HOME}/trick_test/
	@ for i in $(UNIT_TEST_DIRS) ; do \
	    $(MAKE) -C $$i clean ; \
	done
	$(MAKE) -C $(DPX_UNIT_TEST_DIR) clean

clean_doxygen:
	@ $(MAKE) -C ${TRICK_HOME}/doxygen clean


clean_dp:
	@ $(MAKE) clean -C ${TRICK_HOME}/trick_source/data_products

clean_java:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java clean

clean_sim_serv_xml:
	@/bin/rm -rf ${TRICK_HOME}/share/trick/xml/*


# FIXME: Seems to me that the for loop below should be removed and that the
#        'clean' target in trick_sims/makefile should be doing this. --Penn
clean_test: clean_unit_test
	-@ $(MAKE) -C trick_sims clean
	-@ $(MAKE) -C test clean
	@/bin/rm -rf ${TRICK_HOME}/trickops_logs

clean_gui: clean_java

spotless: clean clean_test
	rm -f config.log config.status share/trick/makefiles/config_user.mk

apocalypse: spotless
	@echo "[31mI love the smell of napalm in the morning[0m"


################################################################################
#                                 INSTALL Targets
################################################################################

ER7_HEADERS := $(addprefix $(PREFIX)/include/, $(filter er7_utils/%, $(shell cd trick_source && find er7_utils -name \*.hh)))

${ER7_HEADERS} : ${PREFIX}/include/% : trick_source/%
	@ mkdir -p ${@D}
	install -m 0644 $? $@

install: ${ER7_HEADERS}
	@if [ ! -d ${TRICK_LIB_DIR} ]; then \
		make; \
	fi

	@if [ ${PREFIX} = "/usr/local" ]; then \
	if [[ $EUID -ne 0 ]]; then \
		echo -e "\n\e[31mInstalling Trick to /usr/local, the default installation location, requires super user privileges."; \
		echo -e "Please log in as a super user to continue.\e[0m\n"; \
		exit 1; \
	fi; \
	fi

	cp -r bin include $(notdir ${TRICK_LIB_DIR}) libexec share ${PREFIX}
	@echo -e "\n\e[32mTrick has been installed successfully to ${PREFIX}.\e[0m\n"

uninstall:
	rm -f ${PREFIX}/bin/trick-CP
	rm -f ${PREFIX}/bin/trick-ICG
	rm -f ${PREFIX}/bin/trick-config
	rm -f ${PREFIX}/bin/trick-dp
	rm -f ${PREFIX}/bin/trick-dre
	rm -f ${PREFIX}/bin/trick-gte
	rm -f ${PREFIX}/bin/trick-gxplot
	rm -f ${PREFIX}/bin/trick-jxplot
	rm -f ${PREFIX}/bin/trick-killsim
	rm -f ${PREFIX}/bin/trick-mm
	rm -f ${PREFIX}/bin/trick-mtv
	rm -f ${PREFIX}/bin/trick-qp
	rm -f ${PREFIX}/bin/trick-sie
	rm -f ${PREFIX}/bin/trick-simcontrol
	rm -f ${PREFIX}/bin/trick-sniffer
	rm -f ${PREFIX}/bin/trick-stripchart
	rm -f ${PREFIX}/bin/trick-trk2ascii
	rm -f ${PREFIX}/bin/trick-trk2csv
	rm -f ${PREFIX}/bin/trick-tv
	rm -f ${PREFIX}/bin/trick-version
	rm -rf ${PREFIX}/include/trick
	rm -rf ${PREFIX}/include/er7_utils
	rm -f ${PREFIX}/$(notdir ${TRICK_LIB_DIR})/liber7_utils.a
	rm -f ${PREFIX}/$(notdir ${TRICK_LIB_DIR})/libtrick.a
	rm -f ${PREFIX}/$(notdir ${TRICK_LIB_DIR})/libtrick_comm.a
	rm -f ${PREFIX}/$(notdir ${TRICK_LIB_DIR})/libtrick_connection_handlers.a
	rm -f ${PREFIX}/$(notdir ${TRICK_LIB_DIR})/libtrick_math.a
	rm -f ${PREFIX}/$(notdir ${TRICK_LIB_DIR})/libtrick_mm.a
	rm -f ${PREFIX}/$(notdir ${TRICK_LIB_DIR})/libtrick_pyip.a
	rm -f ${PREFIX}/$(notdir ${TRICK_LIB_DIR})/libtrick_units.a
	rm -rf ${PREFIX}/libexec/trick
	rm -rf ${PREFIX}/share/doc/trick
	rm -f ${PREFIX}/share/man/man1/trick-CP.1
	rm -f ${PREFIX}/share/man/man1/trick-ICG.1
	rm -f ${PREFIX}/share/man/man1/trick-killsim.1
	rm -rf ${PREFIX}/share/trick

################################################################################
#                    MISCELLANEOUS DEVELOPER UTILITY TARGETS                   #
################################################################################
# ICG all sim_services files (for testing and debugging ICG).
# The -f flag forces io_src files to be regenerated whether or not they need to be.
# Replace -isystem with -I so ICG doesn't skip Trick headers
ICG: TRICK_SYSTEM_CXXFLAGS := $(subst -isystem,-I,$(TRICK_SYSTEM_CXXFLAGS))
ICG: $(ICG_EXE)
	$(ICG_EXE) -f -s -m -n ${TRICK_CXXFLAGS} ${TRICK_SYSTEM_CXXFLAGS} ${TRICK_HOME}/include/trick/files_to_ICG.hh


ICG_EXE: force-icg-build

.PHONY: force-icg-build
force-icg-build: 
	$(MAKE) -C trick_source/codegen/Interface_Code_Gen

# This builds a tricklib share library.
trick_lib: $(SIM_SERV_DIRS) $(UTILS_DIRS) | $(TRICK_LIB_DIR)
	${TRICK_CXX} $(SHARED_LIB_OPT) -o ${TRICK_LIB_DIR}/libtrick.so $(SIM_SERV_OBJS) $(UTILS_OBJS)

# For NASA/JSC developers include optional rules
-include Makefile_jsc_dirs
