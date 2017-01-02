# Ambi-Light

Ambi-Light port for Samsung LE46B650 using LAN/WiFi for data transition.
Led lights are driven by Wemos D1 mini powered by ESP8266 flashed with [Firmware](https://github.com/aenniw/wemos_d1)

Application contains python UI for led setup and application debugging. Refresh rate of whole screen scan is heavily delayed,
by TV screen memory access and may cause lags when starting or under heavy load. It scans every 3th pixel to sample colors
and sends them by UDP socket to LED driver.

Based on topic from [Forum](http://forum.samygo.tv/viewtopic.php?f=5&t=963&sid=c4e216e79b0a9286ec017d83d837c794)

## Usage

Start application by injecting it at startup or by loading it manually from content library.
 
| Button | Function         |
|:------:|:----------------:|
| Red    | Start sampling   |
| Green  | Stop sampling    |