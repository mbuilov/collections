include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/java.mk

JAR      := $(EMCOLLECTIONS_JAR_NAME)
JSRC     := $(addprefix emcollections/,Btree.java Dlist.java Prbtree.java Ptree.java)
MANIFEST := $(GEN_DIR)/$(EMCOLLECTIONS_JAR_NAME).mf

$(call ADD_GENERATED,$(MANIFEST))

$(MANIFEST):
	$(call SUP,GEN,$@)$(call ECHO,$(call \
  JAR_VERSION_MANIFEST,emcollections/,$(COLLECTIONS_DESC),$(\
)$(PRODUCT_VER),$(VENDOR_NAME),emcollections,$(PRODUCT_VER),$(VENDOR_NAME))) > $@

$(DEFINE_TARGETS)
