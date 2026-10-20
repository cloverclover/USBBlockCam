#ifndef USB_BLOCK_CAM_DEVICE_H
#define USB_BLOCK_CAM_DEVICE_H

#include <IOKit/IOService.h>
#include <IOKit/usb/IOUSBHostDevice.h>

class USBBlockCamDevice : public IOUSBHostDevice
{
    OSDeclareDefaultStructors(USBBlockCamDevice)

public:
    virtual IOService * probe(IOService * provider, SInt32 * score) override;
    virtual bool start(IOService * provider) override;
    virtual void stop(IOService * provider) override;
};

#endif
