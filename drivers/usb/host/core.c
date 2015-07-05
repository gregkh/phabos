#include <asm/delay.h>
#include <phabos/usb/hcd.h>
#include <phabos/utils.h>
#include <phabos/assert.h>
#include <phabos/workqueue.h>

#include <errno.h>
#include <string.h>

static atomic_t dev_id;

static int hub_check_changes(struct usb_device *dev, struct urb *urb);
static int enumerate_device(struct usb_device *dev);
static struct usb_device *usb_device_create(struct usb_hcd *hcd,
                                            struct usb_device *hub);
static struct workqueue *hub_wq;

static void print_descriptor(void *raw_descriptor)
{
    char *header = raw_descriptor;
    struct usb_device_descriptor *dev_desc = raw_descriptor;
    struct usb_string_descriptor *str_desc = raw_descriptor;
    struct usb_config_descriptor *config_desc = raw_descriptor;
    struct usb_interface_descriptor *iface_desc = raw_descriptor;
    struct usb_endpoint_descriptor *endpoint_desc = raw_descriptor;

    switch (header[1]) {
    case 1:
        kprintf("Device:\n");
        kprintf("\tdevice class: %u:%u\n", dev_desc->bDeviceClass, dev_desc->bDeviceSubClass);
        kprintf("\tdevice protocol: %u\n", dev_desc->bDeviceProtocol);
        kprintf("\tmax packet size: %u\n", dev_desc->bMaxPacketSize0);
        kprintf("\tvendor id: %X\n", dev_desc->idVendor);
        kprintf("\tproduct id: %X\n", dev_desc->idProduct);
        kprintf("\tbcd device: %X\n", dev_desc->bcdDevice);
        kprintf("\tmanufacturer index: %u\n", dev_desc->iManufacturer);
        kprintf("\tproduct index: %u\n", dev_desc->iProduct);
        kprintf("\tserial number index: %u\n", dev_desc->iSerialNumber);
        kprintf("\t# configuration: %u\n", dev_desc->bNumConfigurations);
        break;

    case 2:
        kprintf("Configuration:\n");
        kprintf("\ttotal descriptor length: %u\n", config_desc->wTotalLength);
        kprintf("\t# interface: %u\n", config_desc->bNumInterfaces);
        kprintf("\tconfig value: %u\n", config_desc->bConfigurationValue);
        kprintf("\tiConfiguration: %u\n", config_desc->iConfiguration);
        kprintf("\tattributes: %X\n", config_desc->bmAttributes);
        kprintf("\tmax power: %X\n", config_desc->bMaxPower);

        int size = config_desc->wTotalLength;
        while (size > 0) {
            dev_desc = (char*) dev_desc + dev_desc->bLength;
            size -= dev_desc->bLength;
            print_descriptor(dev_desc);
        }
        break;

    case 3:
        kprintf("String: ");
        for (int i = 0; i < str_desc->bLength; i += 2)
            kprintf("%c", ((char*) str_desc->bString)[i]);
        kprintf("\n");
        break;

    case 4:
        kprintf("Interface:\n");
        kprintf("\tinterface #: %u\n", iface_desc->bInterfaceNumber);
        kprintf("\talternate setting: %u\n", iface_desc->bAlternateSetting);
        kprintf("\t# of endpoints: %u\n", iface_desc->bNumEndpoints);
        kprintf("\tinterface class: %u:%u\n", iface_desc->bInterfaceClass, iface_desc->bInterfaceSubClass);
        kprintf("\tinterface protocol: %u\n", iface_desc->bInterfaceProtocol);
        kprintf("\tiInterface: %u\n", iface_desc->iInterface);
        break;

    case 5:
        kprintf("Endpoint:\n");
        kprintf("\tendpoint address: %u\n", endpoint_desc->bEndpointAddress);
        kprintf("\tattributes: %X\n", endpoint_desc->bmAttributes);
        break;
    }
}

