include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
include $(MTOP)/defs.mk

TO_MAKE := dlist.mk prbtree

ifdef OS_WINXX
TO_MAKE += buildnumber
endif

include $(MTOP)/parallel.mk
