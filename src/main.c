#include <string.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include "usb.h"
#include "usb_descriptors.h"
#include "utils.h"
#include "format_conversion.h"

#define ENABLE_LOGGING 1

#if ENABLE_LOGGING == 1
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...) (void)0
#endif

PSP_MODULE_INFO("UVC", PSP_MODULE_KERNEL, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define USB_DRIVERNAME "UVC"
#define USB_PRODUCT_ID 0x1337

#define SCE_DMACPLUS_FORMAT_RGBA8888 0
#define SCE_DMACPLUS_FORMAT_RGB565   1
#define SCE_DMACPLUS_FORMAT_RGBA5551 2
#define SCE_DMACPLUS_FORMAT_RGBA4444 3

#define MAX_UVC_VIDEO_FRAME_SIZE	VIDEO_FRAME_SIZE_YUY2(480, 272)

#define UVC_PAYLOAD_HEADER_SIZE		12
#define UVC_PAYLOAD_SIZE(frame_size)	(UVC_PAYLOAD_HEADER_SIZE + (frame_size))
#define MAX_UVC_PAYLOAD_TRANSFER_SIZE	UVC_PAYLOAD_SIZE(MAX_UVC_VIDEO_FRAME_SIZE)

#define EVENT_STOP_STREAM	(1u << 0)
#define EVENT_FRAME_SENT	(1u << 1)

struct uvc_frame {
	unsigned char header[UVC_PAYLOAD_HEADER_SIZE];
	unsigned char data[];
} __attribute__((packed));

static const struct uvc_streaming_control uvc_probe_control_setting_default = {
	.bmHint				= 0,
	.bFormatIndex			= FORMAT_INDEX_UNCOMPRESSED_YUY2,
	.bFrameIndex			= 1,
	.dwFrameInterval		= FPS_TO_INTERVAL(60),
	.wKeyFrameRate			= 0,
	.wPFrameRate			= 0,
	.wCompQuality			= 0,
	.wCompWindowSize		= 0,
	.wDelay				= 0,
	.dwMaxVideoFrameSize		= MAX_UVC_VIDEO_FRAME_SIZE,
	.dwMaxPayloadTransferSize	= MAX_UVC_PAYLOAD_TRANSFER_SIZE,
	.dwClockFrequency		= 0,
	.bmFramingInfo			= 0,
	.bPreferedVersion		= 1,
	.bMinVersion			= 0,
	.bMaxVersion			= 0,
};

static struct uvc_streaming_control uvc_probe_control_setting;

static struct {
	unsigned char buffer[64];
	struct DeviceRequest ep0_req;
} __attribute__((aligned(64))) pending_recv;

//static SceUID uvc_thread_id;
//static SceUID uvc_event_flag_id;
//static int uvc_thread_run;
static int stream;
static SceUID uvc_frame_req_evflag;
static unsigned char tx_buf[MAX_UVC_PAYLOAD_TRANSFER_SIZE] __attribute__((aligned(64)));

static int usb_ep0_req_send(const void *data, unsigned int size)
{
	static struct UsbbdDeviceRequest req;

	req = (struct UsbbdDeviceRequest){
		.endpoint = &endpoints[0],
		.data = (void *)data,
		.size = size,
		.isControlRequest = 0,
		.onComplete = NULL,
		.transmitted = 0,
		.returnCode = 0,
		.next = NULL,
		.unused = NULL,
		.physicalAddress = NULL
	};

	sceKernelDcacheWritebackRange(data, size);

	return sceUsbbdReqSend(&req);
}

static void usb_ep0_req_recv_on_complete(struct UsbbdDeviceRequest *req);

static int usb_ep0_enqueue_recv_for_req(const struct DeviceRequest *ep0_req)
{
	static struct UsbbdDeviceRequest req;

	pending_recv.ep0_req = *ep0_req;

	req = (struct UsbbdDeviceRequest){
		.endpoint = &endpoints[0],
		.data = (void *)pending_recv.buffer,
		.size = pending_recv.ep0_req.wLength,
		.isControlRequest = 0,
		.onComplete = &usb_ep0_req_recv_on_complete,
		.transmitted = 0,
		.returnCode = 0,
		.next = NULL,
		.unused = NULL,
		.physicalAddress = NULL
	};

	sceKernelDcacheInvalidateRange(pending_recv.buffer, pending_recv.ep0_req.wLength);

	return sceUsbbdReqRecv(&req);
}

static int uvc_frame_req_init(void)
{
	uvc_frame_req_evflag = sceKernelCreateEventFlag("uvc_frame_req_evflag", 0, 0, NULL);
	if (uvc_frame_req_evflag < 0) {
		return uvc_frame_req_evflag;
	}

	return 0;
}

static int uvc_frame_req_fini(void)
{
	return sceKernelDeleteEventFlag(uvc_frame_req_evflag);
}

static void uvc_handle_video_streaming_req_recv(const struct DeviceRequest *req)
{
	struct uvc_streaming_control *streaming_control =
		(struct uvc_streaming_control *)pending_recv.buffer;

	switch (req->wValue >> 8) {
	case UVC_VS_PROBE_CONTROL:
		switch (req->bRequest) {
		case UVC_SET_CUR:
			uvc_probe_control_setting.bFormatIndex = streaming_control->bFormatIndex;
			uvc_probe_control_setting.bFrameIndex = streaming_control->bFrameIndex;
			uvc_probe_control_setting.dwFrameInterval = streaming_control->dwFrameInterval;
			LOG("Probe SET_CUR, bFormatIndex: %d, bmFramingInfo: %x\n",
			    uvc_probe_control_setting.bFormatIndex,
			    uvc_probe_control_setting.bmFramingInfo);
			break;
		}
		break;
	case UVC_VS_COMMIT_CONTROL:
		switch (req->bRequest) {
		case UVC_SET_CUR:
			uvc_probe_control_setting.bFormatIndex = streaming_control->bFormatIndex;
			uvc_probe_control_setting.bFrameIndex = streaming_control->bFrameIndex;
			uvc_probe_control_setting.dwFrameInterval = streaming_control->dwFrameInterval;
			LOG("Commit SET_CUR, bFormatIndex: %d, bmFramingInfo: %x\n",
			    uvc_probe_control_setting.bFormatIndex,
			    uvc_probe_control_setting.bmFramingInfo);

			LOG("Start streaming!\n");
			stream = 1;
			//sceKernelSetEventFlag(uvc_event_flag_id, 1);
			break;
		}
		break;
	}
}

void usb_ep0_req_recv_on_complete(struct UsbbdDeviceRequest *req)
{
	switch (pending_recv.ep0_req.wIndex & 0xFF) {
	case STREAM_INTERFACE:
		uvc_handle_video_streaming_req_recv(&pending_recv.ep0_req);
		break;
	}
}

static void uvc_handle_interface_ctrl_req(const struct DeviceRequest *req)
{
	LOG("  uvc_handle_interface_ctrl_req\n");
}

static void uvc_handle_input_terminal_req(const struct DeviceRequest *req)
{
	LOG("  uvc_handle_input_terminal_req %x, %x\n", req->wValue, req->bRequest);
}

static void uvc_handle_output_terminal_req(const struct DeviceRequest *req)
{
	LOG("  uvc_handle_output_terminal_req\n");
}

static void uvc_handle_video_streaming_req(const struct DeviceRequest *req)
{
	LOG("  uvc_handle_video_streaming_req %x, %x\n", req->wValue, req->bRequest);

	switch (req->wValue >> 8) {
	case UVC_VS_PROBE_CONTROL:
		switch (req->bRequest) {
		case UVC_GET_INFO:
			break;
		case UVC_GET_LEN:
			break;
		case UVC_GET_MIN:
		case UVC_GET_MAX:
		case UVC_GET_DEF:
			LOG("Probe GET_DEF, bFormatIndex: %d, bmFramingInfo: %x\n",
			    uvc_probe_control_setting_default.bFormatIndex,
			    uvc_probe_control_setting_default.bmFramingInfo);
			usb_ep0_req_send(&uvc_probe_control_setting_default,
					 sizeof(uvc_probe_control_setting_default));
			break;
		case UVC_GET_CUR:
			LOG("Probe GET_CUR, bFormatIndex: %d, bmFramingInfo: %x\n",
			    uvc_probe_control_setting.bFormatIndex,
			    uvc_probe_control_setting.bmFramingInfo);
			usb_ep0_req_send(&uvc_probe_control_setting,
					 sizeof(uvc_probe_control_setting));
			break;
		case UVC_SET_CUR:
			usb_ep0_enqueue_recv_for_req(req);
			break;
		}
		break;
	case UVC_VS_COMMIT_CONTROL:
		switch (req->bRequest) {
		case UVC_GET_INFO:
			break;
		case UVC_GET_LEN:
			break;
		case UVC_GET_CUR:
			usb_ep0_req_send(&uvc_probe_control_setting,
					 sizeof(uvc_probe_control_setting));
			break;
		case UVC_SET_CUR:
			usb_ep0_enqueue_recv_for_req(req);
			break;
		}
		break;
	}
}

static void uvc_handle_video_abort(void)
{
	LOG("uvc_handle_video_abort\n");

	if (stream) {
		stream = 0;

		sceUsbbdClearFIFO(&endpoints[1]);
		sceUsbbdReqCancelAll(&endpoints[1]);
	}
}

static void usb_handle_set_interface(const struct DeviceRequest *req)
{
	LOG("usb_handle_set_interface %x %x\n", req->wIndex, req->wValue);

	/* MAC OS sends Set Interface Alternate Setting 0 command after
	 * stopping to stream. This application needs to stop streaming. */
	if ((req->wIndex == STREAM_INTERFACE) && (req->wValue == 0))
		uvc_handle_video_abort();
}

static void usb_handle_clear_feature(const struct DeviceRequest *req)
{
	LOG("usb_handle_clear_feature\n");

	/* Windows OS sends Clear Feature Request after it stops streaming,
	 * however MAC OS sends clear feature request right after it sends a
	 * Commit -> SET_CUR request. Hence, stop streaming only of streaming
	 * has started. */
	switch (req->wValue) {
	case USB_FEATURE_ENDPOINT_HALT:
		if ((req->wIndex & USB_ENDPOINT_ADDRESS_MASK) ==
		    endpoints[1].endpointNumber) {
			uvc_handle_video_abort();
		}
		break;
	}
}

static int usb_process_request(int recipient, int arg, struct DeviceRequest *req)
{
	LOG("usb_driver_process_request(recipient: %x, arg: %x)\n", recipient, arg);
	LOG("  request: %x type: %x wValue: %x wIndex: %x wLength: %x\n",
		req->bRequest, req->bmRequestType, req->wValue, req->wIndex, req->wLength);

	if (arg < 0)
		return -1;

	switch (req->bmRequestType) {
	case USB_CTRLTYPE_DIR_DEVICE2HOST |
	     USB_CTRLTYPE_TYPE_CLASS |
	     USB_CTRLTYPE_REC_INTERFACE: /* 0xA1 */
	case USB_CTRLTYPE_DIR_HOST2DEVICE |
	     USB_CTRLTYPE_TYPE_CLASS |
	     USB_CTRLTYPE_REC_INTERFACE: /* 0x21 */
		switch (req->wIndex & 0xFF) {
		case CONTROL_INTERFACE:
			switch (req->wIndex >> 8) {
			case INTERFACE_CTRL_ID:
				uvc_handle_interface_ctrl_req(req);
				break;
			case INPUT_TERMINAL_ID:
				uvc_handle_input_terminal_req(req);
				break;
			case OUTPUT_TERMINAL_ID:
				uvc_handle_output_terminal_req(req);
				break;
			}
			break;
		case STREAM_INTERFACE:
			uvc_handle_video_streaming_req(req);
			break;
		}
		break;
	case USB_CTRLTYPE_DIR_HOST2DEVICE |
	     USB_CTRLTYPE_TYPE_STANDARD |
	     USB_CTRLTYPE_REC_INTERFACE: /* 0x01 */
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			usb_handle_set_interface(req);
			break;
		}
		break;
	case USB_CTRLTYPE_DIR_HOST2DEVICE |
	     USB_CTRLTYPE_TYPE_STANDARD |
	     USB_CTRLTYPE_REC_ENDPOINT: /* 0x02 */
		switch (req->bRequest) {
		case USB_REQ_CLEAR_FEATURE:
			usb_handle_clear_feature(req);
			break;
		}
		break;
	case USB_CTRLTYPE_DIR_DEVICE2HOST |
	     USB_CTRLTYPE_TYPE_STANDARD |
	     USB_CTRLTYPE_REC_DEVICE: /* 0x80 */
		switch (req->wValue >> 8) {
		case 0x0A: /* USB_DT_DEBUG */
			break;
		}
		break;
	default:
		LOG("Unknown bmRequestType: 0x%02X\n", req->bmRequestType);
	}

	return 0;
}

