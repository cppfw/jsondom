include prorab.mk

this_name := parsejson

this_out_dir := build

this_cxxflags += -Wall
this_cxxflags += -Werror
this_cxxflags += -funsigned-char # the 'char' type is unsigned
this_cxxflags += -fstrict-aliasing
this_cxxflags += -std=c++14
this_cxxflags += -g

ifeq ($(debug),true)
	this_cxxflags += -DDEBUG
else
	this_cxxflags += -O3
endif

this_srcs += $(call prorab-src-dir, .)

this_ldlibs += -ljsondom -lpapki -lclargs -lstdc++
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

this_dirs := $(subst /, ,$(d))
this_test := $(word $(words $(this_dirs)),$(this_dirs))

this_samples := $(shell ls $(d)testdata/*.json)

define this_rules
test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(this_test)
$(.RECIPEPREFIX)$(a)cp $(d)../../src/build/*.dll $(d)build || true
$(.RECIPEPREFIX)$(a)for f in $(this_samples); do \
	    echo "checking file $$$$f" && \
	    LD_LIBRARY_PATH=$(d)../../src/build DYLD_LIBRARY_PATH=$$$$LD_LIBRARY_PATH $(d)./build/parsejson --out-file=$(d)out.json $$$$f && \
	    cmp $(d)out.json $$$$f.cmp; \
	    if [ $$$$? -ne 0 ]; then myci-error.sh "parsed JSON did not match etalon sample"; exit 1; fi \
	done
$(.RECIPEPREFIX)$(a)rm $(d)out.json
$(.RECIPEPREFIX)@myci-passed.sh
endef
$(eval $(this_rules))

# add dependency on libjsondom
$(prorab_this_name): $(abspath $(d)../../src/build/libjsondom$(dot_so))

$(eval $(call prorab-include, ../../src/makefile))