static void usb_control_msg_callback(struct urb *urb)
{
    RET_IF_FAIL(urb,);
    semaphore_up(&urb->semaphore);
}

static int usb_control_msg(struct usb_device *dev, uint16_t typeReq,
                           uint16_t wValue, uint16_t wIndex, uint16_t wLength,
                           void *buffer)
{
    int retval;
    struct urb *urb;
    int direction;

    RET_IF_FAIL(dev, -EINVAL);
    RET_IF_FAIL(dev->hcd, -EINVAL);
    RET_IF_FAIL(dev->hcd->driver, -EINVAL);
    RET_IF_FAIL(dev->hcd->driver->urb_enqueue, -EINVAL);

    urb = urb_create(dev);
    RET_IF_FAIL(urb, -ENOMEM);

    direction = (typeReq & 0x8000) ? USB_HOST_DIR_IN : USB_HOST_DIR_OUT;

    urb->complete = usb_control_msg_callback;
    urb->pipe = (USB_HOST_PIPE_CONTROL << 30) | (dev->address << 8) | direction;
    urb->buffer = buffer;
    urb->maxpacket = 0x40;
    urb->length = wLength;

    urb->setup_packet[0] = typeReq >> 8;
    urb->setup_packet[1] = typeReq & 0xff;
    urb->setup_packet[2] = wValue & 0xff;
    urb->setup_packet[3] = wValue >> 8;
    urb->setup_packet[4] = wIndex & 0xff;
    urb->setup_packet[5] = wIndex >> 8;
    urb->setup_packet[6] = wLength & 0xff;
    urb->setup_packet[7] = wLength >> 8;

    retval = dev->hcd->driver->urb_enqueue(dev->hcd, urb);
    if (retval)
        goto out;

    semaphore_down(&urb->semaphore);

out:
    urb_destroy(urb);
    return retval;
}

static void hub_update_status(void *data)
{
    struct urb *urb = data;
    uint8_t change_status = *(uint8_t*) urb->buffer;
    uint32_t status;

    if (!urb->status)
        kprintf("hub changes: %X\n", change_status);

    change_status >>= 1;

    for (int i = 1; change_status; i++, change_status >>= 1) {
        if (!(change_status & 1))
            continue;

        usb_control_msg(urb->device, USB_GET_PORT_STATUS, 0, i, sizeof(status),
                        &status);

        kprintf("port status: %X\n", status);

        if (status & (1 << 16)) {
            usb_control_msg(urb->device, USB_CLEAR_PORT_FEATURE, C_PORT_CONNECTION, i,
                            0, NULL);
            kprintf("Port connection changed\n");

            if (status & (1 << PORT_CONNECTION)) {
                struct usb_device *dev = usb_device_create(urb->device->hcd, urb->device);
                if (!dev)
                    continue;

                dev->port = i;
                dev->speed = USB_SPEED_FULL;
                if (status & (1 << 9))
                    dev->speed = USB_SPEED_LOW;
                if (status & (1 << 10))
                    dev->speed = USB_SPEED_HIGH;

                usb_control_msg(urb->device, USB_SET_PORT_FEATURE, PORT_RESET, i, 0, NULL);

                mdelay(1000);

                usb_control_msg(urb->device, USB_GET_PORT_STATUS, 0, i, sizeof(status),
                                &status);

                enumerate_device(dev);
            } else {
                kprintf("Port disconnected\n");
            }

            usb_control_msg(urb->device, USB_GET_PORT_STATUS, 0, i, sizeof(status),
                            &status);

            kprintf("port status: %X\n", status);
        }

        if (status & (1 << 19)) {
            usb_control_msg(urb->device, USB_CLEAR_PORT_FEATURE,
                            C_PORT_OVER_CURRENT, i, 0, NULL);
            kprintf("Port over current\n");
        }

        if (status & (1 << 20)) {
            usb_control_msg(urb->device, USB_CLEAR_PORT_FEATURE, C_PORT_RESET, i, 0,
                            NULL);
            kprintf("Port reset\n");
        }
    }

    hub_check_changes(urb->device, urb);
}

