################################################################################
#
# strace
#
################################################################################

ifeq ($(BR2_kvx),y)
STRACE_VERSION = 19f3bb2e38be375d5febfbf6e517623a3da5d703
STRACE_SITE = $(call github,kalray,strace,$(STRACE_VERSION))
STRACE_AUTORECONF = YES
define STRACE_BOOTSTRAP_HOOK
	$(SED) 's%^\(autoreconf.*\)%#\1%' $(@D)/bootstrap
	(cd $(@D); ./bootstrap)
endef
STRACE_POST_PATCH_HOOKS += STRACE_BOOTSTRAP_HOOK
else
STRACE_VERSION = 5.12
STRACE_SOURCE = strace-$(STRACE_VERSION).tar.xz
STRACE_SITE = https://strace.io/files/$(STRACE_VERSION)
endif
STRACE_LICENSE = LGPL-2.1+
STRACE_LICENSE_FILES = COPYING LGPL-2.1-or-later
STRACE_CPE_ID_VENDOR = strace_project
STRACE_CONF_OPTS = --enable-mpers=no

ifeq ($(BR2_PACKAGE_LIBUNWIND),y)
STRACE_DEPENDENCIES += libunwind
STRACE_CONF_OPTS += --with-libunwind
else
STRACE_CONF_OPTS += --without-libunwind
endif

# Demangling symbols in stack trace needs libunwind and libiberty.
ifeq ($(BR2_PACKAGE_BINUTILS)$(BR2_PACKAGE_LIBUNWIND),yy)
STRACE_DEPENDENCIES += binutils
STRACE_CONF_OPTS += --with-libiberty=check
else
STRACE_CONF_OPTS += --without-libiberty
endif

ifeq ($(BR2_PACKAGE_PERL),)
define STRACE_REMOVE_STRACE_GRAPH
	rm -f $(TARGET_DIR)/usr/bin/strace-graph
endef

STRACE_POST_INSTALL_TARGET_HOOKS += STRACE_REMOVE_STRACE_GRAPH
endif

$(eval $(autotools-package))