static int usb_change_setting(int interfaceNumber, int alternateSetting)
{
	LOG("usb_change %d %d\n", interfaceNumber, alternateSetting);
	return 0;
}

static int usb_attach(int usb_version)
{
	LOG("usb_attach %d\n", usb_version);
	return 0;
}

static void usb_detach(void)
{
	LOG("usb_detach\n");
	uvc_handle_video_abort();
}

static void usb_configure(int usb_version, int desc_count, struct InterfaceSettings *settings)
{
	LOG("usb_configure %d %d %p %d\n", usb_version, desc_count, settings, settings->numDescriptors);
}

static int usb_driver_start(int size, void *args)
{
	LOG("usb_driver_start\n");
	return 0;
}

static int usb_driver_stop(int size, void *args)
{
	LOG("usb_driver_stop\n");
	uvc_handle_video_abort();
	return 0;
}

static struct UsbDriver usb_driver = {
	.driverName			= USB_DRIVERNAME,
	.numEndpoints			= 2,
	.endpoints			= endpoints,
	.interface			= &interface,
	.descriptor_hi			= &devdesc_hi,
	.configuration_hi		= &config_hi,
	.descriptor			= &devdesc_full,
	.configuration			= &config_full,
	.stringDescriptors		= NULL,
	.processRequest			= &usb_process_request,
	.chageSetting			= &usb_change_setting,
	.attach				= &usb_attach,
	.detach				= &usb_detach,
	.configure			= &usb_configure,
	.start				= &usb_driver_start,
	.stop				= &usb_driver_stop,
	.link				= NULL
};

