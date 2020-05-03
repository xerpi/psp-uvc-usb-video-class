#ifndef __USB_H
#define __USB_H

#include <pspsdk.h>
#include <stddef.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
 * PSVita compatibility definitions
 */
#define SceUdcdStringDescriptor StringDescriptor
#define SceUdcdDeviceDescriptor DeviceDescriptor
#define SceUdcdEndpointDescriptor EndpointDescriptor
#define SceUdcdInterfaceDescriptor InterfaceDescriptor
#define SceUdcdConfigDescriptor ConfigDescriptor
#define SceUdcdConfiguration UsbConfiguration
#define SceUdcdInterface UsbInterface
#define SceUdcdInterfaceSettings InterfaceSettings
#define SceUdcdEndpoint UsbEndpoint
#define SceUdcdDriver UsbDriver

/*
 * Device and/or Interface Class codes
 */
#define USB_CLASS_PER_INTERFACE         0       /* for DeviceClass */
#define USB_CLASS_AUDIO                 1
#define USB_CLASS_COMM                  2
#define USB_CLASS_HID                   3
#define USB_CLASS_PRINTER               7
#define USB_CLASS_PTP                   6
#define USB_CLASS_MASS_STORAGE          8
#define USB_CLASS_HUB                   9
#define USB_CLASS_DATA                  10
#define USB_CLASS_VIDEO                 14
#define USB_CLASS_VENDOR_SPEC           0xff

/*
 * Descriptor types
 */
#define USB_DT_DEVICE                   0x01
#define USB_DT_CONFIG                   0x02
#define USB_DT_STRING                   0x03
#define USB_DT_INTERFACE                0x04
#define USB_DT_ENDPOINT                 0x05
#define USB_DT_DEVQUAL                  0x06
#define USB_DT_OTHERSPEED               0x07

/** Control message request type bitmask
 */
#define USB_CTRLTYPE_DIR_MASK	          0x80
#define USB_CTRLTYPE_DIR_HOST2DEVICE    (0 << 7)
#define USB_CTRLTYPE_DIR_DEVICE2HOST    (1 << 7)
#define USB_CTRLTYPE_TYPE_MASK          0x60
#define USB_CTRLTYPE_TYPE_STANDARD      (0 << 5)
#define USB_CTRLTYPE_TYPE_CLASS         (1 << 5)
#define USB_CTRLTYPE_TYPE_VENDOR        (2 << 5)
#define USB_CTRLTYPE_TYPE_RESERVED      (3 << 5)
#define USB_CTRLTYPE_REC_MASK           0x1f
#define USB_CTRLTYPE_REC_DEVICE         0
#define USB_CTRLTYPE_REC_INTERFACE      1
#define USB_CTRLTYPE_REC_ENDPOINT       2
#define USB_CTRLTYPE_REC_OTHER          3

/*
 * Descriptor sizes per descriptor type
 */
#define USB_DT_DEVICE_SIZE              18
#define USB_DT_CONFIG_SIZE              9
#define USB_DT_INTERFACE_SIZE           9
#define USB_DT_ENDPOINT_SIZE            7
#define USB_DT_DEVQUAL_SIZE             10

/*
 * Endpoint types and masks
 */
#define USB_ENDPOINT_ADDRESS_MASK       0x0f    /* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK           0x80

#define USB_ENDPOINT_IN	                0x80
#define USB_ENDPOINT_OUT                0x00

#define USB_FEATURE_ENDPOINT_HALT       0

#define USB_ENDPOINT_TYPE_MASK          0x03    /* in bmAttributes */
#define USB_ENDPOINT_TYPE_CONTROL       0
#define USB_ENDPOINT_TYPE_ISOCHRONOUS   1
#define USB_ENDPOINT_TYPE_BULK          2
#define USB_ENDPOINT_TYPE_INTERRUPT     3

/*
 * bmRequestType bit definitions
 */
#define USB_REQ_TYPE_DIRECTION         0x80
#define USB_REQ_TYPE_TYPE              0x60
#define USB_REQ_TYPE_RECIPIENT         0x1F

#define USB_REQ_TYPE_IN                0x80

#define USB_REQ_TYPE_STANDARD          0x00
#define USB_REQ_TYPE_CLASS             0x20
#define USB_REQ_TYPE_VENDOR            0x40

#define USB_REQ_TYPE_DEVICE            0x00
#define USB_REQ_TYPE_INTERFACE         0x01
#define USB_REQ_TYPE_ENDPOINT          0x02
#define USB_REQ_TYPE_OTHER             0x03

/*
 * Standard requests
 */
