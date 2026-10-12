"""Draws the modern in-game interface's button icons into ui/.

Each icon is a glossy tile in a side's colour with a recessed glyph, after the look of Yuri's
Revenge's sidebar buttons. Each sheet holds three square frames: normal, pressed and disabled.
Run it from anywhere: python tools/hudicons/make_hud_icons.py
"""
import math
import os
from PIL import Image, ImageDraw, ImageFilter, ImageChops

SS = 8  # supersampling factor

SIDES = {
    'allied': {'base': (40, 110, 235), 'light': (150, 215, 255), 'dark': (10, 30, 120), 'glyph': (8, 22, 90)},
    'soviet': {'base': (205, 40, 30), 'light': (255, 170, 140), 'dark': (95, 10, 8), 'glyph': (70, 6, 6)},
    'yuri':   {'base': (150, 55, 215), 'light': (225, 170, 255), 'dark': (60, 15, 100), 'glyph': (45, 8, 75)},
}

def lerp(a, b, t):
    return tuple(int(a[i] + (b[i] - a[i]) * t) for i in range(3))

def rounded_mask(w, h, r):
    m = Image.new('L', (w, h), 0)
    ImageDraw.Draw(m).rounded_rectangle((0, 0, w - 1, h - 1), r, fill=255)
    return m

def tile(w, h, side, radius=4):
    """A glossy bevelled tile in the side colour, w x h final pixels, as an RGBA image at SS scale."""
    c = SIDES[side]
    W, H, R = w * SS, h * SS, radius * SS
    # Steel rim.
    rim = Image.new('RGBA', (W, H))
    d = ImageDraw.Draw(rim)
    for y in range(H):
        t = y / (H - 1)
        d.line((0, y, W, y), fill=lerp((225, 230, 240), (70, 75, 90), t) + (255,))
    rim.putalpha(rounded_mask(W, H, R))
    # Face with a vertical gradient: light at the top, the side colour, dark at the base.
    inset = SS * 1
    fw, fh = W - 2 * inset, H - 2 * inset
    face = Image.new('RGBA', (fw, fh))
    d = ImageDraw.Draw(face)
    for y in range(fh):
        t = y / (fh - 1)
        col = lerp(c['light'], c['base'], t / 0.45) if t < 0.45 else lerp(c['base'], c['dark'], (t - 0.45) / 0.55)
        d.line((0, y, fw, y), fill=col + (255,))
    face.putalpha(rounded_mask(fw, fh, max(R - inset, SS)))
    # Gloss: a soft white band over the top half.
    gloss = Image.new('L', (fw, fh), 0)
    ImageDraw.Draw(gloss).rounded_rectangle((SS, SS, fw - SS, fh * 0.48), max(R - inset - SS, SS), fill=70)
    gloss = gloss.filter(ImageFilter.GaussianBlur(SS * 0.6))
    white = Image.new('RGBA', (fw, fh), (255, 255, 255, 0))
    white.putalpha(ImageChops.multiply(gloss, face.getchannel('A')))
    face = Image.alpha_composite(face, white)
    rim.alpha_composite(face, (inset, inset))
    return rim

