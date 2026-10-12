#!/usr/bin/env python3
"""
bdf2c.py - convert fixed-width BDF bitmap fonts into const C byte arrays
plus `Font` descriptors (see font.h).

Layout of the emitted data (per glyph, glyphs stored back to back):
  - `height` rows, top row first
  - each row is padded up to a whole number of bytes: bytesPerRow = (width + 7) / 8
  - within a row the leftmost pixel is the MOST significant bit (msbFirst = 1)
  - glyph for character `ch` starts at (ch - firstChar) * bytesPerRow * height

Usage:
  python3 bdf2c.py --out-dir out --prefix spleen \
      --license LICENSE --header-note "Spleen 2.2.0, ..." spleen-8x16.bdf ...
"""
import argparse
import os
import re
import sys

FIRST_CHAR = 32     # space
LAST_CHAR = 126     # '~'


def parse_bdf(path):
    txt = open(path, encoding="utf-8").read()
    m = re.search(r"FONTBOUNDINGBOX (\d+) (\d+) (-?\d+) (-?\d+)", txt)
    if not m:
        raise ValueError(f"{path}: no FONTBOUNDINGBOX")
    w, h, xoff, yoff = (int(v) for v in m.groups())

    glyphs = {}
    for g in re.finditer(
        r"STARTCHAR (?P<name>.*?)\n"
        r"ENCODING (?P<enc>-?\d+)\n.*?"
        r"BBX (?P<bw>\d+) (?P<bh>\d+) (?P<bx>-?\d+) (?P<by>-?\d+)\n"
        r"BITMAP\n(?P<bits>.*?)ENDCHAR",
        txt, re.S,
    ):
        enc = int(g["enc"])
        if not (FIRST_CHAR <= enc <= LAST_CHAR):
            continue
        bbx = (int(g["bw"]), int(g["bh"]), int(g["bx"]), int(g["by"]))
        rows = g["bits"].split()
        glyphs[enc] = (g["name"], bbx, rows)
    return (w, h, xoff, yoff), glyphs


def glyph_bytes(path, enc, name, bbx, rows, cell):
    w, h, xoff, yoff = cell
    bytes_per_row = (w + 7) // 8
    if bbx != cell:
        # Every ASCII glyph in Spleen is a full cell. If a font ever had
        # smaller glyph boxes we would have to place them using the offsets.
        raise ValueError(
            f"{path}: glyph {name!r} (U+{enc:04X}) has BBX {bbx}, cell is {cell}; "
            "converter only supports full-cell glyphs")
    if len(rows) != h:
        raise ValueError(f"{path}: glyph {name!r} has {len(rows)} rows, expected {h}")
    out = []
    for r in rows:
        if len(r) != bytes_per_row * 2:
            raise ValueError(f"{path}: glyph {name!r} row {r!r} is not {bytes_per_row} bytes")
        out.extend(int(r[i:i + 2], 16) for i in range(0, len(r), 2))
    return out


def ident(path):
    # spleen-8x16.bdf -> spleen_8x16
    base = os.path.splitext(os.path.basename(path))[0]
    return re.sub(r"[^0-9A-Za-z]+", "_", base).lower()


def camel(path):
    # spleen-8x16.bdf -> Spleen_8x16
    base = ident(path)
    parts = base.split("_")
    return parts[0].capitalize() + "_" + "_".join(parts[1:])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("bdf", nargs="+")
    ap.add_argument("--out-dir", default=".")
    ap.add_argument("--prefix", default="fonts", help="output file stem")
    ap.add_argument("--license", help="license text to embed in the file header")
    ap.add_argument("--header-note", default="", help="one-line source description")
    args = ap.parse_args()

    os.makedirs(args.out_dir, exist_ok=True)

    license_block = ""
    if args.license:
        lic = open(args.license, encoding="utf-8").read().strip("\n")
        license_block = "\n".join((" * " + l).rstrip() for l in lic.splitlines())

    banner = (
        "/*\n"
        " * GENERATED FILE - produced by bdf2c.py, do not edit by hand.\n"
        f" * Source: {args.header_note}\n"
        f" * Characters: ASCII {FIRST_CHAR}..{LAST_CHAR}\n"
        " *\n"
        " * The glyph bitmaps below are derived from the BDF files named above and\n"
        " * remain subject to the original license, reproduced here:\n"
        " *\n"
        f"{license_block}\n"
        " */\n"
    )

    fonts = []
    for path in sorted(args.bdf, key=lambda p: int(re.search(r"(\d+)x(\d+)", p).group(2))):
        cell, glyphs = parse_bdf(path)
        missing = [c for c in range(FIRST_CHAR, LAST_CHAR + 1) if c not in glyphs]
        if missing:
            raise ValueError(f"{path}: missing glyphs {missing}")
        data = []
        for c in range(FIRST_CHAR, LAST_CHAR + 1):
            name, bbx, rows = glyphs[c]
            data.append((c, glyph_bytes(path, c, name, bbx, rows, cell)))
        fonts.append((path, cell, data))

    # ---------------- .c file ----------------
    c = [banner, f'#include "{args.prefix}_fonts.h"\n']
    for path, cell, data in fonts:
        w, h, _, _ = cell
        bpr = (w + 7) // 8
        arr = f"{ident(path)}_data"
        size = len(data) * bpr * h
        c.append(f"\n/* {w}x{h}, {bpr} byte(s) per row, {bpr * h} bytes per glyph, {size} bytes total */")
        c.append(f"static const uint8_t {arr}[] = {{")
        for ch, bs in data:
            label = chr(ch) if chr(ch) not in "\\*/" else f"0x{ch:02X}"
            c.append(f"\t/* {ch:3d} '{label}' */")
            # up to 16 bytes per source line
            for i in range(0, len(bs), 16):
                c.append("\t" + ", ".join(f"0x{b:02X}" for b in bs[i:i + 16]) + ",")
        c.append("};\n")
        c.append(f"const Font {camel(path).replace('Spleen', 'Font_Spleen')} = {{")
        c.append(f"\t.data      = {arr},")
        c.append(f"\t.width     = {w},")
        c.append(f"\t.height    = {h},")
        c.append(f"\t.firstChar = {FIRST_CHAR},")
        c.append(f"\t.numChars  = {LAST_CHAR - FIRST_CHAR + 1},")
        c.append(f"\t.msbFirst  = 1,")
        c.append("};")
    open(os.path.join(args.out_dir, f"{args.prefix}_fonts.c"), "w").write("\n".join(c) + "\n")

    # ---------------- .h file ----------------
    guard = f"{args.prefix.upper()}_FONTS_H"
    h = [f"/* GENERATED by bdf2c.py - see {args.prefix}_fonts.c for source and license. */",
         f"#ifndef {guard}", f"#define {guard}", "", '#include "font.h"', ""]
    for path, cell, _ in fonts:
        w, hh, _, _ = cell
        h.append(f"extern const Font {camel(path).replace('Spleen', 'Font_Spleen')};   /* {w}x{hh} */")
    h += ["", f"#endif /* {guard} */", ""]
    open(os.path.join(args.out_dir, f"{args.prefix}_fonts.h"), "w").write("\n".join(h))

    for path, cell, data in fonts:
        w, hh, _, _ = cell
        bpr = (w + 7) // 8
        print(f"{os.path.basename(path)}: {w}x{hh}, {len(data) * bpr * hh} bytes")


if __name__ == "__main__":
    try:
        main()
    except ValueError as e:
        sys.exit(f"error: {e}")
