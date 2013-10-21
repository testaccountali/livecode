/*
 * Copyright 2006 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "graphics.h"
#include "graphics-internal.h"

#include <assert.h>

#include <SkMath.h>
#include <SkMask.h>
#include <SkColorPriv.h>

#define kBlurRadiusFudgeFactor SkFloatToScalar( .57735f )

static int dilateMask(const uint8_t *src, int src_y_stride, uint8_t *dst, int radius, int width, int height, bool transpose)
{
	int new_width = width + radius * 2;
    int dst_x_stride = transpose ? height : 1;
    int dst_y_stride = transpose ? 1 : new_width;
	for(int y = 0; y < height; ++y)
	{
		uint8_t *dptr;
		dptr = dst + y * dst_y_stride;
		const uint8_t *sptr;
		sptr = src + y * src_y_stride;
		for(int x = -radius; x < width + radius; x++)
		{
			uint8_t v;
			v = 0;
			for(int z = x - radius; z < x + radius; z++)
			{
				if (z < 0)
					continue;
				if (z >= width)
					continue;
				if (sptr[z] > v)
					v = sptr[z];
			}
			*dptr = v;
			dptr += dst_x_stride;
		}
	}
	return new_width;
}

#define INF 1e20

static int dilateDistance(const uint8_t *src, int src_y_stride, uint8_t *dst, int radius, int width, int height, bool transpose)
{
	int new_width = width + radius * 2;
    int dst_x_stride = transpose ? height : 1;
    int dst_y_stride = transpose ? 1 : new_width;

	float *f = new float[new_width];
	float *d = new float[new_width];
	int *v = new int[new_width];
	float *z = new float[new_width + 1];
	
	for(int y = 0; y < height; y++)
	{
		const uint8_t *sptr;
		sptr = src + y * dst_y_stride;
		
		for(int i = 0; i < radius; i++)
		{
			f[i] = 0;
			f[new_width - radius - 1] = 0;
		}
		for(int i = radius; i < new_width - radius; i++)
			f[i] = sptr[i - radius] / 255.0f;
		
		v[0] = 0;
		z[0] = -INF;
		z[1] = +INF;
		
		int k = 0;
		for(int q = 1; q <= new_width - 1; q++)
		{
			float s  = ((f[q]+q*q)-(f[v[k]]+v[k]*v[k]))/(2*q-2*v[k]);
			while (s <= z[k]) {
				k--;
				s  = ((f[q]+q*q)-(f[v[k]]+v[k]*v[k]))/(2*q-2*v[k]);
			}
			k++;
			assert(k < new_width);
			v[k] = q;
			z[k] = s;
			assert(k+1 < new_width + 1);
			z[k+1] = +INF;
		}
		
		int kb = k;
		k = 0;
		for (int q = 0; q <= new_width-1; q++)
		{
			while (z[k+1] < q)
				k++;
			assert(q < new_width);
			assert(v[k] < new_width);
			d[q] = (q-v[k])*(q-v[k]) + f[v[k]];
		}
		
		uint8_t *dptr;
		dptr = dst + y * dst_y_stride;
		for(int x = 0; x < new_width; x++)
		{
			float sqd;
			sqd = sqrt(d[x]);
			sqd -= radius;
			if (sqd < 0.0f)
				sqd = 0.0f;
			if (sqd > 1.0f)
				sqd = 1.0f;
			dptr[x] = (1.0f - sqd) * 255.0f;
			dptr += dst_x_stride;
		}
	}
	
	delete[] f;
	delete[] d;
	delete[] v;
	delete[] z;
	
	return new_width;
}

#define INT_INF 0x7fffffff

#if 0
static void dilateDistanceBoth(const uint8_t *src, uint8_t *dst, int xradius, int yradius, int width, int height, int& r_new_width, int& r_new_height)
{
	int new_width = width + 2 * xradius;
	int new_height = height + 2 * yradius;
	
	// Compute the x distances - the distance from a set pixel is taken to be (255 - value) / 256.
	uint16_t *xd;
	xd = new uint16_t[new_width * height];
	for(int y = 0; y < height; y++)
	{
		uint16_t *xdptr;
		xdptr = xd + new_width * y;
		
		const uint8_t *sptr;
		sptr = src + width * y;
		for(int x = 0; x < width; x++)
		{
			// If there is a value at x, take it (fractional)
			if (sptr[x] != 0)
			{
				xdptr[x + xradius] = 255 - sptr[x];
				continue;
			}
			
			// Otherwise search back...
			int db;
			db = INT_INF;
			for(int z = x; z >= 0; z--)
			{
				if (sptr[z] != 0)
				{
					db = (x - z) * 256 + (255 - sptr[z]);
					break;
				}
			}
			
			// Now search forwards...
			int df;
			df = INT_INF;
			for(int z = x; z < width; z++)
			{
				if (sptr[z] != 0)
				{
					df = (x - z) * 256 + (255 - sptr[z]);
					break;
				}
			}
			
			// Distance is the minimum.
			int d;
			d = SkMin32(db, df);
			if (d != INT_INF)
				xdptr[x + xradius] = d & 0xffff; // claim to 65535
			else
				xdptr[x + xradius] = 255 * 256 + 255; // Use 65535 as 'infinite' for now.
		}
		
		// Now expand the fringes.
		for(int x = 0; x < xradius; x++)
		{
			xdptr[x] = (x * 256 + xdptr[xradius]) & 0xffff;
			xdptr[new_width - x - 1] = (x * 256 + xdptr[new_width - xradius - 1]) & 0xffff;
		}
	}
		
	unsigned int rf;
	rf = xradius * xradius * yradius * yradius;	
	
	// Now use xd to compute the spread mask.
	for(int x = 0; x < new_width; x++)
	{
		for(int y = yradius; y < new_height - yradius; y++)
		{
			uint8_t *dptr = dst + y * new_height + x;
			uint16_t *xdptr;
			xdptr = xd + new_width * y + x;
			
			// If the x distance < 255, then we are done.
			if (xdptr[0] < 256)
			{
				*dptr = xdptr[0];
				continue;
			}
			
			// Otherwise, search up until (yr * xd)^2 + y
			*dptr = 0;
			for(int z = y; z >= 0; z--)
			{
				unsigned int yf;
				yf = (y - z) * yradius;
				yf *= yf;
				if (yf >= rf)
					break;
				
				unsigned int xf;
				xf = yradius * (xd[z * new_width + x] >> 8);
				xf *= xf;
				if (xf < rf - yf)
				{
					*dptr = 255;
					break;
				}
			}
			
			if (*dptr == 255)
				continue;
			
			// Search downwards
			for(int z = y; z < new_height - yradius; z++)
			{
				unsigned int yf;
				yf = (z - y) * yradius;
				yf *= yf;
				if (yf >= rf)
					break;
				
				unsigned int xf;
				xf = yradius * (xd[z * new_width + x] >> 8);
				xf *= xf;
				if (xf < rf - yf)
				{
					*dptr = 255;
					break;
				}
			}
		}
	}
	
	r_new_width = new_width;
	r_new_height = new_height;
}
#endif

static void dilateDistanceBoth(const uint8_t *src, uint8_t *dst, int xradius, int yradius, int width, int height, int& r_new_width, int& r_new_height)
{
	int new_width = width + 2 * xradius;
	int new_height = height + 2 * yradius;

	// Compute the x distances - the distance from a set pixel is taken to be (255 - value) / 256.
	uint8_t *xd;
	xd = new uint8_t[new_width * height];
	for(int y = 0; y < height; y++)
	{
		uint8_t *xdptr;
		xdptr = xd + new_width * y;
		
		const uint8_t *sptr;
		sptr = src + width * y;
		for(int x = 0; x < width; x++)
		{
			// If there is a value at x, take it
			if (sptr[x] != 0)
			{
				xdptr[x + xradius] = 0;
				continue;
			}
			
			// Otherwise search back...
			int db;
			db = INT_INF;
			for(int z = x; z >= 0; z--)
			{
				if (sptr[z] != 0)
				{
					db = x - z;
					break;
				}
			}
			
			// Now search forwards...
			int df;
			df = INT_INF;
			for(int z = x; z < width; z++)
			{
				if (sptr[z] != 0)
				{
					df = z - x;
					break;
				}
			}
			
			// Distance is the minimum.
			int d;
			d = SkMin32(db, df);
			if (d < 255)
				xdptr[x + xradius] = d;
			else
				xdptr[x + xradius] = 255;
		}
		
		// Now expand the fringes.
		for(int x = 0; x < xradius; x++)
		{
			if (xdptr[xradius] + (xradius - x) < 255)
				xdptr[x] = ((xradius - x) + xdptr[xradius]);
			else
				xdptr[x] = 255;

			if (xdptr[width + xradius - 1] + x + 1 < 255)
				xdptr[width + xradius + x] = xdptr[width + xradius - 1] + x + 1;
			else
				xdptr[width + xradius + x] = 255;
		}
	}
	
	unsigned int rf;
	rf = xradius * xradius * yradius * yradius;	
	
	memset(dst, 0, new_width * new_height);
	
	// Now use xd to compute the spread mask.
	for(int x = 0; x < new_width; x++)
	{
		for(int y = 0; y < new_height; y++)
		{
			uint8_t *dptr = dst + y * new_width + x;
			
			// If the distance at x, y is 0 then we are done.
			if (y >= yradius && y < (new_height - yradius) && xd[(y - yradius) * new_width + x] == 0)
			{
				*dptr = 255;
				continue;
			}
			
			// Otherwise, search up.
			*dptr = 0;
			for(int z = y; z >= 0; z--)
			{
				unsigned int yf;
				yf = (y - z) * yradius;
				yf *= yf;
				if (yf >= rf)
					break;
				
				unsigned int xf;
				if (z >= yradius && z < (new_height - yradius))
					xf = yradius * xd[(z - yradius) * new_width + x];
				else
					xf = yradius * 255;
				xf *= xf;
				if (xf < rf - yf)
				{
					*dptr = 255;
					break;
				}
			}
			
			if (*dptr == 255)
				continue;
			
			// Search downwards
			for(int z = y; z < new_height; z++)
			{
				unsigned int yf;
				yf = (z - y) * yradius;
				yf *= yf;
				if (yf >= rf)
					break;
				
				unsigned int xf;
				if (z >= yradius && z < (new_height - yradius))
					xf = yradius * xd[(z - yradius) * new_width + x];
				else
					xf = yradius * 255;
				xf *= xf;
				if (xf < rf - yf)
				{
					*dptr = 255;
					break;
				}
			}
		}
	}
	
	r_new_width = new_width;
	r_new_height = new_height;
}

////////////////////////////////////////////////////////////////////////////////
//
//  Low / Normal quality blurs - box-blur technique.
//

#define UNROLL_SEPARABLE_LOOPS

/**
 * This function performs a box blur in X, of the given radius.  If the
 * "transpose" parameter is true, it will transpose the pixels on write,
 * such that X and Y are swapped. Reads are always performed from contiguous
 * memory in X, for speed. The destination buffer (dst) must be at least
 * (width + leftRadius + rightRadius) * height bytes in size.
 */
