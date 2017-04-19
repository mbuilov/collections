include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
include $(MTOP)/java.mk
include $(MTOP)/exts/jtest.mk

JAR      := PtreeTest
JARS     := $(EMCOLLECTIONS_JAR_NAME)
JSRC     := PtreeTest.java PtreeTests.java TreeTest.java

$(call DO_TEST_JAR,PtreeTest)

$(DEFINE_TARGETS)
