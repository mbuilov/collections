include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
include $(MTOP)/java.mk
include $(MTOP)/exts/jtest.mk

JAR      := PrbtreeTest
JARS     := $(EMCOLLECTIONS_JAR_NAME)
JSRC     := PrbtreeTest.java PtreeTests.java TreeTest.java

$(call DO_TEST_JAR,PrbtreeTest)

$(DEFINE_TARGETS)
