#include <windows.h>
#include "avisynth26.h"

#pragma warning(disable:4996)

class BucketMedian : public GenericVideoFilter {
    PVideoFrame src, dst;
    int r;
    int th_min;
    int th_max;

public:
    BucketMedian(PClip _child, const int radius, const int min, const int max, IScriptEnvironment* env);
    ~BucketMedian() { };
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
};

BucketMedian::
BucketMedian(PClip _child, const int radius, const int min, const int max, IScriptEnvironment* env) : GenericVideoFilter(_child)
{
    if (!vi.IsPlanar())
        env->ThrowError("BucketMedian: this clip is not planar format.");
    r = radius;
    th_min = min;
    th_max = max;
}

PVideoFrame __stdcall BucketMedian::GetFrame(int n, IScriptEnvironment* env)
{
    PVideoFrame src = child->GetFrame(n, env);
    PVideoFrame dst = env->NewVideoFrame(vi);

    int plane = PLANAR_Y;
    const int count = ((2 * r + 1) * (2 * r + 1)) >> 1;
    int width = dst->GetRowSize(plane);
    int height = dst->GetHeight(plane);

    int src_pitch = src->GetPitch(plane);
    const BYTE* ptr_r = src->GetReadPtr(plane);

    int dst_pitch = dst->GetPitch(plane);
    BYTE* ptr_w = dst->GetWritePtr(plane);

    for (int pos_y = 0; pos_y < height; pos_y++) {
        for (int pos_x = 0; pos_x < width; pos_x++) {

            int pix = pos_x + src_pitch * pos_y;
            if (*(ptr_r + pix) < th_min || *(ptr_r + pix) > th_max) {
                *(ptr_w + (pos_x + dst_pitch * pos_y)) = *(ptr_r + pix);
                continue;
            }

            int bucket[256] = {0};
            for (int y = pos_y - r; y <= pos_y + r; y++) {
                for (int x = pos_x - r; x <= pos_x + r; x++) {
                    int xx = x < 0 ? 0 : x >= width  ? width - 1 : x;
                    int yy = y < 0 ? 0 : y >= height ? height - 1 : y;
                    bucket[*(ptr_r + xx + src_pitch * yy)]++;
                }
            }

            int cnt = count;
            unsigned median = 0;
            do {
                cnt -= bucket[median++];
            } while (cnt >= 0);
            *(ptr_w + pos_x + dst_pitch * pos_y) = (BYTE)(median - 1);
        }
    }

    if (!vi.IsY8()) {
        while (plane != PLANAR_V) {
            plane = (plane == PLANAR_Y) ? PLANAR_U : PLANAR_V;
            env->BitBlt(dst->GetWritePtr(plane), dst->GetPitch(plane),
                        src->GetReadPtr(plane), src->GetPitch(plane),
                        dst->GetRowSize(plane), dst->GetHeight(plane));
        }
    }

    return dst;
}

AVSValue __cdecl CreateBucketMedian(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    const int radius = args[1].AsInt(1);
    if (radius < 1 || radius > 255)
        env->ThrowError("BacketMedian: invalid setting, out of 0 < radius < 256");

    const int min = args[2].AsInt(0);
    if (min < 0 || min > 254)
        env->ThrowError("BacketMedian: invalid setting, out of 0 <= min < 255");
    const int max = args[3].AsInt(255);
    if (max < 1 || max > 255)
        env->ThrowError("BacketMedian: invalid setting, out of 0 < max <= 255");
    if (min >= max)
        env->ThrowError("BacketMedian: invalid setting, min requires lower than max");

    return new BucketMedian(args[0].AsClip(), radius, min, max, env);
}

extern "C" __declspec(dllexport) const char* __stdcall AvisynthPluginInit2(IScriptEnvironment* env)
{
    env->AddFunction("BucketMedian", "c[radius]i[min]i[max]i", CreateBucketMedian, 0);
    return "BucketMedian - Author: Oka Motofumi";
}