static int boxBlur(const uint8_t* src, int src_y_stride, uint8_t* dst,
                   int leftRadius, int rightRadius, int width, int height,
                   bool transpose)
{
    int diameter = leftRadius + rightRadius;
    int kernelSize = diameter + 1;
    int border = SkMin32(width, diameter);
    uint32_t scale = (1 << 24) / kernelSize;
    int new_width = width + SkMax32(leftRadius, rightRadius) * 2;
    int dst_x_stride = transpose ? height : 1;
    int dst_y_stride = transpose ? 1 : new_width;
    for (int y = 0; y < height; ++y) {
        int sum = 0;
        uint8_t* dptr = dst + y * dst_y_stride;
        const uint8_t* right = src + y * src_y_stride;
        const uint8_t* left = right;
        for (int x = 0; x < rightRadius - leftRadius; x++) {
            *dptr = 0;
            dptr += dst_x_stride;
        }
#define LEFT_BORDER_ITER \
			sum += *right++; \
			*dptr = (sum * scale) >> 24; \
			dptr += dst_x_stride;
		
        int x = 0;
#ifdef UNROLL_SEPARABLE_LOOPS
        for (; x < border - 16; x += 16) {
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
        }
#endif
        for (; x < border; ++x) {
            LEFT_BORDER_ITER
        }
#undef LEFT_BORDER_ITER
#define TRIVIAL_ITER \
			*dptr = (sum * scale) >> 24; \
			dptr += dst_x_stride;
        x = width;
#ifdef UNROLL_SEPARABLE_LOOPS
        for (; x < diameter - 16; x += 16) {
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
            TRIVIAL_ITER
        }
#endif
        for (; x < diameter; ++x) {
            TRIVIAL_ITER
        }
#undef TRIVIAL_ITER
#define CENTER_ITER \
			sum += *right++; \
			*dptr = (sum * scale) >> 24; \
			sum -= *left++; \
			dptr += dst_x_stride;
		
        x = diameter;
#ifdef UNROLL_SEPARABLE_LOOPS
        for (; x < width - 16; x += 16) {
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
        }
#endif
        for (; x < width; ++x) {
            CENTER_ITER
        }
#undef CENTER_ITER
#define RIGHT_BORDER_ITER \
			*dptr = (sum * scale) >> 24; \
			sum -= *left++; \
			dptr += dst_x_stride;
		
        x = 0;
#ifdef UNROLL_SEPARABLE_LOOPS
        for (; x < border - 16; x += 16) {
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
        }
#endif
        for (; x < border; ++x) {
            RIGHT_BORDER_ITER
        }
#undef RIGHT_BORDER_ITER
        for (int x = 0; x < leftRadius - rightRadius; x++) {
            *dptr = 0;
            dptr += dst_x_stride;
        }
        SkASSERT(sum == 0);
    }
    return new_width;
}

