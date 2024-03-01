include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := basic

this_srcs := $(call prorab-src-dir,.)

this_no_install := true

this_cxxflags += -isystem ../../src
this_ldflags += -L ../../src/out/$(c)

this_ldlibs += -ljsondom -lutki

$(eval $(prorab-build-app))

# add dependency on libjsondom
$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libjsondom$(dot_so))

$(eval $(call prorab-include, ../../src/makefile))
