TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)
JAVA_BUILD_DIR = ${TRICK_HOME}/libexec/trick/java/build


include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

all:
	@echo "[32mBuilding java application...[0m"
	@${MVN} package -q -Dmaven.test.skip -Dcmake=false -Dmaven.wagon.http.retryHandler.count=15

test-headless:
	@echo "[32mBuilding gui-less java tests...[0m"
	@${MVN} test -Dcmake=false -Dmaven.wagon.http.retryHandler.count=15 -Djava.awt.headless=true

test:
	@echo "[32mBuilding java tests...[0m"
	@${MVN} test -Dcmake=false -Dmaven.wagon.http.retryHandler.count=15 -Djava.awt.headless=false

javadoc:
	${MVN} javadoc:javadoc

clean:
	rm -rf ${JAVA_BUILD_DIR}
	@echo java build directory cleaned.
