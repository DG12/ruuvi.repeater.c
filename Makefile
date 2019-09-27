.PHONY: all sync ruuvitag_b clean

all: sync clean ruuvitag_b

sync:
	@echo Synchronizing GIT...
	# TODO: reject if repo is not clean
	git submodule update --init --recursive
	git submodule sync --recursive
	git submodule update --init --recursive


ruuvitag_b:
	@echo build FW
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DNDEBUG
	targets/ruuvitag_b/armgcc/package.sh -n ruuvirepeater_default
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DDEBUG RUN_TESTS=-DRUUVI_RUN_TESTS
	targets/ruuvitag_b/armgcc/package.sh -n ruuvirepeater_test

clean:
	@echo cleaning build files…
	$(MAKE) -C targets/ruuvitag_b/armgcc clean

