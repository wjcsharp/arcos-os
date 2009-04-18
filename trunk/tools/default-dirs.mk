include ./dirs

ifndef DIRS
$(error Your ./dirs file must define the DIRS= macro.)
endif

.PHONY: $(DIRS)
.PHONY: all
.PHONY: clean

all clean: $(DIRS)

$(DIRS):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