def recess(img, glyph_mask, side):
    """Sets a glyph into the tile: dark fill, a light lip below-right, a dark shadow above-left."""
    c = SIDES[side]
    W, H = img.size
    off = max(1, SS // 2)
    lip = ImageChops.offset(glyph_mask, off, off)
    shadow = ImageChops.offset(glyph_mask, -off, -off)
    for mask, col, alpha in ((lip, c['light'], 200), (shadow, (0, 0, 0), 120), (glyph_mask, c['glyph'], 255)):
        layer = Image.new('RGBA', (W, H), col + (0,))
        layer.putalpha(mask.point(lambda v: v * alpha // 255))
        img.alpha_composite(layer)
    return img

def glyph(kind, W, H):
    m = Image.new('L', (W, H), 0)
    d = ImageDraw.Draw(m)
    s = min(W, H)
    cx, cy = W / 2, H / 2
    u = s / 28.0  # glyphs are laid out on a 28-unit square
    def P(x, y):
        return (cx + (x - 14) * u, cy + (y - 14) * u)
    if kind == 'build':  # a factory: roof and walls with a door
        d.polygon([P(5, 13), P(14, 6), P(23, 13)], fill=255)
        d.rectangle((*P(7, 13), *P(21, 22)), fill=255)
        d.rectangle((*P(12, 16), *P(16, 22)), fill=0)
    elif kind == 'defense':  # a shield
        pts = [P(14, 5), P(22, 8), P(21, 16), P(14, 23), P(7, 16), P(6, 8)]
        d.polygon(pts, fill=255)
    elif kind == 'infantry':  # a soldier
        d.ellipse((*P(11.5, 4.5), *P(16.5, 9.5)), fill=255)
        d.rounded_rectangle((*P(9.5, 10.5), *P(18.5, 17)), 2 * u, fill=255)
        d.rectangle((*P(10.5, 16), *P(13.3, 23.5)), fill=255)
        d.rectangle((*P(14.7, 16), *P(17.5, 23.5)), fill=255)
    elif kind == 'vehicle':  # a tank in profile
        d.rounded_rectangle((*P(4, 15), *P(24, 21)), 3 * u, fill=255)
        d.rounded_rectangle((*P(8, 11), *P(18, 15.5)), 1.5 * u, fill=255)
        d.rectangle((*P(17, 12.2), *P(25, 13.8)), fill=255)
        for x in (8, 12, 16, 20):
            d.ellipse((*P(x - 1.3, 16.7), *P(x + 1.3, 19.3)), fill=0)
    elif kind == 'repair':  # a wrench
        d.line((P(8, 20), P(18, 10)), fill=255, width=int(3.2 * u))
        d.ellipse((*P(15, 5), *P(23, 13)), fill=255)
        d.ellipse((*P(18.5, 6.5), *P(22.5, 10.5)), fill=0)
        d.polygon([P(19.5, 4.5), P(24, 9), P(21, 9), P(19.5, 7.5)], fill=0)
        d.ellipse((*P(5.5, 17.5), *P(10.5, 22.5)), fill=255)
    elif kind == 'sell':  # a dollar sign
        d.arc((*P(8, 6), *P(20, 15)), 90, 360, fill=255, width=int(2.6 * u))
        d.arc((*P(8, 13), *P(20, 22)), 270, 540, fill=255, width=int(2.6 * u))
        d.line((P(14, 3.5), P(14, 24.5)), fill=255, width=int(2.2 * u))
    elif kind == 'power':  # a lightning bolt
        d.polygon([P(16.5, 3.5), P(7.5, 15.5), P(13, 15.5), P(10.5, 24.5), P(20.5, 11.5), P(15, 11.5)], fill=255)
    elif kind == 'beacon':  # an arrow down onto a marker ring
        d.ellipse((*P(5, 16.5), *P(23, 24.5)), outline=255, width=int(2.4 * u))
        d.rectangle((*P(12.4, 3.5), *P(15.6, 13)), fill=255)
        d.polygon([P(8, 11.5), P(20, 11.5), P(14, 19)], fill=255)
    elif kind == 'options':  # a cog
        points = []
        for i in range(8):
            a = math.pi * 2 * i / 8
            for r, da in ((8, -0.36), (11, -0.2), (11, 0.2), (8, 0.36)):
                points.append(P(14 + r * math.cos(a + da), 14 + r * math.sin(a + da)))
        d.polygon(points, fill=255)
        d.ellipse((*P(9.8, 9.8), *P(18.2, 18.2)), fill=0)
    elif kind == 'up':
        d.polygon([P(14, 7), P(23, 19), P(5, 19)], fill=255)
    elif kind == 'down':
        d.polygon([P(5, 9), P(23, 9), P(14, 21)], fill=255)
    return m

def icon(kind, w, h, side, radius=4):
    t = tile(w, h, side, radius)
    gm = glyph(kind, t.width, t.height)
    t = recess(t, gm, side)
    return t.resize((w, h), Image.LANCZOS)


# The file stem for each icon; the side's letter follows HUD.
ICONS = [('T0', 'build'), ('T1', 'defense'), ('T2', 'infantry'), ('T3', 'vehicle'),
         ('RP', 'repair'), ('SL', 'sell'), ('PW', 'power'), ('BC', 'beacon'), ('OP', 'options'), ('UP', 'up'), ('DN', 'down')]
LETTERS = {'allied': 'A', 'soviet': 'S', 'yuri': 'Y'}
SIZES = {'up': 24, 'down': 24}


def pressed(img):
    """The pressed frame: darker and one pixel lower."""
    dark = Image.eval(img.convert('RGB'), lambda v: v * 3 // 4).convert('RGBA')
    dark.putalpha(img.getchannel('A'))
    out = Image.new('RGBA', img.size, (0, 0, 0, 0))
    out.alpha_composite(dark, (0, 1))
    return out


def disabled(img):
    """The disabled frame: grey and dim."""
    grey = img.convert('L').point(lambda v: v * 3 // 5).convert('RGBA')
    grey.putalpha(img.getchannel('A'))
    return grey


def write_all(folder):
    for side, letter in LETTERS.items():
        for stem, kind in ICONS:
            size = SIZES.get(kind, 28)
            normal = icon(kind, size, size, side)
            sheet = Image.new('RGBA', (size * 3, size), (0, 0, 0, 0))
            for index, frame in enumerate((normal, pressed(normal), disabled(normal))):
                sheet.alpha_composite(frame, (index * size, 0))
            sheet.save(os.path.join(folder, 'HUD%s%s.png' % (letter, stem)), optimize=True)


if __name__ == '__main__':
    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    write_all(os.path.join(root, 'ui'))
