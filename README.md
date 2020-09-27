# PSP USB Video Class (UVC) plugin

## What's this?

This is a plugin that lets you stream your PSP screen to your computer via USB.

It follows the USB Video Class (UVC) standard, therefore it should work on any OS.

## Supported formats and resolutions

* 480x272 YUY2 @ 30 FPS and 60 FPS (WIP)

## Download and installation

**Download**:

* [uvc.prx](https://github.com/xerpi/psp-uvc-usb-video-class/releases)

**Installation**:

1. Copy `uvc.prx` to `ms0:/seplugins/`
2. Add `uvc.prx` to `ms0:/seplugins/VSH.TXT` and/or `ms0:/seplugins/GAME.TXT`
3. Reboot your PSP

**Viewing**

On Windows I recommend PotPlayer for viewing, and on Linux I recommend *mpv* or *mplayer*.
* `mpv av://v4l2:/dev/videoN --profile=low-latency --untimed`
* `mplayer tv:// -tv driver=v4l2:device=/dev/videoN`

You can use OBS to capture/live-stream the incoming video from the PSP.

**Compilation**

* If you want to compile the source code, [pspsdk](https://github.com/pspdev/pspsdk/) is needed.

## Troubleshooting

* If you use Windows 10 you might have to change the Camera access permissions on the Privacy Settings.
* Remember that if anything goes wrong (like the PSP not booting) you can always press R at boot to enter recovery mode.

Note: No, it *doesn't* stream audio. For that use a 3.5mm jack to jack adapter.
