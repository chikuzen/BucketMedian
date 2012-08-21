function BMBorderProc(clip c, int "r", int "th", int "min", int "max")
{
    r = default(r, 1)
    th = default(th, 1)
    min = default(min, 0)
    max = default(max, 255)
    w = c.width() + r * 2
    h = c.height() + r * 2
    
    return c.PointResize(w, h, -r, -r, w, h).BucketMedian(r, th, min, max).Crop(r, r, -r, -r)
}
