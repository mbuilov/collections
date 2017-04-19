include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
TO_MAKE := lib.mk
ifneq ($(filter check tests clean,$(MAKECMDGOALS)),)
TO_MAKE += tests
endif
include $(MTOP)/parallel.mk