static void uvc_frame_send_req_on_complete(struct UsbbdDeviceRequest *req)
{
	sceKernelSetEventFlag(uvc_frame_req_evflag, EVENT_FRAME_SENT);
}

int convert_and_send_frame_yuy2(int fid, void *fbaddr, int fbstride, int fbpixelformat)
{
	static struct UsbbdDeviceRequest req;
	static const int eof = 1;
	unsigned int event;
	unsigned int t0, t1, t2, t3;
	int ret;

	req = (struct UsbbdDeviceRequest){
		.endpoint = &endpoints[1],
		.data = tx_buf,
		.size = sizeof(tx_buf),
		.isControlRequest = 0,
		.onComplete = uvc_frame_send_req_on_complete,
		.transmitted = 0,
		.returnCode = 0,
		.next = NULL,
		.unused = NULL,
		.physicalAddress = NULL
	};

	tx_buf[0] = UVC_PAYLOAD_HEADER_SIZE;
	tx_buf[1] = UVC_STREAM_EOH;
	if (fid)
		tx_buf[1] |= UVC_STREAM_FID;
	if (eof)
		tx_buf[1] |= UVC_STREAM_EOF;

	t0 = sceKernelGetSystemTimeLow();

	if (fbpixelformat == PSP_DISPLAY_PIXEL_FORMAT_8888)
		r8g8b8a8_to_yuy2(fbaddr, &tx_buf[UVC_PAYLOAD_HEADER_SIZE], fbstride, 480, 272);
	else if (fbpixelformat == PSP_DISPLAY_PIXEL_FORMAT_565)
		r5g6b5_to_yuy2(fbaddr, &tx_buf[UVC_PAYLOAD_HEADER_SIZE], fbstride, 480, 272);
	else if (fbpixelformat == PSP_DISPLAY_PIXEL_FORMAT_5551)
		r5g5b5a1_to_yuy2(fbaddr, &tx_buf[UVC_PAYLOAD_HEADER_SIZE], fbstride, 480, 272);
	else if (fbpixelformat == PSP_DISPLAY_PIXEL_FORMAT_4444)
		r4g4b4a4_to_yuy2(fbaddr, &tx_buf[UVC_PAYLOAD_HEADER_SIZE], fbstride, 480, 272);

	sceKernelDcacheWritebackRange(tx_buf, sizeof(tx_buf));

	t1 = sceKernelGetSystemTimeLow();

	LOG("Sending frame...\n");

	t2 = sceKernelGetSystemTimeLow();

	ret = sceUsbbdReqSend(&req);
	if (ret < 0)
		return ret;

	ret = sceKernelWaitEventFlagCB(uvc_frame_req_evflag, EVENT_STOP_STREAM | EVENT_FRAME_SENT,
	                               PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, &event, NULL);

	t3 = sceKernelGetSystemTimeLow();

	if (event & EVENT_STOP_STREAM)
		LOG("Received stream stop!\n");
	else if (event & EVENT_FRAME_SENT)
		LOG("Frame sent!\n");

	LOG("CSC: %dus, USB send: %dus\n", t1 - t0, t3 - t2);

	return ret;
}

