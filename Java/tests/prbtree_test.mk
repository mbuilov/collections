include $(MTOP)/java.mk
JAR      := PrbtreeTest
JARS     := collections
JSRC     := $(addprefix com/itr/collections/,PrbtreeTest.java PtreeTests.java TreeTest.java)
MANIFEST := PrbtreeTest/MANIFEST.MF
$(DEFINE_TARGETS)
