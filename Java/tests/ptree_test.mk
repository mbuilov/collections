include $(MTOP)/java.mk
JAR      := PtreeTest
JARS     := collections
JSRC     := $(addprefix com/itr/collections/,PtreeTest.java PtreeTests.java TreeTest.java)
MANIFEST := PtreeTest/MANIFEST.MF
$(DEFINE_TARGETS)