#define USB_REQ_GET_STATUS              0x00
#define USB_REQ_CLEAR_FEATURE           0x01
/* 0x02 is reserved */
#define USB_REQ_SET_FEATURE             0x03
/* 0x04 is reserved */
#define USB_REQ_SET_ADDRESS             0x05
#define USB_REQ_GET_DESCRIPTOR          0x06
#define USB_REQ_SET_DESCRIPTOR          0x07
#define USB_REQ_GET_CONFIGURATION       0x08
#define USB_REQ_SET_CONFIGURATION       0x09
#define USB_REQ_GET_INTERFACE           0x0A
#define USB_REQ_SET_INTERFACE           0x0B
#define USB_REQ_SYNCH_FRAME             0x0C




/*
 * PSP Usbbus driver name
 */
#define PSP_USB_BUS_DRIVERNAME "USBBusDriver"

/*
 * PSP Usb Status
 */
#define PSP_USB_STATUS_ACTIVATED              0x200
#define PSP_USB_STATUS_DEACTIVATED            0x100
#define PSP_USB_STATUS_CABLE_CONNECTED        0x020
#define PSP_USB_STATUS_CABLE_DISCONNECTED     0x010
#define PSP_USB_STATUS_CONNECTION_UNKNOWN     0x004
#define PSP_USB_STATUS_CONNECTION_ESTABLISHED 0x002
#define PSP_USB_STATUS_CONNECTION_NEW         0x001

/*
 * PSP USB Driver status
 */
#define PSP_USB_STATUS_DRIVER_STARTED         0x01
#define PSP_USB_STATUS_DRIVER_REGISTERED      0x02

/*
 * PSP limits
 */
#define PSP_USB_MAX_INTERFACES     8
#define PSP_USB_MAX_ENDPOINTS      9
#define PSP_USB_MAX_ALTERNATE      2


#define PSP_USB_RETCODE_CANCEL             -1
#define PSP_USB_RETCODE_CANCEL_ALL         -2
#define PSP_USB_RETCODE_CANCELTRANSMISSION -3
#define PSP_USB_RETCODE_SUCCESS       0
#define PSP_USB_RETCODE_SEND          1
#define PSP_USB_RETCODE_RECV          2

/*
 * Error codes
 */
#define PSP_USB_ERROR_ALREADY_DONE            0x80243001
#define PSP_USB_ERROR_INVALID_ARGUMENT        0x80243002
#define PSP_USB_ERROR_ARGUMENT_EXCEEDED_LIMIT 0x80243003
#define PSP_USB_ERROR_MEMORY_EXHAUSTED        0x80243004
#define PSP_USB_ERROR_DRIVER_NOT_FOUND        0x80243005
#define PSP_USB_ERROR_DRIVER_IN_PROGRESS      0x80243006
#define PSP_USB_ERROR_BUS_DRIVER_NOT_STARTED  0x80243007
#define PSP_USB_ERROR_WAIT_TIMEOUT            0x80243008
#define PSP_USB_ERROR_WAIT_CANCEL             0x80243009
#define PSP_USB_ERROR_INVALID_POINTER         0x80000103
#define PSP_USB_ERROR_INVALID_FLAG            0x80000105
#define PSP_USB_ERROR_INVALID_VALUE           0x800001FE
#define PSP_USB_ERROR_ILLEGAL_CONTEXT         0x80000030



/*
 *  USB string descriptor
 */
struct StringDescriptor
{
  unsigned char bLength;
  unsigned char bDescriptorType;
  short         bString[31];
};  /* Size 64 */

/*
 *  USB device descriptor
 */
struct DeviceDescriptor
{
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned short bcdUSB;
  unsigned char  bDeviceClass;
  unsigned char  bDeviceSubClass;
  unsigned char  bDeviceProtocol;
  unsigned char  bMaxPacketSize0;
  unsigned short idVendor;
  unsigned short idProduct;
  unsigned short bcdDevice;
  unsigned char  iManufacturer;
  unsigned char  iProduct;
  unsigned char  iSerialNumber;
  unsigned char  bNumConfigurations;
} __attribute__ ((aligned(4))); /* size 20 */

/*
 *  USB device qualifier descriptor
 */
struct DeviceQualifierDescriptor
{
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned short bcdUSB;
  unsigned char  bDeviceClass;
  unsigned char  bDeviceSubClass;
  unsigned char  bDeviceProtocol;
  unsigned char  bMaxPacketSize0;
  unsigned char  bNumConfigurations;
  unsigned char  bReserved;
} __attribute__ ((aligned(4))); /* size 12 */

/*
 *  USB configuration descriptor
 */