static void get_display_params_lcdc(void **addr, int *pixelformat, int *width, int *stride)
{
	int ldcd_pixelfmt;

	*addr = (void *)*(volatile unsigned int *)0xBC800100;
	ldcd_pixelfmt = *(volatile unsigned int *)0xBC800104;
	*width = *(volatile unsigned int *)0xBC800108;
	*stride = *(volatile unsigned int *)0xBC80010C;

	if (ldcd_pixelfmt == SCE_DMACPLUS_FORMAT_RGBA8888)
		*pixelformat = PSP_DISPLAY_PIXEL_FORMAT_8888;
	else if (ldcd_pixelfmt == SCE_DMACPLUS_FORMAT_RGB565)
		*pixelformat = PSP_DISPLAY_PIXEL_FORMAT_565;
	else if (ldcd_pixelfmt == SCE_DMACPLUS_FORMAT_RGBA5551)
		*pixelformat = PSP_DISPLAY_PIXEL_FORMAT_5551;
	else if (ldcd_pixelfmt == SCE_DMACPLUS_FORMAT_RGBA4444)
		*pixelformat = PSP_DISPLAY_PIXEL_FORMAT_4444;
}

static int send_frame(void)
{
	static int fid = 0;

	int ret;
	void *fbaddr;
	int fbwidth;
	int fbstride;
	int fbpixelformat = 0;

	//fbwidth = 480;
	//ret = sceDisplayGetFrameBuf(&fbaddr, &fbstride, &fbpixelformat, PSP_DISPLAY_SETBUF_IMMEDIATE);
	//if (ret < 0)
	//	return ret;

	get_display_params_lcdc(&fbaddr, &fbpixelformat, &fbwidth, &fbstride);

	LOG("FB addr: %p, w: %d, stride: %d, pxlfmt: %d\n", fbaddr, fbwidth, fbstride, fbpixelformat);

	switch (uvc_probe_control_setting.bFormatIndex) {
	case FORMAT_INDEX_UNCOMPRESSED_YUY2: {
		//const struct UVC_FRAME_UNCOMPRESSED(2) *frames =
		//	video_streaming_descriptors.frames_uncompressed_yuy2;
		//int dst_width = frames[uvc_probe_control_setting.bFrameIndex - 1].wWidth;
		//int dst_height = frames[uvc_probe_control_setting.bFrameIndex - 1].wHeight;

		ret = convert_and_send_frame_yuy2(fid, fbaddr, fbstride, fbpixelformat);
		if (ret < 0) {
			LOG("Error sending YUY2 frame: 0x%08X\n", ret);
			return ret;
		}

		break;
	}
	}

	if (ret < 0) {
		stream = 0;
		return ret;
	}

	fid ^= 1;

	return 0;
}