/**
 * This variant of the box blur handles blurring of non-integer radii.  It
 * keeps two running sums: an outer sum for the rounded-up kernel radius, and
 * an inner sum for the rounded-down kernel radius.  For each pixel, it linearly
 * interpolates between them.  In float this would be:
 *  outer_weight * outer_sum / kernelSize +
 *  (1.0 - outer_weight) * innerSum / (kernelSize - 2)
 */
static int boxBlurInterp(const uint8_t* src, int src_y_stride, uint8_t* dst,
                         int radius, int width, int height,
                         bool transpose, uint8_t outer_weight)
{
    int diameter = radius * 2;
    int kernelSize = diameter + 1;
    int border = SkMin32(width, diameter);
    int inner_weight = 255 - outer_weight;
    outer_weight += outer_weight >> 7;
    inner_weight += inner_weight >> 7;
    uint32_t outer_scale = (outer_weight << 16) / kernelSize;
    uint32_t inner_scale = (inner_weight << 16) / (kernelSize - 2);
    int new_width = width + diameter;
    int dst_x_stride = transpose ? height : 1;
    int dst_y_stride = transpose ? 1 : new_width;
    for (int y = 0; y < height; ++y) {
        int outer_sum = 0, inner_sum = 0;
        uint8_t* dptr = dst + y * dst_y_stride;
        const uint8_t* right = src + y * src_y_stride;
        const uint8_t* left = right;
        int x = 0;
		
#define LEFT_BORDER_ITER \
			inner_sum = outer_sum; \
			outer_sum += *right++; \
			*dptr = (outer_sum * outer_scale + inner_sum * inner_scale) >> 24; \
			dptr += dst_x_stride;
		
#ifdef UNROLL_SEPARABLE_LOOPS
        for (;x < border - 16; x += 16) {
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
            LEFT_BORDER_ITER
        }
#endif
		
        for (;x < border; x++) {
            LEFT_BORDER_ITER
        }
#undef LEFT_BORDER_ITER
        for (int x = width; x < diameter; ++x) {
            *dptr = (outer_sum * outer_scale + inner_sum * inner_scale) >> 24;
            dptr += dst_x_stride;
        }
        x = diameter;
		
#define CENTER_ITER \
			inner_sum = outer_sum - *left; \
			outer_sum += *right++; \
			*dptr = (outer_sum * outer_scale + inner_sum * inner_scale) >> 24; \
			dptr += dst_x_stride; \
			outer_sum -= *left++;
		
#ifdef UNROLL_SEPARABLE_LOOPS
        for (; x < width - 16; x += 16) {
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
            CENTER_ITER
        }
#endif
        for (; x < width; ++x) {
            CENTER_ITER
        }
#undef CENTER_ITER
		
#define RIGHT_BORDER_ITER \
		inner_sum = outer_sum - *left++; \
		*dptr = (outer_sum * outer_scale + inner_sum * inner_scale) >> 24; \
		dptr += dst_x_stride; \
		outer_sum = inner_sum;
		
        x = 0;
#ifdef UNROLL_SEPARABLE_LOOPS
        for (; x < border - 16; x += 16) {
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
            RIGHT_BORDER_ITER
        }
#endif
        for (; x < border; x++) {
            RIGHT_BORDER_ITER
        }
#undef RIGHT_BORDER_ITER
        SkASSERT(outer_sum == 0 && inner_sum == 0);
    }
    return new_width;
}

