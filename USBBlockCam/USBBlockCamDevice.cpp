#include "USBBlockCamDevice.h"
#include <IOKit/IOLib.h>
#include <libkern/OSByteOrder.h>

#define LOG(fmt, ...) IOLog("USBBlockCamDevice: " fmt "\n", ##__VA_ARGS__)

OSDefineMetaClassAndStructors(USBBlockCamDevice, IOService)

static const uint16_t kTargetVendorID  = 0x174F;
static const uint16_t kTargetProductID = 0x246A;

static bool isTargetDevice(IOService * provider)
{
    if (!provider) return false;

    OSNumber * vendor  = OSDynamicCast(OSNumber, provider->getProperty("idVendor"));
    OSNumber * product = OSDynamicCast(OSNumber, provider->getProperty("idProduct"));
    if (!vendor || !product) {
        return false;
    }

    return (vendor->unsigned16BitValue() == kTargetVendorID &&
            product->unsigned16BitValue() == kTargetProductID);
}

IOService * USBBlockCamDevice::probe(IOService * provider, SInt32 * score)
{
    if (!isTargetDevice(provider)) {
        LOG("probe: not target, declining");
        return nullptr;
    }

    LOG("probe: CLAIMING target camera (VID 0x%04x PID 0x%04x)",
        kTargetVendorID, kTargetProductID);

    if (score) *score += 1000000;

    return IOService::probe(provider, score);
}

bool USBBlockCamDevice::start(IOService * provider)
{
    if (!IOService::start(provider)) {
        LOG("start: IOService::start FAILED");
        return false;
    }
    LOG("start: claimed, doing NO composite/CDC setup");
    return true;
}

void USBBlockCamDevice::stop(IOService * provider)
{
    LOG("stop");
    IOService::stop(provider);
}
