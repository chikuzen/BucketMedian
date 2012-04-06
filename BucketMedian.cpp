/*
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
*/

#include <windows.h>
#include "avisynth26.h"

#pragma warning(disable:4996)

class BucketMedian : public GenericVideoFilter {

    int r;
    int th;
    BYTE th_min;
    BYTE th_max;

public:
    BucketMedian(PClip _child, const int radius, const int thresh, const int min, const int max, IScriptEnvironment* env);
    ~BucketMedian() { };
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
};

BucketMedian::
BucketMedian(PClip _child, const int radius, const int thresh, const int min, const int max, IScriptEnvironment* env) : GenericVideoFilter(_child)
{
    if (!vi.IsPlanar())
        env->ThrowError("BucketMedian: this clip is not planar format.");
    r = radius;
    th = thresh;
    th_min = (BYTE)min;
    th_max = (BYTE)max;

}

PVideoFrame __stdcall BucketMedian::GetFrame(int n, IScriptEnvironment* env)
{
    PVideoFrame src = child->GetFrame(n, env);
    PVideoFrame dst = env->NewVideoFrame(vi);
    int plane = PLANAR_Y;
    int width = dst->GetRowSize(plane);
    int height = dst->GetHeight(plane);

    int src_pitch = src->GetPitch(plane);
    const BYTE* src_origin = src->GetReadPtr(plane);
    BYTE* src_read = (BYTE*)src_origin;

    int dst_pitch = dst->GetPitch(plane);
    BYTE* dst_origin = dst->GetWritePtr(plane);

    const int count = (((2 * r + 1) * (2 * r + 1)) >> 1) + 1;

    for (int pos_y = 0; pos_y < height; pos_y++, dst_origin += dst_pitch, src_read += src_pitch) {
        BYTE* write = dst_origin;
        BYTE* read = src_read;
        for (int pos_x = 0; pos_x < width; pos_x++, write++, read++) {

            if (*read < th_min || *read > th_max) {
                *write = *read;
                continue;
            }

            int bucket[256] = {0};

            for (int y = pos_y - r; y <= pos_y + r; y++) {
                for (int x = pos_x - r; x <= pos_x + r; x++) {
                    int xx = x < 0 ? 0 : x >= width  ? width - 1 : x;
                    int yy = y < 0 ? 0 : y >= height ? height - 1 : y;
                    (*(bucket + *(src_origin + xx + src_pitch * yy)))++;
                }
            }

            int cnt = count;
            int median = -1;
            do {
                cnt -= bucket[++median];
            } while (cnt > 0);
            *write = abs(median - *read) <= th ? (BYTE)median : *read;
        }
    }

    if (!vi.IsY8()) {
        env->BitBlt(dst->GetWritePtr(PLANAR_U), dst->GetPitch(PLANAR_U),
                    src->GetReadPtr(PLANAR_U), src->GetPitch(PLANAR_U),
                    dst->GetRowSize(PLANAR_U), dst->GetHeight(PLANAR_U));
        env->BitBlt(dst->GetWritePtr(PLANAR_V), dst->GetPitch(PLANAR_V),
                    src->GetReadPtr(PLANAR_V), src->GetPitch(PLANAR_V),
                    dst->GetRowSize(PLANAR_V), dst->GetHeight(PLANAR_V));
    }

    return dst;
}

AVSValue __cdecl CreateBucketMedian(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    const int radius = args[1].AsInt(1);
    if (radius < 1 || radius > 255)
        env->ThrowError("BucketMedian: invalid setting, out of 0 < radius < 256");
    const int thresh = args[2].AsInt(1);
    if (thresh < 1)
        env->ThrowError("BucketMedian: thresh needs to be 1 or higher.");
    const int min = args[3].AsInt(0);
    if (min < 0 || min > 254)
        env->ThrowError("BucketMedian: invalid setting, out of 0 <= min < 255");
    const int max = args[4].AsInt(255);
    if (max < 1 || max > 255)
        env->ThrowError("BucketMedian: invalid setting, out of 0 < max <= 255");
    if (min >= max)
        env->ThrowError("BucketMedian: invalid setting, min requires lower than max");

    return new BucketMedian(args[0].AsClip(), radius, thresh, min, max, env);
}

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit2(IScriptEnvironment* env)
{
    env->AddFunction("BucketMedian", "c[radius]i[thresh]i[min]i[max]i", CreateBucketMedian, 0);
    return "BucketMedian - Author: Oka Motofumi";
}
