include prorab.mk

this_name := tests

this_out_dir := build

this_cxxflags += -Wall
this_cxxflags += -Werror
this_cxxflags += -fstrict-aliasing
this_cxxflags += -std=c++14
this_cxxflags += -g

ifeq ($(debug),true)
	this_cxxflags += -DDEBUG
else
	this_cxxflags += -O3
endif

this_srcs += $(call prorab-src-dir, .)

this_ldlibs += -ljsondom -lstdc++
this_ldflags += -L$(d)../../src/build

ifeq ($(os), linux)
    this_cxxflags += -fPIC
    this_ldlibs +=
else ifeq ($(os), macosx)
    this_cxxflags += -stdlib=libc++ # this is needed to be able to use c++11 std lib
    this_ldlibs += -lc++
else ifeq ($(os),windows)
endif

this_no_install := true

$(eval $(prorab-build-app))

include $(d)../test_target.mk

# add dependency on libjsondom
$(prorab_this_name): $(abspath $(d)../../src/build/libjsondom$(dot_so))

$(eval $(call prorab-include, ../../src/makefile))
