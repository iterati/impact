from PIL import Image, ImageDraw, ImageFont
import random

font = ImageFont.truetype("/Library/Fonts/Microsoft/Arial.ttf", 12)

colors = [
    (255,   0,   0),
    (252,  64,   0),
    (248, 128,   0),
    (244, 192,   0),
    (240, 240,   0),
    (192, 244,   0),
    (128, 248,   0),
    ( 64, 252,   0),
    (  0, 255,   0),
    (  0, 252,  64),
    (  0, 248, 128),
    (  0, 244, 192),
    (  0, 240, 240),
    (  0, 192, 244),
    (  0, 128, 248),
    (  0,  64, 252),
    (  0,   0, 255),
    ( 64,   0, 252),
    (128,   0, 248),
    (192,   0, 244),
    (240,   0, 240),
    (244,   0, 192),
    (248,   0, 128),
    (252,   0,  64),
    (  0,   0,   0),
    (255, 255, 255),
]


def make_img(i, r, g, b):
    frame = Image.new("RGB", (64, 80), (0, 0, 0))

    color = Image.new("RGB", (62, 62), (r >> 0, g >> 0, b >> 0))
    frame.paste(color, (1, 1))

    draw = ImageDraw.Draw(frame)
    draw.text((11, 66), "%2d 0x%02X" % (i, i), font=font)
    return frame


def make_strip(start, colors):
    strip = Image.new("RGB", (64 * len(colors), 80), (0, 0, 0))
    for i, c in enumerate(colors):
        img = make_img(start + i, *c)
        strip.paste(img, (64 * i, 0))

    return strip


def make_palette(colors):
    strip = Image.new("RGB", (48 * len(colors), 64), (0, 0, 0))
    for i, c in enumerate(colors):
        f = Image.new("RGB", (48, 64), (0, 0, 0))
        p = Image.new("RGB", (46, 46), unpack(c))
        f.paste(p, (1, 1))
        d = ImageDraw.Draw(f)
        d.text((3, 51), "%2d 0x%02X" % (i + 1, c), font=font)

        strip.paste(f, (48 * i, 0))

    return strip


def make_grid(colors):
    grid = Image.new("RGB", (64 * 8, 80 * 4), (0, 0, 0))
    for i in range(8):
        strip = make_strip(8 * i, colors[8 * i:8 * (i + 1)])
        grid.paste(strip, (0, 80 * i))

    return grid


make_grid(colors).save("palette.png")
