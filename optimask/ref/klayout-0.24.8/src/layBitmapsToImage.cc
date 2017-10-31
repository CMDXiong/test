
/*

  KLayout Layout Viewer
  Copyright (C) 2006-2016 Matthias Koefferlein

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/


#include "layBitmapsToImage.h"
#include "layBitmap.h"
#include "layDitherPattern.h"
#include "tlTimer.h"
#include "tlAssert.h"

#include <QtCore/QMutex>
#include <QtGui/QImage>

namespace lay
{

static void
render_scanline_std (uint32_t dp, const lay::Bitmap *pbitmap, unsigned int y, unsigned int w, uint32_t *data)
{
  const uint32_t *ps = pbitmap->scanline (y);

  unsigned int x = w;
  while (x >= lay::wordlen) {
    *data++ = *ps++ & dp;
    x -= lay::wordlen;
  }

  if (x > 0) {
    *data = *ps & dp;
  }
}

static void
render_scanline_px (uint32_t dp, const lay::Bitmap *pbitmap, 
                    unsigned int y, unsigned int w, unsigned int h, uint32_t *data,
                    unsigned int pixels)
{
  if (pixels < 1) {
    return;
  }

  if (pixels > 15) {
    pixels = 15;
  }
  unsigned int px1 = (pixels - 1) / 2;
  unsigned int px2 = (pixels - 1) - px1;

  const uint32_t *ps[16];
  for (unsigned int p = 0; p < pixels; ++p) {
    if (y + p < px1) {
      ps[p] = pbitmap->scanline (0);
    } else if ((y + p - px1) >= h) {
      ps[p] = pbitmap->scanline (h - 1);
    } else {
      ps[p] = pbitmap->scanline (y + p - px1);
    }
  }

  uint32_t d, dd = 0, dn = 0;
  for (unsigned int p = 0; p < pixels; ++p) {
    dn |= *(ps[p]++);
  }
  dn &= dp;

  unsigned int x = w;
  while (true) {

    d = dn;

    dn = 0;
    if (x > lay::wordlen) {
      for (unsigned int p = 0; p < pixels; ++p) {
        dn |= *(ps[p]++);
      }
      dn &= dp;
    }

    uint32_t d0 = d;
    for (unsigned int p = 1; p <= px1; ++p) {
      d |= (d0 >> p) | (dn << (32 - p));
    }
    for (unsigned int p = 1; p <= px2; ++p) {
      d |= (d0 << p) | (dd >> (32 - p));
    }

    dd = d0;

    *data++ = d;

    if (x > lay::wordlen) {
      x -= lay::wordlen;
    } else {
      break;
    }

  }

}

static void
render_scanline_cross (uint32_t dp, const lay::Bitmap *pbitmap, 
                       unsigned int y, unsigned int w, unsigned int h, uint32_t *data,
                       unsigned int pixels)
{
  if (pixels < 1) {
    return;
  }

  if (pixels > 15) {
    pixels = 15;
  }
  unsigned int px1 = (pixels - 1) / 2;
  unsigned int px2 = (pixels - 1) - px1;

  const uint32_t *ps[16];
  for (unsigned int p = 0; p < pixels; ++p) {
    if (y + p < px1) {
      ps[p] = pbitmap->scanline (0);
    } else if ((y + p - px1) >= h) {
      ps[p] = pbitmap->scanline (h - 1);
    } else {
      ps[p] = pbitmap->scanline (y + p - px1);
    }
  }

  uint32_t d, dd = 0, dn;
  dn = *(ps[px1]++) & dp;

  unsigned int x = w;
  while (true) {

    d = dn;

    dn = 0;
    if (x > lay::wordlen) {
      dn = *(ps[px1]++) & dp;
    }

    uint32_t d0 = d;
    if (d0 != 0) {
      for (unsigned int p = 1; p <= px1; ++p) {
        d |= (d0 >> p);
      }
      for (unsigned int p = 1; p <= px2; ++p) {
        d |= (d0 << p);
      }
    }
    if (dn != 0) {
      for (unsigned int p = 1; p <= px1; ++p) {
        d |= (dn << (32 - p));
      }
    }
    if (dd != 0) {
      for (unsigned int p = 1; p <= px2; ++p) {
        d |= (dd >> (32 - p));
      }
    }
    for (unsigned int p = 0; p < px1; ++p) {
      d |= *(ps[p]++);
    }
    for (unsigned int p = px1 + 1; p < pixels; ++p) {
      d |= *(ps[p]++);
    }

    dd = d0;
    *data++ = d;

    if (x > lay::wordlen) {
      x -= lay::wordlen;
    } else {
      break;
    }

  }
}


static void 
bitmaps_to_image_rgb (const std::vector<lay::ViewOp> &view_ops_in, 
                      const std::vector<lay::Bitmap *> &pbitmaps_in,
                      const lay::DitherPattern &dp, 
                      QImage *pimage, unsigned int width, unsigned int height,
                      bool use_bitmap_index,
                      QMutex *mutex)
{
  unsigned int n_in = view_ops_in.size ();

  std::vector<unsigned int> bm_map;
  std::vector<unsigned int> vo_map;

  vo_map.reserve (n_in);
  for (unsigned int i = 0; i < n_in; ++i) {
    vo_map.push_back (i);
  }

  if (! use_bitmap_index) {
    bm_map = vo_map;
  } else {
    bm_map.reserve (n_in);
    for (unsigned int i = 0; i < n_in; ++i) {
      bm_map.push_back (view_ops_in [i].bitmap_index () < 0 ? i : view_ops_in [i].bitmap_index ());
    }
  }

  std::vector<lay::ViewOp> view_ops;
  std::vector<const lay::Bitmap *> pbitmaps;
  std::vector<std::pair <lay::color_t, lay::color_t> > masks;
  std::vector<uint32_t> non_empty_sls;

  view_ops.reserve (n_in);
  pbitmaps.reserve (n_in);
  masks.reserve (n_in);
  non_empty_sls.reserve (n_in);

  //  to optimize the bitmap generation, the bitmaps are checked
  //  for emptyness in slices of "slice" scanlines
  unsigned int slice = 32;

  //  allocate a pixel buffer large enough to hold a scanline for all 
  //  planes.
  unsigned int nwords = (width + 31) / 32;
  uint32_t *buffer = new uint32_t [n_in * nwords];

  for (unsigned int y = 0; y < height; y++) {

    //  lock bitmaps against change by the redraw thread
    if (mutex) {
      mutex->lock ();
    }

    //  every "slice" scan lines test what bitmaps are empty 
    if (y % slice == 0) { 

      view_ops.erase (view_ops.begin (), view_ops.end ());
      pbitmaps.erase (pbitmaps.begin (), pbitmaps.end ());
      non_empty_sls.erase (non_empty_sls.begin (), non_empty_sls.end ());
      for (unsigned int i = 0; i < n_in; ++i) {

        const lay::ViewOp &vop = view_ops_in [vo_map[i]];
        const lay::Bitmap *pb = 0;
        if (bm_map [i] < pbitmaps_in.size ()) {
          pb = pbitmaps_in [bm_map[i]];
        }

        unsigned int w = vop.width ();

        if (pb != 0 
            && w > 0
            && ((pb->first_scanline () < y + slice && pb->last_scanline () > y) || w > 1)
            && (vop.ormask () | ~vop.andmask ()) != 0) {

          uint32_t non_empty_sl = 0;
          uint32_t m = 1;

          for (unsigned int yy = 0; yy < slice && yy + y < height; ++yy, m <<= 1) {
            if (! pb->is_scanline_empty (yy + y)) {
              non_empty_sl |= m;
            }
          }

          if (non_empty_sl || w > 1) {
            view_ops.push_back (vop);
            pbitmaps.push_back (pb);
            non_empty_sls.push_back (non_empty_sl);
          }

        }

      }

    } 

    //  Collect all necessary informations to transfer a single scanline ..
    
    masks.erase (masks.begin (), masks.end ());

    uint32_t needed_bits = 0xffffff; // alpha channel not needed
    uint32_t *dptr = buffer;
    uint32_t ne_mask = (1 << (y % slice));
    for (unsigned int i = 0; i < view_ops.size (); ++i) {

      if (view_ops [i].width () > 1 || (view_ops [i].width () == 1 && (non_empty_sls [i] & ne_mask) != 0)) {

        uint32_t dither = dp.pattern (view_ops [i].dither_index ()) [y & (lay::wordlen - 1)];
        if (dither != 0) {

          masks.push_back (std::make_pair (view_ops [i].ormask () & needed_bits,
                                           ~view_ops [i].ormask () & view_ops [i].andmask () & needed_bits));

          if (view_ops [i].width () == 1) {
            render_scanline_std (dither, pbitmaps [i], y, width, dptr);
          } else if (view_ops [i].width () > 1) {
            if (view_ops [i].shape () == lay::ViewOp::Rect) {
              render_scanline_px (dither, pbitmaps [i], y, width, height, dptr, (unsigned int) view_ops [i].width ());
            } else if (view_ops [i].shape () == lay::ViewOp::Cross) {
              render_scanline_cross (dither, pbitmaps [i], y, width, height, dptr, (unsigned int) view_ops [i].width ());
            }
          }

          dptr += nwords;

        }

      }

    }

    //  unlock bitmaps against change by the redraw thread
    if (mutex) {
      mutex->unlock ();
    }

    //  .. and do the actual transfer.

    if (masks.size () > 0) {

      lay::color_t *pt = (lay::color_t *) pimage->scanLine (height - 1 - y);
      uint32_t *dptr_end = dptr; 

      unsigned int i = 0;
      for (unsigned int x = 0; x < width; x += 32, ++i) {

        lay::color_t y[32] = {
          0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0,
          0, 0, 0, 0, 0, 0, 0, 0,
        };

        lay::color_t z[32] = { 
          lay::wordones, lay::wordones, lay::wordones, lay::wordones, 
          lay::wordones, lay::wordones, lay::wordones, lay::wordones, 
          lay::wordones, lay::wordones, lay::wordones, lay::wordones, 
          lay::wordones, lay::wordones, lay::wordones, lay::wordones, 
          lay::wordones, lay::wordones, lay::wordones, lay::wordones, 
          lay::wordones, lay::wordones, lay::wordones, lay::wordones, 
          lay::wordones, lay::wordones, lay::wordones, lay::wordones, 
          lay::wordones, lay::wordones, lay::wordones, lay::wordones, 
        };

        dptr = dptr_end - nwords + i;
        for (int j = masks.size () - 1; j >= 0; --j) {
          uint32_t d = *dptr;
          if (d != 0) {
            uint32_t m = 1;
            for (unsigned int k = 0; k < 32 && x + k < width; ++k, m <<= 1) {
              if ((d & m) != 0) { 
                y [k] |= masks [j].first & z [k];
                z [k] &= masks [j].second;
              }
            }
          }
          dptr -= nwords;
        }

        for (unsigned int k = 0; k < 32 && x + k < width; ++k) {
          *pt = (*pt & z[k]) | y[k];
          ++pt;
        }

      }

    }

  }

  //  free the pixel buffer
  delete [] buffer;
}

static void 
bitmaps_to_image_mono (const std::vector<lay::ViewOp> &view_ops_in, 
                       const std::vector<lay::Bitmap *> &pbitmaps_in,
                       const lay::DitherPattern &dp, 
                       QImage *pimage, unsigned int width, unsigned int height,
                       bool use_bitmap_index,
                       QMutex *mutex)
{
  unsigned int n_in = view_ops_in.size ();

  std::vector<unsigned int> bm_map;
  std::vector<unsigned int> vo_map;

  vo_map.reserve (n_in);
  for (unsigned int i = 0; i < n_in; ++i) {
    vo_map.push_back (i);
  }

  if (! use_bitmap_index) {
    bm_map = vo_map;
  } else {
    bm_map.reserve (n_in);
    for (unsigned int i = 0; i < n_in; ++i) {
      bm_map.push_back (view_ops_in [i].bitmap_index () < 0 ? i : view_ops_in [i].bitmap_index ());
    }
  }

  std::vector<lay::ViewOp> view_ops;
  std::vector<const lay::Bitmap *> pbitmaps;
  std::vector<std::pair <lay::color_t, lay::color_t> > masks;
  std::vector<uint32_t> non_empty_sls;

  view_ops.reserve (n_in);
  pbitmaps.reserve (n_in);
  masks.reserve (n_in);
  non_empty_sls.reserve (n_in);

  //  to optimize the bitmap generation, the bitmaps are checked
  //  for emptyness in slices of "slice" scanlines
  unsigned int slice = 32;

  //  allocate a pixel buffer large enough to hold a scanline for all 
  //  planes.
  unsigned int nwords = (width + 31) / 32;
  uint32_t *buffer = new uint32_t [n_in * nwords];

  for (unsigned int y = 0; y < height; y++) {

    //  lock bitmaps against change by the redraw thread
    if (mutex) {
      mutex->lock ();
    }

    //  every "slice" scan lines test what bitmaps are empty 
    if (y % slice == 0) { 

      view_ops.erase (view_ops.begin (), view_ops.end ());
      pbitmaps.erase (pbitmaps.begin (), pbitmaps.end ());
      non_empty_sls.erase (non_empty_sls.begin (), non_empty_sls.end ());
      for (unsigned int i = 0; i < n_in; ++i) {

        const lay::ViewOp &vop = view_ops_in [vo_map[i]];
        const lay::Bitmap *pb = 0;
        if (bm_map [i] < pbitmaps_in.size ()) {
          pb = pbitmaps_in [bm_map[i]];
        }

        unsigned int w = vop.width ();

        if (pb != 0 
            && w > 0
            && ((pb->first_scanline () < y + slice && pb->last_scanline () > y) || w > 1)
            && (vop.ormask () | ~vop.andmask ()) != 0) {

          uint32_t non_empty_sl = 0;
          uint32_t m = 1;

          for (unsigned int yy = 0; yy < slice && yy + y < height; ++yy, m <<= 1) {
            if (! pb->is_scanline_empty (yy + y)) {
              non_empty_sl |= m;
            }
          }

          if (non_empty_sl || w > 1) {
            view_ops.push_back (vop);
            pbitmaps.push_back (pb);
            non_empty_sls.push_back (non_empty_sl);
          }

        }

      }

    } 

    //  Collect all necessary informations to transfer a single scanline ..
    
    masks.erase (masks.begin (), masks.end ());

    uint32_t needed_bits = 0x008000; // only green bit 7 required
    uint32_t *dptr = buffer;
    uint32_t ne_mask = (1 << (y % slice));
    for (unsigned int i = 0; i < view_ops.size (); ++i) {

      if (view_ops [i].width () > 1 || (view_ops [i].width () == 1 && (non_empty_sls [i] & ne_mask) != 0)) {

        uint32_t dither = dp.pattern (view_ops [i].dither_index ()) [y & (lay::wordlen - 1)];
        if (dither != 0) {

          masks.push_back (std::make_pair (view_ops [i].ormask () & needed_bits,
                                           ~view_ops [i].ormask () & view_ops [i].andmask () & needed_bits));

          if (view_ops [i].width () == 1) {
            render_scanline_std (dither, pbitmaps [i], y, width, dptr);
          } else if (view_ops [i].width () > 1) {
            if (view_ops [i].shape () == lay::ViewOp::Rect) {
              render_scanline_px (dither, pbitmaps [i], y, width, height, dptr, (unsigned int) view_ops [i].width ());
            } else if (view_ops [i].shape () == lay::ViewOp::Cross) {
              render_scanline_cross (dither, pbitmaps [i], y, width, height, dptr, (unsigned int) view_ops [i].width ());
            }
          }

          dptr += nwords;

        }

      }

    }

    //  unlock bitmaps against change by the redraw thread
    if (mutex) {
      mutex->unlock ();
    }

    //  .. and do the actual transfer.

    if (masks.size () > 0) {

      lay::color_t *pt = (lay::color_t *) pimage->scanLine (height - 1 - y);
      uint32_t *dptr_end = dptr; 

      unsigned int i = 0;
      for (unsigned int x = 0; x < width; x += 32, ++i) {

        uint32_t y = 0;
        uint32_t z = lay::wordones;

        dptr = dptr_end - nwords + i;
        for (int j = masks.size () - 1; j >= 0; --j) {
          uint32_t d = *dptr;
          if (d != 0) {
            uint32_t m = 1;
            for (unsigned int k = 0; k < 32 && x + k < width; ++k, m <<= 1) {
              if ((d & m) != 0) { 
                if (masks [j].first & needed_bits) {
                  y |= (z & m);
                }
                if (! (masks [j].second & needed_bits)) {
                  z &= ~m;
                }
              }
            }
          }
          dptr -= nwords;
        }

        *pt = (*pt & z) | y;
        ++pt;

      }

    }

  }

  //  free the pixel buffer
  delete [] buffer;
}

void 
bitmaps_to_image (const std::vector<lay::ViewOp> &view_ops_in, 
                  const std::vector<lay::Bitmap *> &pbitmaps_in,
                  const lay::DitherPattern &dp, 
                  QImage *pimage, unsigned int width, unsigned int height,
                  bool use_bitmap_index,
                  QMutex *mutex)
{
  if (pimage->depth () <= 1) {
    bitmaps_to_image_mono (view_ops_in, pbitmaps_in, dp, pimage, width, height, use_bitmap_index, mutex);
  } else {
    bitmaps_to_image_rgb (view_ops_in, pbitmaps_in, dp, pimage, width, height, use_bitmap_index, mutex);
  }
}

void
bitmap_to_bitmap (const lay::ViewOp &view_op, const lay::Bitmap &bitmap,
                  unsigned char *data,
                  unsigned int width, unsigned int height,
                  const lay::DitherPattern &dp)
{
  //  quick exit, if line width is zero
  if (view_op.width () == 0) {
    return;
  }

  unsigned int nwords = (width + 31) / 32;
  uint32_t *buffer = new uint32_t [nwords];

  //  determine endianess ..
  unsigned int x = 0xc0000001;
  unsigned char x0 = ((unsigned char *) &x) [0];

  for (unsigned int y = 0; y < height; ++y) {

    unsigned nbytes = ((width + 7) / 8);

    if (view_op.width () > 1 || ! bitmap.is_scanline_empty (height - 1 - y)) {

      uint32_t dither = dp.pattern (view_op.dither_index ()) [(height - 1 - y) & (lay::wordlen - 1)];

      if (view_op.width () == 1) {
        render_scanline_std (dither, &bitmap, height - 1 - y, width, buffer);
      } else if (view_op.width () > 1) {
        if (view_op.shape () == lay::ViewOp::Rect) {
          render_scanline_px (dither, &bitmap, height - 1 - y, width, height, buffer, (unsigned int) view_op.width ());
        } else if (view_op.shape () == lay::ViewOp::Cross) {
          render_scanline_cross (dither, &bitmap, height - 1 - y, width, height, buffer, (unsigned int) view_op.width ());
        }
      }

      const uint32_t *p = buffer;
      uint32_t d = 0;
      const char *dp = (const char *)&d;

      if (x0 == 0xc0) {

        //  MSB first ..
        while (nbytes >= 4) {
          d = *p++;
          if (d) {
            *data++ |= dp[3];
            *data++ |= dp[2];
            *data++ |= dp[1];
            *data++ |= dp[0];
          } else {
            data += 4;
          }
          nbytes -= 4;
        }
        if (nbytes > 0) {
          d = *p++;
          if (d) {
            dp += 4;
            while (nbytes > 0) {
              *data++ |= *--dp;
              --nbytes;
            }
          } else {
            data += nbytes;
          }
        }

      } else if (x0 == 0x01) {

        //  LSB first ..
        while (nbytes >= 4) {
          d = *p++;
          if (d) {
            *data++ |= dp[0];
            *data++ |= dp[1];
            *data++ |= dp[2];
            *data++ |= dp[3];
          } else {
            data += 4;
          }
          nbytes -= 4;
        }
        if (nbytes > 0) {
          d = *p++;
          if (d) {
            while (nbytes > 0) {
              *data++ |= *dp++;
              --nbytes;
            }
          } else {
            data += nbytes;
          }
        }

      } else {
        //  unable to determine endianess
        tl_assert (false);
      }

    } else {
      data += nbytes;
    }
    
  }

  delete [] buffer;
}

}

