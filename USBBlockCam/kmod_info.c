#include <mach/mach_types.h>

extern kern_return_t _start(kmod_info_t *ki, void *data);
extern kern_return_t _stop(kmod_info_t *ki, void *data);

kmod_info_t kmod_info = {
    .info_version     = KMOD_INFO_VERSION,
    .id               = -1U,
    .name             = "com.opencode.usbblockcam",
    .version          = "1.0.0",
    .reference_count  = -1,
    .start            = _start,
    .stop             = _stop,
};
