################################################################################
#
# libc-test
#
################################################################################

LIBC_TEST_VERSION = 445dc15f0758be6b4666149c1aca8af2a69209df
LIBC_TEST_SITE = git://nsz.repo.hu:49100/repo/libc-test
LIBC_TEST_LICENSE = standard MIT license, BSD (src/math/ucb/*), GPL (src/math/crlibm/*)
LIBC_TEST_LICENSE_FILES = COPYRIGHT

define LIBC_TEST_CONFIGURE
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) config.mak
endef

define LIBC_TEST_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) \
		CC="$(TARGET_CC)" \
		CROSS_COMPILE="$(TARGET_CROSS)"
endef

define LIBC_TEST_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/libc-test
	cp -rf $(@D)/*  $(TARGET_DIR)/usr/lib/libc-test
endef

$(eval $(generic-package))
