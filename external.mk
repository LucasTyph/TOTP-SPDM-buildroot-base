TOTP_SPDM_VERSION = 0.1
TOTP_SPDM_SITE = $(BR2_EXTERNAL_TOTP_SPDM_PATH)
TOTP_SPDM_SITE_METHOD = local
TOTP_SPDM_LICENSE = GPL-2.0
TOTP_SPDM_LICENSE_FILES = COPYING

$(eval $(kernel-module))
$(eval $(generic-package))