static void get_adjusted_radii(SkScalar passRadius, int *loRadius, int *hiRadius)
{
    *loRadius = *hiRadius = SkScalarCeil(passRadius);
    if (SkIntToScalar(*hiRadius) - passRadius > SkFloatToScalar(0.5f)) {
        *loRadius = *hiRadius - 1;
    }
}

// pass 0 is (radius + (3 - 0 - 1)) / (3 - 0) = (radius + 2) / 3  (4)
// radius -= p0_radius
// pass 1 is (radius + (3 - 1 - 1)) / (3 - 1) = (radius + 1) / 2  (3)
// radius -= p1_radius
// pass 2 is (radius + (3 - 2 - 1)) / (3 - 2) = (radius + 0) / 1  (3)

bool MCGBlurBox(const SkMask& p_src, SkScalar p_x_radius, SkScalar p_y_radius, SkScalar p_x_spread, SkScalar p_y_spread, SkMask& r_dst)
{
	int t_pass_count;
	t_pass_count = 3;
	
	int x_spread, y_spread;
	x_spread = SkScalarFloor(p_x_radius * p_x_spread);
	y_spread = SkScalarFloor(p_y_radius * p_y_spread);
	
	p_x_radius -= x_spread;
	p_y_radius -= y_spread;
	
	int rx, ry;
	rx = SkScalarCeil(p_x_radius);
	ry = SkScalarCeil(p_y_radius);
	
	SkScalar px, py;
	px = rx;
	py = ry;
	
	int wx, wy;
	wx = 255 - SkScalarRound((SkIntToScalar(rx) - px) * 255);
	wy = 255 - SkScalarRound((SkIntToScalar(ry) - py) * 255);
	
	int t_pad_x, t_pad_y;
	t_pad_x = rx + x_spread;
	t_pad_y = ry + y_spread;
	
	r_dst . fBounds . set(p_src . fBounds . fLeft - t_pad_x, p_src . fBounds . fTop - t_pad_y,
						  p_src . fBounds . fRight + t_pad_x, p_src . fBounds . fBottom + t_pad_y);
	r_dst . fRowBytes = r_dst . fBounds . width();
	r_dst . fFormat = SkMask::kA8_Format;
	r_dst . fImage = NULL;
	
	if (p_src . fImage == NULL)
		return true;
	
	size_t t_dst_size;
	t_dst_size = r_dst . computeImageSize();
	if (t_dst_size == 0)
		return false;
	
	int sw, sh;
	sw = p_src . fBounds . width();
	sh = p_src . fBounds . height();
	
	const uint8_t *sp;
	sp = p_src . fImage;
	
	uint8_t *dp;
	dp = SkMask::AllocImage(t_dst_size);
	if (dp == nil)
		return false;
	
	uint8_t *tp;
	tp = SkMask::AllocImage(t_dst_size);
	if (tp == nil)
	{
		SkMask::FreeImage(dp);
		return false;
	}
	
	int w, h;
	w = sw;
	h = sh;
	if (wx == 255)
	{
		if (t_pass_count == 3)
		{
			int r;
			r = rx;
			
			bool t_has_spread;
			t_has_spread = false;
			if (x_spread != 0 || y_spread != 0)
			{
				//w = dilateMask(sp, p_src . fRowBytes, tp, x_spread, w, h, true);
				//h = dilateMask(tp, h, dp, y_spread, h, w, true);
				//w = dilateDistance(sp, p_src . fRowBytes, tp, x_spread, w, h, true);
				//h = dilateDistance(tp, h, dp, y_spread, h, w, true);
				dilateDistanceBoth(sp, dp, x_spread, y_spread, w, h, w, h);
				t_has_spread = true;
			}
			
			int trx;
			trx = (r + 2) / 3; 
			r -= trx;
			
			int rx_lo, rx_hi;
			rx_lo = rx_hi = trx;
			
			w = boxBlur(t_has_spread ? dp : sp, t_has_spread ? w : p_src . fRowBytes, tp, rx_lo, rx_hi, w, h, false);
			
			trx = (r + 1) / 2;
			r -= trx;
			
			rx_lo = rx_hi = trx;
			w = boxBlur(tp, w, dp, rx_hi, rx_lo, w, h, false);
			
			trx = r;
			rx_lo = rx_hi = trx;
			w = boxBlur(dp, w, tp, rx_hi, rx_hi, w, h, true);
		}
		else
			w = boxBlur(sp, p_src . fRowBytes, tp, rx, rx, w, h, true);
	}
	else
	{
		if (t_pass_count == 3)
		{
			int r;
			r = rx;
			
			bool t_has_spread;
			t_has_spread = false;
			if (x_spread != 0 || y_spread != 0)
			{
				//w = dilateMask(sp, p_src . fRowBytes, tp, x_spread, w, h, true);
				//h = dilateMask(tp, h, dp, y_spread, h, w, true);
				//w = dilateDistance(sp, p_src . fRowBytes, tp, x_spread, w, h, true);
				//h = dilateDistance(tp, h, dp, y_spread, h, w, true);
				dilateDistanceBoth(sp, dp, x_spread, y_spread, w, h, w, h);
				t_has_spread = true;
			}
			
			int trx;
			trx = (r + 2) / 3; 
			r -= trx;
			
			w = boxBlurInterp(t_has_spread ? dp : sp, t_has_spread ? w : p_src . fRowBytes, tp, trx, w, h, false, wx);
			
			trx = (r + 1) / 2;
			r -= trx;
			
			w = boxBlurInterp(tp, w, dp, trx, w, h, false, wx);
			
			trx = r;
			
			w = boxBlurInterp(dp, w, tp, trx, w, h, true, wx);
		}
		else
			w = boxBlurInterp(sp, p_src . fRowBytes, tp, rx, w, h, true, wx);
	}
	
	if (wy == 255)
	{
		if (t_pass_count == 3)
		{
			int r;
			r = ry;
			
			int sry;
			sry = (r + 2) / 3;
			r -= sry;
			
			int ry_lo, ry_hi;
			ry_lo = ry_hi = sry;
			
			h = boxBlur(tp, h, dp, ry_lo, ry_hi, h, w, false);
			
			sry = (r + 1) / 2;
			r -= sry;
			ry_lo = ry_hi = sry;
			
			h = boxBlur(dp, h, tp, ry_hi, ry_lo, h, w, false);
			
			sry = r;
			ry_lo = ry_hi = sry;
			
			h = boxBlur(tp, h, dp, ry_hi, ry_hi, h, w, true);
		}
		else
			h = boxBlur(tp, h, dp, ry, ry, h, w, true);
	}
	else
	{
		if (t_pass_count == 3)
		{
			int r;
			r = ry;
			
			int sry;
			sry = (r + 2) / 3;
			r -= sry;
			
			h = boxBlurInterp(tp, h, dp, sry, h, w, false, wy);
			
			sry = (r + 1) / 2;
			r -= sry;
			
			h = boxBlurInterp(dp, h, tp, sry, h, w, false, wy);
			
			sry = r;
			
			h = boxBlurInterp(tp, h, dp, sry, h, w, true, wy);
		}
		else
			w = boxBlurInterp(tp, h, dp, rx, h, w, true, wy);
	}
	
	SkMask::FreeImage(tp);
	
	r_dst . fImage = dp;
	
	return true;
}


