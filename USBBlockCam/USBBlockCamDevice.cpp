#include "USBBlockCamDevice.h"
#include <IOKit/IOLib.h>
#include <IOKit/usb/IOUSBHostDevice.h>

#define LOG(fmt, ...) IOLog("USBBlockCamDevice: " fmt "\n", ##__VA_ARGS__)

OSDefineMetaClassAndStructors(USBBlockCamDevice, IOService)

static const uint16_t kTargetVendorID  = 0x174F;
static const uint16_t kTargetProductID = 0x246A;
static const uint8_t  kHubDescriptorType = 0x29;
static const uint8_t  kCameraHubPort = 4;

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

static void hideCameraHubPort(IOUSBHostDevice * device, IOService * client)
{
    if (!device) return;

    const StandardUSB::DeviceDescriptor * deviceDescriptor = device->getDeviceDescriptor();
    if (!deviceDescriptor || deviceDescriptor->bDeviceClass != 9) {
        return;
    }

    OSNumber * vendor  = OSDynamicCast(OSNumber, device->getProperty("idVendor"));
    OSNumber * product = OSDynamicCast(OSNumber, device->getProperty("idProduct"));
    uint16_t vid = vendor ? vendor->unsigned16BitValue() : 0;
    uint16_t pid = product ? product->unsigned16BitValue() : 0;

    bool opened = device->open(client);
    uint16_t length = 9;
    const StandardUSB::Descriptor * hubDescriptor =
        device->getDescriptor(kHubDescriptorType, length, 0, 0, kRequestTypeClass);

    if (!hubDescriptor || length < 3 || hubDescriptor->bLength < 3) {
        LOG("hub: descriptor read failed (vid 0x%04x pid 0x%04x open=%d)",
            vid, pid, opened);
        if (opened) device->close(client);
        return;
    }

    const uint8_t * raw = reinterpret_cast<const uint8_t *>(hubDescriptor);
    uint8_t ports = raw[2];
    LOG("hub: vid 0x%04x pid 0x%04x ports=%u open=%d", vid, pid, ports, opened);

    if (ports == kCameraHubPort) {
        uint8_t patched[9];
        for (unsigned int i = 0; i < sizeof(patched); i++) {
            patched[i] = (i < hubDescriptor->bLength) ? raw[i] : 0;
        }
        patched[0] = (uint8_t)sizeof(patched);
        patched[2] = kCameraHubPort - 1;

        IOReturn result = device->cacheDescriptor(
            reinterpret_cast<const StandardUSB::Descriptor *>(patched),
            (uint16_t)sizeof(patched), 0, 0);
        LOG("hub: hiding port %u, cacheDescriptor -> 0x%x", kCameraHubPort, result);
    }

    if (opened) device->close(client);
}

IOService * USBBlockCamDevice::probe(IOService * provider, SInt32 * score)
{
    IOUSBHostDevice * device = OSDynamicCast(IOUSBHostDevice, provider);
    if (device) {
        hideCameraHubPort(device, this);
    }

    if (!isTargetDevice(provider)) {
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
