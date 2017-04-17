include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
include $(MTOP)/defs.mk

TO_MAKE := lib.mk

ifdef OS_WINXX
TO_MAKE += version
endif

ifneq ($(filter check tests clean,$(MAKECMDGOALS)),)
TO_MAKE += test.mk
endif

include $(MTOP)/parallel.mk
