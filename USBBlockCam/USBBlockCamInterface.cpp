#include "USBBlockCamInterface.h"
#include <IOKit/IOLib.h>
#include <IOKit/usb/IOUSBHostDevice.h>

#define LOG(fmt, ...) IOLog("USBBlockCamInterface: " fmt "\n", ##__VA_ARGS__)

OSDefineMetaClassAndStructors(USBBlockCamInterface, IOUSBHostInterface)

static const uint16_t kTargetVendorID  = 0x174F;
static const uint16_t kTargetProductID = 0x246A;

static bool parentIsTargetCamera(IOService * provider)
{
    if (!provider) return false;

    IOUSBHostDevice * device = OSDynamicCast(IOUSBHostDevice, provider->getParentEntry(gIOServicePlane));
    if (!device) {
        device = OSDynamicCast(IOUSBHostDevice, provider->getProvider());
    }
    if (!device) return false;

    OSNumber * vendor  = OSDynamicCast(OSNumber, device->getProperty("idVendor"));
    OSNumber * product = OSDynamicCast(OSNumber, device->getProperty("idProduct"));
    if (!vendor || !product) return false;

    return (vendor->unsigned16BitValue() == kTargetVendorID &&
            product->unsigned16BitValue() == kTargetProductID);
}

IOService * USBBlockCamInterface::probe(IOService * provider, SInt32 * score)
{
    if (!parentIsTargetCamera(provider)) {
        return nullptr;
    }

    LOG("probe: CLAIMING interface of target camera (VID 0x%04x PID 0x%04x)",
        kTargetVendorID, kTargetProductID);

    if (score) *score += 1000000;

    return IOUSBHostInterface::probe(provider, score);
}

bool USBBlockCamInterface::start(IOService * provider)
{
    if (!IOUSBHostInterface::start(provider)) return false;
    LOG("start: claimed, doing nothing (blocks UVC/CDC interface drivers)");
    return true;
}

void USBBlockCamInterface::stop(IOService * provider)
{
    LOG("stop");
    IOUSBHostInterface::stop(provider);
}