static void hub_state_changed(struct urb *urb)
{
    kprintf("%s() = %d\n", __func__, urb->status);
    workqueue_queue(hub_wq, hub_update_status, urb);
}

static int hub_check_changes(struct usb_device *dev, struct urb *urb)
{
    RET_IF_FAIL(dev, -EINVAL);
    RET_IF_FAIL(dev->hcd, -EINVAL);
    RET_IF_FAIL(dev->hcd->driver, -EINVAL);
    RET_IF_FAIL(dev->hcd->driver->urb_enqueue, -EINVAL);

    if (!urb) {
        urb = urb_create(dev);
        RET_IF_FAIL(urb, -ENOMEM);

        urb->length = 4;
        urb->buffer = kmalloc(urb->length, MM_DMA);
        if (!urb->buffer) {
            urb_destroy(urb);
            return -ENOMEM;
        }
    }

    urb->status = 0;
    urb->actual_length = 0;
    urb->complete = hub_state_changed;
    urb->pipe = (USB_HOST_PIPE_INTERRUPT << 30) | (1 << 15) |
                (dev->address << 8) | USB_HOST_DIR_IN;
    urb->maxpacket = 0x40;
    urb->flags = 1;

    return dev->hcd->driver->urb_enqueue(dev->hcd, urb);
}

static int enumerate_hub(struct usb_device *hub)
{
    int retval;
    struct usb_hub_descriptor *desc;
    struct usb_device *dev;
    uint32_t status;

    retval = usb_control_msg(hub, USB_DEVICE_SET_CONFIGURATION, 1,
                             0, 0, NULL);
    if (retval)
        return retval; // FIXME: unpower device port

    desc = kmalloc(sizeof(*desc), 0);
    RET_IF_FAIL(desc, -ENOMEM);

    retval = usb_control_msg(hub, USB_GET_HUB_DESCRIPTOR, 0, 0, sizeof(*desc),
                             desc);
    if (retval)
        return retval;

    kprintf("%s: found new hub with %u ports.\n", hub->hcd->device.name,
                                                  desc->bNbrPorts);

    for (int i = 1; i <= desc->bNbrPorts; i++) {
        usb_control_msg(hub, USB_SET_PORT_FEATURE, PORT_POWER, i, 0, NULL);

        mdelay(desc->bPwrOn2PwrGood * 2);
        mdelay(1000);

        usb_control_msg(hub, USB_GET_PORT_STATUS, 0, i, sizeof(status),
                        &status);

        if (!(status & (1 << PORT_CONNECTION)))
            continue;

        dev = usb_device_create(hub->hcd, hub);
        if (!dev)
            continue;

        dev->port = i;
        dev->speed = USB_SPEED_FULL;
        if (status & (1 << 9))
            dev->speed = USB_SPEED_LOW;
        if (status & (1 << 10))
            dev->speed = USB_SPEED_HIGH;

        usb_control_msg(hub, USB_SET_PORT_FEATURE, PORT_RESET, i, 0, NULL);

        mdelay(1000);

        usb_control_msg(hub, USB_GET_PORT_STATUS, 0, i, sizeof(status),
                        &status);

        if (status & (1 << 16)) {
            usb_control_msg(hub, USB_CLEAR_PORT_FEATURE, C_PORT_CONNECTION, i,
                            0, NULL);
        }

        if (status & (1 << 20)) {
            usb_control_msg(hub, USB_CLEAR_PORT_FEATURE, C_PORT_RESET, i, 0,
                            NULL);
        }

        enumerate_device(dev);
    }

    hub_check_changes(hub, NULL);

    return 0;
}