struct ConfigDescriptor
{
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned short wTotalLength;
  unsigned char  bNumInterfaces;
  unsigned char  bConfigurationValue;
  unsigned char  iConfiguration;
  unsigned char  bmAttributes;
  unsigned char  bMaxPower;

  struct InterfaceSettings *settings;

  unsigned char *extra;   /* Extra descriptors */
  int extraLength;
}; /* size 24 */

/*
 *  USB driver interfaces structure
 */
struct InterfaceSettings
{
  /* Pointers to the individual interface descriptors */
  struct InterfaceDescriptor *descriptors;

  unsigned int alternateSetting;

  /* Number of interface descriptors */
  unsigned int numDescriptors;
};

/*
 *  USB Interface descriptor
 */
struct InterfaceDescriptor
{
  unsigned char bLength;
  unsigned char bDescriptorType;
  unsigned char bInterfaceNumber;
  unsigned char bAlternateSetting;
  unsigned char bNumEndpoints;
  unsigned char bInterfaceClass;
  unsigned char bInterfaceSubClass;
  unsigned char bInterfaceProtocol;
  unsigned char iInterface;

  struct EndpointDescriptor *endpoints;

  unsigned char *extra;   /* Extra descriptors */
  int extraLength;
}; /* size 24 */

/*
 *  USB endpoint descriptor
 */
struct EndpointDescriptor
{
  unsigned char  bLength;
  unsigned char  bDescriptorType;
  unsigned char  bEndpointAddress;
  unsigned char  bmAttributes;
  unsigned short wMaxPacketSize;
  unsigned char  bInterval;

  unsigned char *extra;   /* Extra descriptors */
  int extraLength;
}; /* size 16 */


/*
 *  USB driver interface
 */
struct UsbInterface
{
  /* Expectant interface number (interface number or -1) */
  int expectNumber;
  /* End interface  */
  int interfaceNumber;
  /* Number of interfaces */
  int numInterfaces;
};

/*
 *  USB driver endpoint
 */
struct UsbEndpoint
{
  /* Driver Endpoint number (must be filled in sequentially) */
  int driverEndpointNumber;
  /* Endpoint number (Filled in by the bus driver) */
  int endpointNumber;
  /* Number of transmitted bytes */
  int transmittedBytes;
};


/*
 *  USB driver configuration
 */
struct UsbConfiguration
{
  /* Pointer to the configuration descriptors */
  struct ConfigDescriptor *configDescriptors;
  /* USB driver interface settings */
  struct InterfaceSettings *settings;
  /* Pointer to the first interface descriptor */
  struct InterfaceDescriptor *interfaceDescriptors;
  /* Pointer to the first endpoint descriptor */
  struct EndpointDescriptor *endpointDescriptors;
};

/*
 *  USB EP0 Device Request
 */
struct DeviceRequest
{
  unsigned char  bmRequestType;
  unsigned char  bRequest;
  unsigned short wValue;
  unsigned short wIndex;
  unsigned short wLength;
};

/*
 *  USB driver structure
 */
struct UsbDriver
{
  /* Name of the USB driver */
  const char *driverName;
  /* Number of endpoints in this driver (including default control) */
  int numEndpoints;
  /* List of endpoint structures (used when calling other functions) */
  struct UsbEndpoint *endpoints;
  /* Interface list */
  struct UsbInterface *interface;
  /* Pointer to hi-speed device descriptor */
  struct DeviceDescriptor *descriptor_hi;
  /* Pointer to hi-speed device configuration */
  struct UsbConfiguration *configuration_hi;
  /* Pointer to full-speed device descriptor */
  struct DeviceDescriptor *descriptor;
  /* Pointer to full-speed device configuration */
  struct UsbConfiguration *configuration;
  /* Default String descriptor */
  struct StringDescriptor *stringDescriptors;
  /* Received a control request  */
  int (*processRequest) (int arg1, int arg2, struct DeviceRequest *req);
  /* Change alternate setting */
  int (*chageSetting) (int interfaceNumber, int alternateSetting);
  /* Configuration set (attach) function */
  int (*attach) (int usb_version);
  /* Configuration unset (detach) function */
  void (*detach) (void);
  /* Configure the device */
  void (*configure) (int usb_version, int desc_count, struct InterfaceSettings *settings);
  /* Function called when the driver is started */
  int (*start) (int size, void *args);
  /* Function called when the driver is stopped */
  int (*stop) (int size, void *args);
  /* Link to next USB driver in the chain, set to NULL */
  struct UsbDriver *link;
};

/*
 *  USB device request
 */
