include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
include $(MTOP)/java.mk
include $(MTOP)/exts/jtest.mk

JAR      := DlistTest
JARS     := $(EMCOLLECTIONS_JAR_NAME)
JSRC     := DlistTest.java

$(call DO_TEST_JAR,DlistTest)

$(DEFINE_TARGETS)