static int enumerate_device(struct usb_device *dev)
{
    int retval;
    int address;
    struct usb_device_descriptor *desc;
    struct usb_class_driver *klass;

    desc = kmalloc(sizeof(*desc), 0);
    RET_IF_FAIL(desc, -ENOMEM);

    retval = usb_control_msg(dev, USB_DEVICE_GET_DESCRIPTOR,
                             USB_DESCRIPTOR_DEVICE << 8, 0, sizeof(*desc),
                             desc);
    if (retval)
        goto out; // FIXME: unpower device port

#if 0
    klass = find_class_driver(desc->bDeviceClass, desc->bDeviceSubClass,
                              desc->bDeviceProtocol);
    if (!klass)
        return -ENODEV;
#endif

    address = atomic_inc(&dev_id);

    retval = usb_control_msg(dev, USB_DEVICE_SET_ADDRESS, address, 0, 0, NULL);
    if (retval)
        goto out; // FIXME: unpower device port

    dev->address = address;

    kprintf("Device ID: %d\n", dev->address);
    print_descriptor(desc);

    if (desc->bDeviceClass == 9)// FIXME to remove later
        enumerate_hub(dev);

out:
    kfree(desc);
    return retval;
}

static struct usb_device *usb_device_create(struct usb_hcd *hcd,
                                            struct usb_device *hub)
{
    struct usb_device *dev = kzalloc(sizeof(*dev), 0);
    RET_IF_FAIL(dev, NULL);

    dev->speed = USB_SPEED_HIGH;
    dev->hcd = hcd;
    dev->hub = hub;

    return dev;
}

static int enumerate_bus(struct usb_hcd *hcd)
{
    int retval;
    struct usb_hub_descriptor desc;
    struct usb_device *dev;
    uint32_t status;

    RET_IF_FAIL(hcd, -EINVAL);
    RET_IF_FAIL(hcd->driver, -EINVAL);
    RET_IF_FAIL(hcd->driver->start, -EINVAL);
    RET_IF_FAIL(hcd->driver->hub_control, -EINVAL);

    retval = hcd->driver->start(hcd);
    if (retval)
        return retval;

    retval = hcd->driver->hub_control(hcd, USB_GET_HUB_DESCRIPTOR, 0, 0,
                                      sizeof(desc), (char*) &desc);
    if (retval)
        return retval;

    kprintf("%s: found new hub with %u ports.\n", hcd->device.name,
                                                  desc.bNbrPorts);

    for (int i = 1; i <= desc.bNbrPorts; i++) {
        hcd->driver->hub_control(hcd, USB_SET_PORT_FEATURE, PORT_POWER, i, 0,
                                 NULL);

        mdelay(desc.bPwrOn2PwrGood * 2);
        mdelay(1000);

        retval = hcd->driver->hub_control(hcd, USB_GET_PORT_STATUS, 0, i, 4,
                                          (char*) &status);
        if (retval)
            continue;

        if (!(status & (1 << PORT_CONNECTION))) {
            hcd->driver->hub_control(hcd, USB_CLEAR_PORT_FEATURE, PORT_POWER,
                                     i, 0, NULL);
            continue;
        }

        dev = usb_device_create(hcd, NULL);
        if (!dev)
            continue;

        dev->speed = USB_SPEED_FULL;
        if (status & (1 << 9))
            dev->speed = USB_SPEED_LOW;
        if (status & (1 << 10))
            dev->speed = USB_SPEED_HIGH;

        hcd->driver->hub_control(hcd, USB_SET_PORT_FEATURE, PORT_RESET, i, 0,
                                 NULL);

        enumerate_device(dev);
    }

    return 0;
}

void enumerate_everything(void *data)
{
    enumerate_bus(data);
    while (1);
}

int usb_hcd_register(struct usb_hcd *hcd)
{
    atomic_init(&dev_id, 1);
    hub_wq = workqueue_create("usb-hubd");
    task_run(enumerate_everything, hcd, 0);
    return 0;
}