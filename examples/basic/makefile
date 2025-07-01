include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := basic

this_srcs := $(call prorab-src-dir,.)

this_no_install := true

this_cxxflags += -isystem ../../src

this_ldlibs += -l utki$(this_dbg)

this_ldlibs += ../../src/out/$(c)/libjsondom$(this_dbg)$(dot_so)

$(eval $(prorab-build-app))

$(eval $(call prorab-include, ../../src/makefile))
