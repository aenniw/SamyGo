#!/usr/bin/env python

import socket
import struct
import time
import select
import Tkinter
from PIL import ImageTk, Image

UDP_IP = "0.0.0.0"
UDP_PORT = 65000
WIDTH = 10
HEIGHT = 8
pixels = []


def get_color_(r, g, b):
    return '#%02x%02x%02x' % (r, g, b)


def get_color(c):
    return get_color_((c & 0xFF0000) >> 16, (c & 0x00FF00) >> 8, c & 0x0000FF)


def generate_pixels(C, w, h):
    w_size = (1920 / 2 - 2 * 1080 / 64) / w
    h_size = (1080 / 2 - 2 * 1080 / 64) / h
    pizel_count = 1
    # UPPER
    for i in range(0, w):
        pixels.append(C.create_rectangle(1080 / 64 + i * w_size, 0, 1080 / 64 + (i + 1) * w_size, 1080 / 64,
                                         fill=get_color(0xFF00FF)))
        C.create_text(1080 / 64 + i * w_size + 50, 10, text=pizel_count.__str__(), fill="white")
        pizel_count += 1
    # RIGHT
    for i in range(0, h):
        pixels.append(
            C.create_rectangle(1920 / 2 - 1080 / 64, 1080 / 64 + i * h_size, 1920 / 2, 1080 / 64 + (i + 1) * h_size,
                               fill=get_color(0xFF00FF)))
        C.create_text(1920 / 2 - 8, 1080 / 64 + i * h_size + 30, text=pizel_count.__str__(), fill="white")
        pizel_count += 1
    # LOWER
    for i in range(w - 1, -1, -1):
        pixels.append(
            C.create_rectangle(1080 / 64 + i * w_size, (1080 / 2) - (1080 / 64), 1080 / 64 + (i + 1) * w_size, 1080 / 2,
                               fill=get_color(0xFF00FF)))
        C.create_text(1080 / 64 + i * w_size + 50, 1080 / 2 - 8, text=pizel_count.__str__(), fill="white")
        pizel_count += 1
    # LEFT
    for i in range(h - 1, -1, -1):
        pixels.append(
            C.create_rectangle(0, 1080 / 64 + i * h_size, 1080 / 64, 1080 / 64 + (i + 1) * h_size,
                               fill=get_color(0xFF00FF)))
        C.create_text(8, 1080 / 64 + i * h_size + 30, text=pizel_count.__str__(), fill="white")
        pizel_count += 1


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
            data = struct.unpack('!5B', data)
            print "received message:", data
            C.itemconfig(pixels[(data[0] << 8) | data[1] - 1], fill=get_color_(data[2], data[3], data[4]))
    exit(0)
