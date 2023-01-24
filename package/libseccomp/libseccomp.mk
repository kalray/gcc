################################################################################
#
# libseccomp
#
################################################################################

LIBSECCOMP_VERSION = 2.4.4
LIBSECCOMP_SITE = https://github.com/seccomp/libseccomp/releases/download/v$(LIBSECCOMP_VERSION)
LIBSECCOMP_LICENSE = LGPL-2.1
LIBSECCOMP_LICENSE_FILES = LICENSE
LIBSECCOMP_CPE_ID_VENDOR = libseccomp_project
LIBSECCOMP_INSTALL_STAGING = YES
LIBSECCOMP_AUTORECONF = YES
LIBSECCOMP_CONF_OPTS += --enable-static

$(eval $(autotools-package))
