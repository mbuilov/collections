include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
include $(MTOP)/c.mk

SRC     := prbtree.c
INCLUDE := ../include
USE     := cmn_headers.mk
MODVER  := $(PRBTREE_VER)

ifndef NO_SHARED
DLL     := $(PRBTREE_LIB_NAME) $(PRBTREE_DLL_VARIANTS)
DEFINES := PRBTREE_EXPORTS=$(DLL_EXPORTS_DEFINE)
endif

$(call MAKE_CONTINUE,SRC INCLUDE USE MODVER)

ifndef NO_STATIC
LIB     := $(PRBTREE_LIB_NAME) $(PRBTREE_LIB_VARIANTS)
endif

$(DEFINE_TARGETS)

# installation

DLL := $(if $(NO_SHARED),,$(PRBTREE_LIB_NAME) $(PRBTREE_DLL_VARIANTS))
LIB := $(if $(NO_STATIC),,$(PRBTREE_LIB_NAME) $(PRBTREE_LIB_VARIANTS))

LIBRARY_NAME := prbtree
LIBRARY_HDIR :=
LIBRARY_HEADERS := include/btree.h include/prbtree.h

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