#if 0
////////////////////////////////////////////////////////////////////////////////
//
//  High quality blurs - uses convolution
//

// Unrolling the integer blur kernel seems to give us a ~15% speedup on Windows,
// breakeven on Mac, and ~15% slowdown on Linux.
// Reading a word at a time when bulding the sum buffer seems to give
// us no appreciable speedup on Windows or Mac, and 2% slowdown on Linux.
#if defined(SK_BUILD_FOR_WIN32)
#define UNROLL_KERNEL_LOOP 1
#endif

/** The sum buffer is an array of u32 to hold the accumulated sum of all of the
 src values at their position, plus all values above and to the left.
 When we sample into this buffer, we need an initial row and column of 0s,
 so we have an index correspondence as follows:
 
 src[i, j] == sum[i+1, j+1]
 sum[0, j] == sum[i, 0] == 0
 
 We assume that the sum buffer's stride == its width
 */
static void build_sum_buffer(uint32_t sum[], int srcW, int srcH,
                             const uint8_t src[], int srcRB) {
    int sumW = srcW + 1;
	
    SkASSERT(srcRB >= srcW);
    // mod srcRB so we can apply it after each row
    srcRB -= srcW;
	
    int x, y;
	
    // zero out the top row and column
    memset(sum, 0, sumW * sizeof(sum[0]));
    sum += sumW;
	
    // special case first row
    uint32_t X = 0;
    *sum++ = 0; // initialze the first column to 0
    for (x = srcW - 1; x >= 0; --x) {
        X = *src++ + X;
        *sum++ = X;
    }
    src += srcRB;
	
    // now do the rest of the rows
    for (y = srcH - 1; y > 0; --y) {
        uint32_t L = 0;
        uint32_t C = 0;
        *sum++ = 0; // initialze the first column to 0
		
        for (x = srcW - 1; !SkIsAlign4((intptr_t) src) && x >= 0; x--) {
            uint32_t T = sum[-sumW];
            X = *src++ + L + T - C;
            *sum++ = X;
            L = X;
            C = T;
        }
		
        for (; x >= 4; x-=4) {
            uint32_t T = sum[-sumW];
            X = *src++ + L + T - C;
            *sum++ = X;
            L = X;
            C = T;
            T = sum[-sumW];
            X = *src++ + L + T - C;
            *sum++ = X;
            L = X;
            C = T;
            T = sum[-sumW];
            X = *src++ + L + T - C;
            *sum++ = X;
            L = X;
            C = T;
            T = sum[-sumW];
            X = *src++ + L + T - C;
            *sum++ = X;
            L = X;
            C = T;
        }
		
        for (; x >= 0; --x) {
            uint32_t T = sum[-sumW];
            X = *src++ + L + T - C;
            *sum++ = X;
            L = X;
            C = T;
        }
        src += srcRB;
    }
}

