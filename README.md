# SamyGo

[![N|Solid](https://travis-ci.org/aenniw/SamyGo.svg?branch=master)](https://travis-ci.org/aenniw/SamyGo)

Content Library applications for [Samsung LE46B650](http://www.samsung.com/cz/consumer/tv-av/tv/hd/LE46B650T2WXXH)

[**Libraries and resources**](http://download.samygo.tv/) /
[**Wiki**](https://wiki.samygo.tv/index.php5?title=Main_Page) /
[**Forum**](https://forum.samygo.tv/viewforum.php?f=11&sid=04ff6f8cfb73bf6dd4755dd96e95cf73)

## Setup environment

### Local env setup

```bash
sudo apt-get update
sudo apt-get install gcc-arm-linux-gnueabi cmake make zip git
sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev
```

### Docker env setup

```bash
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu $(lsb_release -cs) stable"
sudo apt-get update
sudo apt-cache policy docker-ce
sudo apt-get install -y docker-ce
sudo usermod -aG docker ${USER}
```

## Build

### Local env build

```bash
./make all --debug
```

### Docker env build

```bash
./docker make all --debug
```

## SubProjects

[Ambi-Light](Ambi-Light/README.md) |
[Virtual-Usb-Emulator](Virtual-Usb-Emulator/README.md) |
[Template](Template/README.md)