int main(int argc, char *argv[])
{
#if ENABLE_LOGGING == 1
	pspDebugScreenInit();
#endif
	SetupCallbacks();

	LOG("UVC. USB Video Class\n");

	LOG("Registering USB driver...");
	int ret = sceUsbbdRegister(&usb_driver);
	LOG("returned %i\n", ret);

	LOG("Starting PSP_USBBUS...");
	ret = sceUsbStart(PSP_USB_BUS_DRIVERNAME, 0, 0);
	LOG("returned %i\n", ret);

	LOG("Starting "USB_DRIVERNAME"...");
	ret = sceUsbStart(USB_DRIVERNAME, 0, 0);
	LOG("returned 0x%08X\n", ret);

	ret = uvc_frame_req_init();
	if (ret < 0) {
		LOG("Error allocating USB request (0x%08X)\n", ret);
		return ret;
	}

	/*
	 * Set the current streaming settings to the default ones.
	 */
	memcpy(&uvc_probe_control_setting, &uvc_probe_control_setting_default,
	       sizeof(uvc_probe_control_setting));

	stream = 0;

	LOG("Activating 0x%04X...", USB_PRODUCT_ID);
	ret = sceUsbActivate(USB_PRODUCT_ID);
	LOG("returned 0x%08X\n", ret);

	while (run) {
		SceCtrlData pad;
		sceCtrlPeekBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_START)
			run = 0;

		sceDisplayWaitVblankStart();

		if (sceUsbGetState () & PSP_USB_STATUS_CONNECTION_ESTABLISHED) {
			if (stream)
				send_frame();
		}
	}

	LOG("UVC exiting!!\n");

	//uvc_handle_video_abort();
	sceKernelSetEventFlag(uvc_frame_req_evflag, EVENT_STOP_STREAM);
	uvc_frame_req_fini();

	LOG("Deactivating...\n");
	sceUsbDeactivate(); //USB_PRODUCT_ID??
	LOG("Stopping " USB_DRIVERNAME "...\n");
	sceUsbStop(USB_DRIVERNAME, 0, 0 );
	LOG("Stopping " PSP_USB_BUS_DRIVERNAME "...\n");
	sceUsbStop(PSP_USB_BUS_DRIVERNAME, 0, 0);
	LOG("Unregistering USB driver...\n");
	sceUsbbdUnregister(&usb_driver);

	LOG("Exiting...\n");
	return 0;
}
