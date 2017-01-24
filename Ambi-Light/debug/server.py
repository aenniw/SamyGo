#!/usr/bin/env python

import Tkinter
import select
import socket
import struct
from PIL import ImageTk, Image

UDP_IP = "0.0.0.0"
UDP_PORT = 65000
WIDTH = 60
HEIGHT = 34
pixels = []


def get_color_(r, g, b):
    return '#%02x%02x%02x' % (r, g, b)


def get_color(c):
    return get_color_((c & 0xFF0000) >> 16, (c & 0x00FF00) >> 8, c & 0x0000FF)


def generate_pixels(C, w, h):
    w_size = (1920 / 2 - 2 * 1080 / 64) / w
    h_size = (1080 / 2 - 2 * 1080 / 64) / h
    # LOWER
    for i in range(w, 0, -1):
        pixels.append(
            C.create_rectangle(1080 / 64 + (i - 1) * w_size, (1080 / 2) - (1080 / 64), 1080 / 64 + i * w_size,
                               1080 / 2,
                               fill=get_color(0xFF00FF)))
    # LEFT
    for i in range(h, 0, -1):
        pixels.append(
            C.create_rectangle(0, 1080 / 64 + (i - 1) * h_size, 1080 / 64, 1080 / 64 + i * h_size,
                               fill=get_color(0xFF00FF)))
    # UPPER
    for i in range(0, w):
        pixels.append(C.create_rectangle(1080 / 64 + i * w_size, 0, 1080 / 64 + (i + 1) * w_size, 1080 / 64,
                                         fill=get_color(0xFF00FF)))
    # RIGHT
    for i in range(0, h):
        pixels.append(
            C.create_rectangle(1920 / 2 - 1080 / 64, 1080 / 64 + i * h_size, 1920 / 2, 1080 / 64 + (i + 1) * h_size,
                               fill=get_color(0xFF00FF)))


if __name__ == '__main__':

    top = Tkinter.Tk()
    top.title("Ambi debug")
    C = Tkinter.Canvas(top, height=1080 / 2, width=1920 / 2)
    C.pack()

    image = Image.open('color_test.jpg')
    image = image.resize((1920 / 2, 1080 / 2), Image.ANTIALIAS)
    img = ImageTk.PhotoImage(image)
    C.create_image(1920 / 4, 1080 / 4, image=img)

    generate_pixels(C, WIDTH, HEIGHT)
    print("UDP target IP:" + UDP_IP)
    print("UDP target port:" + UDP_PORT.__str__())
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setblocking(0)
    sock.bind((UDP_IP, UDP_PORT))
    while True:
        top.update()
        ready = select.select([sock], [], [], 0.01)
        if ready[0]:
            data, addr = sock.recvfrom(5)
            data = struct.unpack('!{}B'.format(6 * (WIDTH + HEIGHT)), data)
            print("received message:", data)
            for p in range(0, 2 * (WIDTH * HEIGHT)):
                C.itemconfig(p, fill=get_color_(data[p * 3], data[1 + p * 3], data[2 + p * 3]))
    exit(0)
