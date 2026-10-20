#include <mach/mach_types.h>

static kern_return_t kext_start(kmod_info_t *ki, void *data) {
    (void)ki;
    (void)data;
    return KERN_SUCCESS;
}

static kern_return_t kext_stop(kmod_info_t *ki, void *data) {
    (void)ki;
    (void)data;
    return KERN_FAILURE;
}

kmod_info_t kmod_info = {
    .info_version     = KMOD_INFO_VERSION,
    .id               = -1U,
    .name             = "com.opencode.usbblockcam",
    .version          = "1.0.0",
    .reference_count  = -1,
    .start            = kext_start,
    .stop             = kext_stop,
};
