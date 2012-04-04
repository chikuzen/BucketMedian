BucketMedian.dll - an AviSynth filter plugin

    Copyright (C) 2012 Oka Motofumi (chikuzen.mo at gmail dot com)

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted, provided that the above
copyright notice and this permission notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


What's this ? :
    BucketMedian is an implementation of spatial median fileter
    adapting bucket(counting) sort algorithm.

Requirement:
    AviSynth2.5.8 or later
    msvcrt100.dll(Microsoft VisualC++2010 Redistributable Package)

How to use ? :

    BucketMedian(clip c, int "radius", int "min", int "max")

    clip :
        supported only planar formats (YV12, YV24, YV16, Y8).
        and, this filter process Y plane only.

    radius (1 to 255, default: 1 (3x3)):
        The neighborhood pixel radius to reference.
        box size is (radius*2+1) * (radius*2+1)

    min (0 to 254, default: 0):
        threshold of low pix value.
        if source's value is lower than this, the pix is not processed.
       

    max (1 to 255, default:255):
        Threshold of high pix value.
        if source's value is higher than this, the pix is not processed.

FAQ :
    Q: Why does this filter make my video blurry as hell ?
    A: Because this is a median filter.

    Q: Why is this filter slow like administration of Japan ?
    A: Patches welcome.

    Q: Why did you write such a trivial code ?
    A: Since it was windy yesterday, I wasn't able to play outside.

    Q: Where can I get the source code ?
    A: https://github.com/chikuzen/BucketMedian/
