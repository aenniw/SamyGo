# Pull base image.
FROM ubuntu:16.04

# Configure.
RUN apt-get update
RUN apt-get install -y -qq gcc-arm-linux-gnueabi make cmake zip git
RUN apt-get install -y -qq libsdl1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev
RUN rm -rf /var/lib/apt/lists/*

# Define default command.
CMD ["bash"]