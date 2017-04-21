include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/defs.mk
$(DEFINE_TARGETS)

# installation

LIBRARY_NAME := dlist
LIBRARY_HDIR :=
LIBRARY_HEADERS := include/dlist.h

include $(MTOP)/exts/install_lib.mk
