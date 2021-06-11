include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := tests

this_srcs += $(call prorab-src-dir, .)

this_ldlibs += -ljsondom -lutki -ltst
this_ldflags += -L$(d)../../src/out/$(c)

this_no_install := true

$(eval $(prorab-build-app))

include $(d)../test_target.mk

# add dependency on libjsondom
$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libjsondom$(dot_so))

$(eval $(call prorab-include, ../../src/makefile))
