include prorab.mk

this_name := basic

this_out_dir := build

this_srcs := $(call prorab-src-dir,.)

this_no_install := true

this_ldflags += -L$(d)../../src/build

this_ldlibs += -lstdc++
this_ldlibs += -ljsondom

$(eval $(prorab-build-app))

# add dependency on libjsondom
$(prorab_this_name): $(abspath $(d)../../src/build/libjsondom$(dot_so))

$(eval $(call prorab-include, ../../src/makefile))