/**
 * This is the path for apply_kernel() to be taken when the kernel
 * is wider than the source image.
 */
static void kernel_clamped(uint8_t dst[], int rx, int ry, const uint32_t sum[],
                           int sw, int sh) {
    SkASSERT(2*rx > sw);
	
    uint32_t scale = (1 << 24) / ((2*rx + 1)*(2*ry + 1));
	
    int sumStride = sw + 1;
	
    int dw = sw + 2*rx;
    int dh = sh + 2*ry;
	
    int prev_y = -2*ry;
    int next_y = 1;
	
    for (int y = 0; y < dh; y++) {
        int py = SkClampPos(prev_y) * sumStride;
        int ny = SkFastMin32(next_y, sh) * sumStride;
		
        int prev_x = -2*rx;
        int next_x = 1;
		
        for (int x = 0; x < dw; x++) {
            int px = SkClampPos(prev_x);
            int nx = SkFastMin32(next_x, sw);
			
            uint32_t tmp = sum[px+py] + sum[nx+ny] - sum[nx+py] - sum[px+ny];
            *dst++ = SkToU8(tmp * scale >> 24);
			
            prev_x += 1;
            next_x += 1;
        }
		
        prev_y += 1;
        next_y += 1;
    }
}

/**
 *  sw and sh are the width and height of the src. Since the sum buffer
 *  matches that, but has an extra row and col at the beginning (with zeros),
 *  we can just use sw and sh as our "max" values for pinning coordinates
 *  when sampling into sum[][]
 *
 *  The inner loop is conceptually simple; we break it into several sections
 *  to improve performance. Here's the original version:
 for (int x = 0; x < dw; x++) {
 int px = SkClampPos(prev_x);
 int nx = SkFastMin32(next_x, sw);
 
 uint32_t tmp = sum[px+py] + sum[nx+ny] - sum[nx+py] - sum[px+ny];
 *dst++ = SkToU8(tmp * scale >> 24);
 
 prev_x += 1;
 next_x += 1;
 }
 *  The sections are:
 *     left-hand section, where prev_x is clamped to 0
 *     center section, where neither prev_x nor next_x is clamped
 *     right-hand section, where next_x is clamped to sw
 *  On some operating systems, the center section is unrolled for additional
 *  speedup.
 */
