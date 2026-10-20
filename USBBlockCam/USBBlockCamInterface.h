#ifndef USB_BLOCK_CAM_INTERFACE_H
#define USB_BLOCK_CAM_INTERFACE_H

#include <IOKit/IOService.h>
#include <IOKit/usb/IOUSBHostInterface.h>

class USBBlockCamInterface : public IOUSBHostInterface
{
    OSDeclareDefaultStructors(USBBlockCamInterface)

public:
    virtual IOService * probe(IOService * provider, SInt32 * score) override;
    virtual bool start(IOService * provider) override;
    virtual void stop(IOService * provider) override;
};

#endif
