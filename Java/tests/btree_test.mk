include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
include $(MTOP)/java.mk
include $(MTOP)/exts/jtest.mk

JAR      := BtreeTest
JARS     := $(EMCOLLECTIONS_JAR_NAME)
JSRC     := BtreeTest.java TreeTest.java

$(call DO_TEST_JAR,BtreeTest)

$(DEFINE_TARGETS)