static void apply_kernel(uint8_t dst[], int rx, int ry, const uint32_t sum[],
                         int sw, int sh) {
    if (2*rx > sw) {
        kernel_clamped(dst, rx, ry, sum, sw, sh);
        return;
    }
	
    uint32_t scale = (1 << 24) / ((2*rx + 1)*(2*ry + 1));
	
    int sumStride = sw + 1;
	
    int dw = sw + 2*rx;
    int dh = sh + 2*ry;
	
    int prev_y = -2*ry;
    int next_y = 1;
	
    SkASSERT(2*rx <= dw - 2*rx);
	
    for (int y = 0; y < dh; y++) {
        int py = SkClampPos(prev_y) * sumStride;
        int ny = SkFastMin32(next_y, sh) * sumStride;
		
        int prev_x = -2*rx;
        int next_x = 1;
        int x = 0;
		
        for (; x < 2*rx; x++) {
            SkASSERT(prev_x <= 0);
            SkASSERT(next_x <= sw);
			
            int px = 0;
            int nx = next_x;
			
            uint32_t tmp = sum[px+py] + sum[nx+ny] - sum[nx+py] - sum[px+ny];
            *dst++ = SkToU8(tmp * scale >> 24);
			
            prev_x += 1;
            next_x += 1;
        }
		
        int i0 = prev_x + py;
        int i1 = next_x + ny;
        int i2 = next_x + py;
        int i3 = prev_x + ny;
		
#if UNROLL_KERNEL_LOOP
        for (; x < dw - 2*rx - 4; x += 4) {
            SkASSERT(prev_x >= 0);
            SkASSERT(next_x <= sw);
			
            uint32_t tmp = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            *dst++ = SkToU8(tmp * scale >> 24);
            tmp = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            *dst++ = SkToU8(tmp * scale >> 24);
            tmp = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            *dst++ = SkToU8(tmp * scale >> 24);
            tmp = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            *dst++ = SkToU8(tmp * scale >> 24);
			
            prev_x += 4;
            next_x += 4;
        }
#endif
		
        for (; x < dw - 2*rx; x++) {
            SkASSERT(prev_x >= 0);
            SkASSERT(next_x <= sw);
			
            uint32_t tmp = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            *dst++ = SkToU8(tmp * scale >> 24);
			
            prev_x += 1;
            next_x += 1;
        }
		
        for (; x < dw; x++) {
            SkASSERT(prev_x >= 0);
            SkASSERT(next_x > sw);
			
            int px = prev_x;
            int nx = sw;
			
            uint32_t tmp = sum[px+py] + sum[nx+ny] - sum[nx+py] - sum[px+ny];
            *dst++ = SkToU8(tmp * scale >> 24);
			
            prev_x += 1;
            next_x += 1;
        }
		
        prev_y += 1;
        next_y += 1;
    }
}

/**
 * This is the path for apply_kernel_interp() to be taken when the kernel
 * is wider than the source image.
 */
static void kernel_interp_clamped(uint8_t dst[], int rx, int ry,
								  const uint32_t sum[], int sw, int sh, U8CPU outer_weight) {
    SkASSERT(2*rx > sw);
	
    int inner_weight = 255 - outer_weight;
	
    // round these guys up if they're bigger than 127
    outer_weight += outer_weight >> 7;
    inner_weight += inner_weight >> 7;
	
    uint32_t outer_scale = (outer_weight << 16) / ((2*rx + 1)*(2*ry + 1));
    uint32_t inner_scale = (inner_weight << 16) / ((2*rx - 1)*(2*ry - 1));
	
    int sumStride = sw + 1;
	
    int dw = sw + 2*rx;
    int dh = sh + 2*ry;
	
    int prev_y = -2*ry;
    int next_y = 1;
	
    for (int y = 0; y < dh; y++) {
        int py = SkClampPos(prev_y) * sumStride;
        int ny = SkFastMin32(next_y, sh) * sumStride;
		
        int ipy = SkClampPos(prev_y + 1) * sumStride;
        int iny = SkClampMax(next_y - 1, sh) * sumStride;
		
        int prev_x = -2*rx;
        int next_x = 1;
		
        for (int x = 0; x < dw; x++) {
            int px = SkClampPos(prev_x);
            int nx = SkFastMin32(next_x, sw);
			
            int ipx = SkClampPos(prev_x + 1);
            int inx = SkClampMax(next_x - 1, sw);
			
            uint32_t outer_sum = sum[px+py] + sum[nx+ny]
			- sum[nx+py] - sum[px+ny];
            uint32_t inner_sum = sum[ipx+ipy] + sum[inx+iny]
			- sum[inx+ipy] - sum[ipx+iny];
            *dst++ = SkToU8((outer_sum * outer_scale
							 + inner_sum * inner_scale) >> 24);
			
            prev_x += 1;
            next_x += 1;
        }
        prev_y += 1;
        next_y += 1;
    }
}

/**
 *  sw and sh are the width and height of the src. Since the sum buffer
 *  matches that, but has an extra row and col at the beginning (with zeros),
 *  we can just use sw and sh as our "max" values for pinning coordinates
 *  when sampling into sum[][]
 *
 *  The inner loop is conceptually simple; we break it into several variants
 *  to improve performance. Here's the original version:
 for (int x = 0; x < dw; x++) {
 int px = SkClampPos(prev_x);
 int nx = SkFastMin32(next_x, sw);
 
 int ipx = SkClampPos(prev_x + 1);
 int inx = SkClampMax(next_x - 1, sw);
 
 uint32_t outer_sum = sum[px+py] + sum[nx+ny]
 - sum[nx+py] - sum[px+ny];
 uint32_t inner_sum = sum[ipx+ipy] + sum[inx+iny]
 - sum[inx+ipy] - sum[ipx+iny];
 *dst++ = SkToU8((outer_sum * outer_scale
 + inner_sum * inner_scale) >> 24);
 
 prev_x += 1;
 next_x += 1;
 }
 *  The sections are:
 *     left-hand section, where prev_x is clamped to 0
 *     center section, where neither prev_x nor next_x is clamped
 *     right-hand section, where next_x is clamped to sw
 *  On some operating systems, the center section is unrolled for additional
 *  speedup.
 */
