include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/defs.mk
$(DEFINE_TARGETS)

# installation

LIBRARY_NAME := dlist
LIBRARY_HDIR :=
LIBRARY_HEADERS := include/dlist.h

define PRBTREE_PC_COMMENT
Author:  $(VENDOR_NAME)
License: $(PRODUCT_LICENCE)
endef

# generate contents of pkg-config .pc-file
# $1 - static or dynamic library name
# $2 - library variant
PRBTREE_PC_GENERATOR = $(call PKGCONF_DEF_TEMPLATE,$1,$(MODVER),$(PRBTREE_DESC),$(PRBTREE_PC_COMMENT),$(VENDOR_URL),,,,$(addprefix \
  -D,$(DEFINES))$(call VARIANT_CFLAGS,$2))

LIBRARY_PC_GEN := PRBTREE_PC_GENERATOR

include $(MTOP)/exts/install_lib.mk
