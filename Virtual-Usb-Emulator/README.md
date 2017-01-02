# Virtual Usb mounter

CI application that writes entries to `/dtv/usb/log` and emulates adding and removing of USB drives into TV.
It may requires that at least one USB drive is plunged into TV or virtual USB is emulated in kernel so that this hack properly works.
Se more in [Forum discussion](https://forum.samygo.tv/viewtopic.php?t=113)

## Usage
```
mounter -d                                  # Start mounter daemon
mounter -mount sdc1 "Virtual Drive"         # Create virtual drive pointing to /dtv/usb/sdc1/
mounter -unmount sdc1                       # Remove vistual drive pointing to /dtv/usb/sdc1
```