static void apply_kernel_interp(uint8_t dst[], int rx, int ry,
								const uint32_t sum[], int sw, int sh, U8CPU outer_weight) {
    SkASSERT(rx > 0 && ry > 0);
    SkASSERT(outer_weight <= 255);
	
    if (2*rx > sw) {
        kernel_interp_clamped(dst, rx, ry, sum, sw, sh, outer_weight);
        return;
    }
	
    int inner_weight = 255 - outer_weight;
	
    // round these guys up if they're bigger than 127
    outer_weight += outer_weight >> 7;
    inner_weight += inner_weight >> 7;
	
    uint32_t outer_scale = (outer_weight << 16) / ((2*rx + 1)*(2*ry + 1));
    uint32_t inner_scale = (inner_weight << 16) / ((2*rx - 1)*(2*ry - 1));
	
    int sumStride = sw + 1;
	
    int dw = sw + 2*rx;
    int dh = sh + 2*ry;
	
    int prev_y = -2*ry;
    int next_y = 1;
	
    SkASSERT(2*rx <= dw - 2*rx);
	
    for (int y = 0; y < dh; y++) {
        int py = SkClampPos(prev_y) * sumStride;
        int ny = SkFastMin32(next_y, sh) * sumStride;
		
        int ipy = SkClampPos(prev_y + 1) * sumStride;
        int iny = SkClampMax(next_y - 1, sh) * sumStride;
		
        int prev_x = -2*rx;
        int next_x = 1;
        int x = 0;
		
        for (; x < 2*rx; x++) {
            SkASSERT(prev_x < 0);
            SkASSERT(next_x <= sw);
			
            int px = 0;
            int nx = next_x;
			
            int ipx = 0;
            int inx = next_x - 1;
			
            uint32_t outer_sum = sum[px+py] + sum[nx+ny]
			- sum[nx+py] - sum[px+ny];
            uint32_t inner_sum = sum[ipx+ipy] + sum[inx+iny]
			- sum[inx+ipy] - sum[ipx+iny];
            *dst++ = SkToU8((outer_sum * outer_scale
							 + inner_sum * inner_scale) >> 24);
			
            prev_x += 1;
            next_x += 1;
        }
		
        int i0 = prev_x + py;
        int i1 = next_x + ny;
        int i2 = next_x + py;
        int i3 = prev_x + ny;
        int i4 = prev_x + 1 + ipy;
        int i5 = next_x - 1 + iny;
        int i6 = next_x - 1 + ipy;
        int i7 = prev_x + 1 + iny;
		
#if UNROLL_KERNEL_LOOP
        for (; x < dw - 2*rx - 4; x += 4) {
            SkASSERT(prev_x >= 0);
            SkASSERT(next_x <= sw);
			
            uint32_t outer_sum = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            uint32_t inner_sum = sum[i4++] + sum[i5++] - sum[i6++] - sum[i7++];
            *dst++ = SkToU8((outer_sum * outer_scale
							 + inner_sum * inner_scale) >> 24);
            outer_sum = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            inner_sum = sum[i4++] + sum[i5++] - sum[i6++] - sum[i7++];
            *dst++ = SkToU8((outer_sum * outer_scale
							 + inner_sum * inner_scale) >> 24);
            outer_sum = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            inner_sum = sum[i4++] + sum[i5++] - sum[i6++] - sum[i7++];
            *dst++ = SkToU8((outer_sum * outer_scale
							 + inner_sum * inner_scale) >> 24);
            outer_sum = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            inner_sum = sum[i4++] + sum[i5++] - sum[i6++] - sum[i7++];
            *dst++ = SkToU8((outer_sum * outer_scale
							 + inner_sum * inner_scale) >> 24);
			
            prev_x += 4;
            next_x += 4;
        }
#endif
		
        for (; x < dw - 2*rx; x++) {
            SkASSERT(prev_x >= 0);
            SkASSERT(next_x <= sw);
			
            uint32_t outer_sum = sum[i0++] + sum[i1++] - sum[i2++] - sum[i3++];
            uint32_t inner_sum = sum[i4++] + sum[i5++] - sum[i6++] - sum[i7++];
            *dst++ = SkToU8((outer_sum * outer_scale
							 + inner_sum * inner_scale) >> 24);
			
            prev_x += 1;
            next_x += 1;
        }
		
        for (; x < dw; x++) {
            SkASSERT(prev_x >= 0);
            SkASSERT(next_x > sw);
			
            int px = prev_x;
            int nx = sw;
			
            int ipx = prev_x + 1;
            int inx = sw;
			
            uint32_t outer_sum = sum[px+py] + sum[nx+ny]
			- sum[nx+py] - sum[px+ny];
            uint32_t inner_sum = sum[ipx+ipy] + sum[inx+iny]
			- sum[inx+ipy] - sum[ipx+iny];
            *dst++ = SkToU8((outer_sum * outer_scale
							 + inner_sum * inner_scale) >> 24);
			
            prev_x += 1;
            next_x += 1;
        }
		
        prev_y += 1;
        next_y += 1;
    }
}
#endif
