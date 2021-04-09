include prorab.mk

$(eval $(call prorab-config, ../../config))

this_name := parsejson

this_srcs += $(call prorab-src-dir, .)

this_ldlibs += -ljsondom -lpapki -lclargs -lutki
this_ldflags += -L$(d)../../src/out/$(c)

this_no_install := true

$(eval $(prorab-build-app))

this_dirs := $(subst /, ,$(d))
this_test := $(word $(words $(this_dirs)),$(this_dirs))

this_samples := $(shell ls $(d)testdata/*.json)

define this_rules
test:: $(prorab_this_name)
$(.RECIPEPREFIX)@myci-running-test.sh $(this_test)
$(.RECIPEPREFIX)$(a)cp $(d)../../src/out/$(c)/*.dll $(d)out/$(c) || true
$(.RECIPEPREFIX)$(a)for f in $(this_samples); do \
	    echo "checking file $$$$f" && \
	    LD_LIBRARY_PATH=$(d)../../src/out/$(c) DYLD_LIBRARY_PATH=$$$$LD_LIBRARY_PATH $(d)./out/$(c)/parsejson --out-file=$(d)out.json $$$$f && \
	    cmp $(d)out.json $$$$f.cmp; \
	    if [ $$$$? -ne 0 ]; then myci-error.sh "parsed JSON did not match etalon sample"; exit 1; fi \
	done
$(.RECIPEPREFIX)$(a)rm $(d)out.json
$(.RECIPEPREFIX)@myci-passed.sh
endef
$(eval $(this_rules))

# add dependency on libjsondom
$(prorab_this_name): $(abspath $(d)../../src/out/$(c)/libjsondom$(dot_so))

$(eval $(call prorab-include, ../../src/makefile))
