#!/usr/bin/env python3

# Creates a huebar image that displays the complete hue range

from PIL import Image
from sys import stdout

HEIGHT = 1
WIDTH = 10


img = Image.new("HSV", (WIDTH, HEIGHT))

def get_hue(x: int) -> int:
    MAX_HUE = 255
    res = int(x * MAX_HUE / WIDTH)
    return res


for i in range(WIDTH):
    for j in range(HEIGHT):
        img.putpixel((i, j), (get_hue(i), 255, 255))

# output image to stdout, so we can directly pass it to xxd -i
img.convert("RGBA").save(stdout, format="PNG")