struct UsbbdDeviceRequest
{
  /* Pointer to the endpoint to queue request on */
  struct UsbEndpoint *endpoint;
  /* Pointer to the data buffer to use in the request */
  void *data;
  /* Size of the data buffer */
  int  size;
  /* Is a control request? */
  int  isControlRequest;
  /* Pointer to the function to call on completion */
  void (*onComplete) (struct UsbbdDeviceRequest *req);
  /* Number of transmitted bytes  */
  int  transmitted;
  /* Return code of the request, 0 == success, -3 == cancelled */
  int  returnCode;
  /* Link pointer to next request used by the driver, set it to NULL */
  struct UsbbdDeviceRequest *next;
  /* An unused value (maybe an argument) */
  void *unused;
  /* Physical address */
  void *physicalAddress;
};

struct UsbDriverName {
  int  size;
  char name[32];
  int  flags;
} __attribute__ ((aligned(16))) /* size 48 */;

/**
  * Start a USB driver.
  *
  * @param driverName - name of the USB driver to start
  * @param size - Size of arguments to pass to USB driver start
  * @param args - Arguments to pass to USB driver start
  *
  * @return 0 on success
  */
int sceUsbStart (const char* driverName, int size, void *args);

/**
  * Stop a USB driver.
  *
  * @param driverName - name of the USB driver to stop
  * @param size - Size of arguments to pass to USB driver start
  * @param args - Arguments to pass to USB driver start
  *
  * @return 0 on success
  */
int sceUsbStop (const char* driverName, int size, void *args);

/**
  * Activate a USB driver.
  *
  * @param pid - Product ID for the default USB Driver
  *
  * @return 0 on success
  */
int sceUsbActivate (unsigned int productId);

/**
  * Deactivate USB driver.
  *
  * @return 0 on success
  */
int sceUsbDeactivate (void);

/**
  * Get USB state
  *
  * @return OR'd PSP_USB_STATUS_* constants
  */
int sceUsbGetState (void);

/**
  * Get state of a specific USB driver
  *
  * @param driverName - name of USB driver to get status from
  *
  * @return PSP_USB_DRIVER_STARTED  if the driver has been started, PSP_USB_DRIVER_REGISTERED if it is stopped
  */
int sceUsbGetDrvState (const char *driverName);

/**
 *  Get the list of drivers
 *  @param flags - combination of PSP_USB_DRIVER_STARTED or PSP_USB_DRIVER_REGISTERED
 *  @param list - points to the output list
 *  @param size - number of entries in the output list
 *  @return the number of drivers in the output or < 0 in case of error
 */
int sceUsbGetDrvList (unsigned int flags, struct UsbDriverName *list, int size);

/**
 * Wait for USB state
 * @param state - combination of states (returned by sceUsbGetState)
 * @param waitMode - PSP_EVENT_WAITAND, PSP_EVENT_WAITOR, ...
 * @param timeout - pointer to timeout
 * @return the usb state or < 0 in case of error
 */
int sceUsbWaitState (unsigned int state, unsigned int waitMode, SceUInt *timeout);

/*
 * Cancel a pending sceUsbWaitState
 * @return 0 on success
 */
int sceUsbWaitCancel (void);



/**
 * Register a USB driver.
 *
 * @param drv - Pointer to a filled out USB driver
 *
 * @return 0 on success, < 0 on error
 */
int sceUsbbdRegister (struct UsbDriver *drv);

/**
 * Unregister a USB driver
 *
 * @param drv - Pointer to a filled out USB driver
 *
 * @return 0 on success, < 0 on error
 */
int sceUsbbdUnregister (struct UsbDriver *drv);

/**
 * Clear the FIFO on an endpoint
 *
 * @param endp - The endpoint to clear
 *
 * @return 0 on success, < 0 on error
 */
int sceUsbbdClearFIFO (struct UsbEndpoint *endp);

/**
 * Cancel any pending requests on an endpoint.
 *
 * @param endp - The endpoint to cancel
 *
 * @return 0 on success, < 0 on error
 */
int sceUsbbdReqCancelAll (struct UsbEndpoint *endp);

/**
 * Stall an endpoint
 *
 * @param endp - The endpoint to stall
 *
 * @return 0 on success, < 0 on error
 */
int sceUsbbdStall (struct UsbEndpoint *endp);

/**
 * Queue a send request (IN from host pov)
 *
 * @param req - Pointer to a filled out UsbbdDeviceRequest structure.
 *
 * @return 0 on success, < 0 on error
 */
int sceUsbbdReqSend (struct UsbbdDeviceRequest *req);

/**
 * Queue a receive request (OUT from host pov)
 *
 * @param req - Pointer to a filled out UsbbdDeviceRequest structure
 *
 * @return 0 on success, < 0 on error
 */
int sceUsbbdReqRecv (struct UsbbdDeviceRequest *req);



#endif /* __USB_H */
