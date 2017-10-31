
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



#include "layDitherPattern.h"

#include <ctype.h>
#include <algorithm>

namespace lay
{

// ---------------------------------------------------------------------
//  The standard dither pattern

static const char *dither_strings [] = {

  // 0: solid
  "solid",
  "*",

  // 1: hollow
  "hollow",
  ".", 

  // 2: dotted
  "dotted",
  "*.\n"
  ".*",

  // 3: coarsly dotted
  "coarsly dotted",
  "*...\n"
  "....\n"
  "..*.\n"
  "....",

  // 4: left-hatched
  "left-hatched",
  "*...\n"
  ".*..\n"
  "..*.\n"
  "...*",

  // 5: lightly left-hatched
  "lightly left-hatched",
  "*.......\n"
  ".*......\n"
  "..*.....\n"
  "...*....\n"
  "....*...\n"
  ".....*..\n"
  "......*.\n"
  ".......*",

  // 6: strongly left-hatched dense
  "strongly left-hatched dense",
  "**..\n"
  ".**.\n"
  "..**\n"
  "*..*",

  // 7: strongly left-hatched sparse
  "strongly left-hatched sparse",
  "**......\n"
  ".**.....\n"
  "..**....\n"
  "...**...\n"
  "....**..\n"
  ".....**.\n"
  "......**\n"
  "*......*",

  // 8: right-hatched
  "right-hatched",
  "*...\n"
  "...*\n"
  "..*.\n"
  ".*..",

  // 9: lightly right-hatched
  "lightly right-hatched",
  "*.......\n"
  ".......*\n"
  "......*.\n"
  ".....*..\n"
  "....*...\n"
  "...*....\n"
  "..*.....\n"
  ".*......",

  // 10: strongly right-hatched dense
  "strongly right-hatched dense",
  "**..\n"
  "*..*\n"
  "..**\n"
  ".**.",

  // 11: strongly right-hatched sparse
  "strongly right-hatched sparse",
  "**......\n"
  "*......*\n"
  "......**\n"
  ".....**.\n"
  "....**..\n"
  "...**...\n"
  "..**....\n"
  ".**.....",

  // 12: cross-hatched
  "cross-hatched",
  "*...\n"
  ".*.*\n"
  "..*.\n"
  ".*.*",

  // 13: lightly cross-hatched
  "lightly cross-hatched",
  "*.......\n"
  ".*.....*\n"
  "..*...*.\n"
  "...*.*..\n"
  "....*...\n"
  "...*.*..\n"
  "..*...*.\n"
  ".*.....*",

  // 14: checkerboard 2px
  "checkerboard 2px",
  "**..\n"
  "**..\n"
  "..**\n"
  "..**",

  // 15: strongly cross-hatched sparse
  "strongly cross-hatched sparse",
  "**......\n"
  "***....*\n"
  "..**..**\n"
  "...****.\n"
  "....**..\n"
  "...****.\n"
  "..**..**\n"
  "***....*",

  // 16: heavy checkerboard
  "heavy checkerboard",
  "****....\n"
  "****....\n"
  "****....\n"
  "****....\n"
  "....****\n"
  "....****\n"
  "....****\n"
  "....****",

  // 17: hollow bubbles
  "hollow bubbles",
  ".*...*..\n"
  "*.*.....\n"
  ".*...*..\n"
  "....*.*.\n"
  ".*...*..\n"
  "*.*.....\n"
  ".*...*..\n"
  "....*.*.",

  // 18: solid bubbles
  "solid bubbles",
  ".*...*..\n"
  "***.....\n"
  ".*...*..\n"
  "....***.\n"
  ".*...*..\n"
  "***.....\n"
  ".*...*..\n"
  "....***.",

  // 19: pyramids
  "pyramids",
  ".*......\n"
  "*.*.....\n"
  "****...*\n"
  "........\n"
  "....*...\n"
  "...*.*..\n"
  "..*****.\n"
  "........",

  // 20: turned pyramids
  "turned pyramids",
  "****...*\n"
  "*.*.....\n"
  ".*......\n"
  "........\n"
  "..*****.\n"
  "...*.*..\n"
  "....*...\n"
  "........",

  // 21: plus
  "plus",
  "..*...*.\n"
  "..*.....\n"
  "*****...\n"
  "..*.....\n"
  "..*...*.\n"
  "......*.\n"
  "*...****\n"
  "......*.",

  // 22: minus
  "minus",
  "........\n"
  "........\n"
  "*****...\n"
  "........\n"
  "........\n"
  "........\n"
  "*...****\n"
  "........",

  // 23: 22.5 degree down
  "22.5 degree down",
  "*......*\n"
  ".**.....\n"
  "...**...\n"
  ".....**.\n"
  "*......*\n"
  ".**.....\n"
  "...**...\n"
  ".....**.",

  // 24: 22.5 degree up
  "22.5 degree up",
  "*......*\n"
  ".....**.\n"
  "...**...\n"
  ".**.....\n"
  "*......*\n"
  ".....**.\n"
  "...**...\n"
  ".**.....",

  // 25: 67.5 degree down
  "67.5 degree down",
  "*...*...\n"
  ".*...*..\n"
  ".*...*..\n"
  "..*...*.\n"
  "..*...*.\n"
  "...*...*\n"
  "...*...*\n"
  "*...*...",

  // 26: 67.5 degree up
  "67.5 degree up",
  "...*...*\n"
  "..*...*.\n"
  "..*...*.\n"
  ".*...*..\n"
  ".*...*..\n"
  "*...*...\n"
  "*...*...\n"
  "...*...*",

  // 27: 22.5 cross hatched
  "22.5 degree cross hatched",
  "*......*\n"
  ".**..**.\n"
  "...**...\n"
  ".**..**.\n"
  "*......*\n"
  ".**..**.\n"
  "...**...\n"
  ".**..**.",

  // 28: zig zag
  "zig zag",
  "..*...*.\n"
  ".*.*.*.*\n"
  "*...*...\n"
  "........\n"
  "..*...*.\n"
  ".*.*.*.*\n"
  "*...*...\n"
  "........",

  // 29: sine 
  "sine",
  "..***...\n"
  ".*...*..\n"
  "*.....**\n"
  "........\n"
  "..***...\n"
  ".*...*..\n"
  "*.....**\n"
  "........",

  // 30: special pattern for light heavy dithering
  "heavy unordered",
  "****.*.*\n"
  "**.****.\n"
  "*.**.***\n"
  "*****.*.\n"
  ".**.****\n"
  "**.***.*\n"
  ".****.**\n"
  "*.*.****",

  // 31: special pattern for light frame dithering
  "light unordered",
  "....*.*.\n"
  "..*....*\n"
  ".*..*...\n"
  ".....*.*\n"
  "*..*....\n"
  "..*...*.\n"
  "*....*..\n"
  ".*.*....",

  // 32: vertical dense
  "vertical dense",
  "*.\n"
  "*.\n",

  // 33: vertical 
  "vertical",
  ".*..\n"
  ".*..\n"
  ".*..\n"
  ".*..\n",

  // 34: vertical thick
  "vertical thick",
  ".**.\n"
  ".**.\n"
  ".**.\n"
  ".**.\n",

  // 35: vertical sparse
  "vertical sparse",
  "...*....\n"
  "...*....\n"
  "...*....\n"
  "...*....\n",

  // 36: vertical sparse, thick
  "vertical sparse, thick",
  "...**...\n"
  "...**...\n"
  "...**...\n"
  "...**...\n",

  // 37: horizontal dense
  "horizontal dense",
  "**\n"
  "..\n",

  // 38: horizontal 
  "horizontal",
  "....\n"
  "****\n"
  "....\n"
  "....\n",

  // 39: horizontal thick
  "horizontal thick",
  "....\n"
  "****\n"
  "****\n"
  "....\n",

  // 40: horizontal 
  "horizontal sparse",
  "........\n"
  "........\n"
  "........\n"
  "********\n"
  "........\n"
  "........\n"
  "........\n"
  "........\n",

  // 41: horizontal 
  "horizontal sparse, thick",
  "........\n"
  "........\n"
  "........\n"
  "********\n"
  "********\n"
  "........\n"
  "........\n"
  "........\n",

  // 42: grid dense
  "grid dense",
  "**\n"
  "*.\n",

  // 43: grid 
  "grid",
  ".*..\n"
  "****\n"
  ".*..\n"
  ".*..\n",

  // 44: grid thick
  "grid thick",
  ".**.\n"
  "****\n"
  "****\n"
  ".**.\n",

  // 45: grid sparse
  "grid sparse",
  "...*....\n"
  "...*....\n"
  "...*....\n"
  "********\n"
  "...*....\n"
  "...*....\n"
  "...*....\n"
  "...*....\n",

  // 46: grid sparse, thick
  "grid sparse, thick",
  "...**...\n"
  "...**...\n"
  "...**...\n"
  "********\n"
  "********\n"
  "...**...\n"
  "...**...\n"
  "...**...\n",
};


// ---------------------------------------------------------------------
//  DitherPatternInfo implementation

DitherPatternInfo::DitherPatternInfo ()
  : m_order_index (0)
{
  memset (m_pattern, 0, sizeof (m_pattern));
}
  
DitherPatternInfo::DitherPatternInfo (const DitherPatternInfo &d)
  : m_order_index (d.m_order_index), m_name (d.m_name)
{
  memcpy (m_pattern, d.m_pattern, sizeof (m_pattern));
}
  
DitherPatternInfo &
DitherPatternInfo::operator= (const DitherPatternInfo &d)
{
  if (&d != this) {
    m_order_index = d.m_order_index;
    m_name = d.m_name;
    memcpy (m_pattern, d.m_pattern, sizeof (m_pattern));
  }
  return *this;
}

bool 
DitherPatternInfo::same_bitmap (const DitherPatternInfo &d) const
{
  for (unsigned int i = 0; i < sizeof (m_pattern) / sizeof (m_pattern [0]); ++i) {
    if (m_pattern [i] != d.m_pattern [i]) {
      return false;
    }
  }
  return true;
}

bool 
DitherPatternInfo::less_bitmap (const DitherPatternInfo &d) const
{
  for (unsigned int i = 0; i < sizeof (m_pattern) / sizeof (m_pattern [0]); ++i) {
    if (m_pattern [i] < d.m_pattern [i]) {
      return true;
    } else if (m_pattern [i] > d.m_pattern [i]) {
      return false;
    }
  }
  return false;
}

bool 
DitherPatternInfo::operator== (const DitherPatternInfo &d) const
{
  return same_bitmap (d) && m_name == d.m_name && m_order_index == d.m_order_index;
}

bool 
DitherPatternInfo::operator< (const DitherPatternInfo &d) const
{
  if (! same_bitmap (d)) {
    return less_bitmap (d);
  } 
  if (m_name != d.m_name) {
    return m_name < d.m_name;
  }
  return m_order_index < d.m_order_index;
}

QBitmap
DitherPatternInfo::get_bitmap (int width, int height) const
{
  if (height < 0) {
    height = 36;
  }
  if (width < 0) {
    width = 34;
  }

  const uint32_t *p = pattern ();
  unsigned int stride = (width + 7) / 8;

  unsigned char *data = new unsigned char[stride * height];
  memset (data, 0x00, size_t (stride * height));

  for (unsigned int i = 1; i < (unsigned int)(height - 1); ++i) {
    for (unsigned int j = 0; j < stride; ++j) {
      data [i * stride + j] = 0xff;
    }
  }

  for (unsigned int i = 0; i < (unsigned int)(height - 4); ++i) {
    uint32_t w = p [(height - 5 - i) % 32];
    for (unsigned int j = 0; j < (unsigned int)(width - 2); ++j) {
      if (! (w & (1 << (j % 32)))) {
        data [stride * (i + 2) + (j + 1) / 8] &= ~(1 << ((j + 1) % 8));
      }
    }
  }

  QBitmap bitmap (QBitmap::fromData (QSize (width, height), data, QImage::Format_MonoLSB));
  delete[] data;

  return bitmap;
}

void 
DitherPatternInfo::set_pattern (const uint32_t *pt, unsigned int w, unsigned int h) 
{
  if (w == 0) {
    w = 1;
  } else if (w >= 32) {
    w = 32;
  } 

  if (h == 0) {
    h = 1;
  } else if (h >= 32) {
    h = 32;
  } 

  for (unsigned int n = 0; n < 64; ++n) {

    uint32_t wd;
    if (w < 32) {
      wd = pt[n % h] & ((1 << w) - 1);
    } else {
      wd = pt[n % h];
    }

    unsigned int wl = w;
    while (wl < 32) {
      wd = wd | (wd << wl);
      wl *= 2;
    }

    m_pattern [n] = wd;

  }

}

std::string
DitherPatternInfo::to_string () const
{
  std::string res;
  
  for (unsigned int i = 0; i < 32; ++i) {
    res += "          ";
    for (unsigned int j = 0; j < 32; ++j) {
      if ((m_pattern [32 - i - 1] & (1 << j)) != 0) {
        res += "*";
      } else {
        res += ".";
      }
    }
    res += "\n";
  }

  return res;
}

std::vector <std::string>
DitherPatternInfo::to_strings () const
{
  std::vector <std::string> res;
  
  for (unsigned int i = 0; i < 32; ++i) {
    std::string r;
    for (unsigned int j = 0; j < 32; ++j) {
      if ((m_pattern [32 - i - 1] & (1 << j)) != 0) {
        r += "*";
      } else {
        r += ".";
      }
    }
    res.push_back (r);
  }

  return res;
}

static const char *uint_from_string (const char *s, uint32_t &w)
{
  while (*s && isspace (*s)) {
    ++s;
  }

  w = 0;

  const char *s0 = s;
  const char *s1 = s0;
  while (*s1 >= ' ') {
    ++s1;
  }

  unsigned int b = 1;
  while (b > 0) {
    if (*s < ' ') {
      s = s0;
    } 
    if (*s++ == '*') {
      w |= b;
    }
    b <<= 1;
  }

  return s1;
}

void
DitherPatternInfo::from_strings (const std::vector<std::string> &strv) 
{
  for (unsigned int l = 0; l < 64; ++l) {
    m_pattern[64 - l - 1] = 0;
    if (strv.size () > 0) {
      const char *s = strv [l % strv.size ()].c_str ();
      uint_from_string (s, m_pattern [64 - l - 1]);
    }
  }
}

void
DitherPatternInfo::from_string (const std::string &cstr) 
{
  const char *ds = cstr.c_str ();
  const char *s = ds;

  for (unsigned int l = 0; l < 64; ++l) {

    m_pattern[64 - l - 1] = 0;

    while (*s && isspace (*s)) {
      ++s;
    }
    if (! *s) {
      s = ds;
    }
    
    s = uint_from_string (s, m_pattern [64 - l - 1]);

  }
}

// ---------------------------------------------------------------------
//  DitherPattern implementation

struct ReplaceDitherPatternOp
  : public db::Op
{
  ReplaceDitherPatternOp (unsigned int i, const DitherPatternInfo &o, const DitherPatternInfo &n) 
    : db::Op (), index (i), m_old (o), m_new (n) 
  { }

  unsigned int index;
  DitherPatternInfo m_old, m_new;
};

DitherPattern::DitherPattern ()
  : QObject (), db::Object (0)
{
  for (unsigned int d = 0; d < sizeof (dither_strings) / sizeof (dither_strings [0]); d += 2) {
    m_pattern.push_back (DitherPatternInfo ());
    m_pattern.back ().set_name (dither_strings [d]);
    m_pattern.back ().from_string (dither_strings [d + 1]);
  }
}

DitherPattern::~DitherPattern ()
{
  //  .. nothing yet ..
}

DitherPattern::DitherPattern (const DitherPattern &p)
{
  m_pattern = p.m_pattern;
}

DitherPattern & 
DitherPattern::operator= (const DitherPattern &p)
{
  if (this != &p) {
    unsigned int i;
    for (i = 0; i < p.count (); ++i) {
      replace_pattern (i, p.begin () [i]);
    }
    for ( ; i < count (); ++i) {
      replace_pattern (i, DitherPatternInfo ());
    }
  }
  return *this;
}

QBitmap
DitherPattern::get_bitmap (unsigned int i, int width, int height) const
{
  if (i < count ()) {
    return m_pattern [i].get_bitmap (width, height);
  } else {
    return m_pattern [1].get_bitmap (width, height);
  }
}

const uint32_t *
DitherPattern::pattern (unsigned int i) const
{
  unsigned int off = (i >> 16) % 32;
  i &= 0xffff;
  if (i < count ()) {
    return m_pattern [i].pattern () + off;
  } else {
    return m_pattern [1].pattern (); //  empty
  }
}

void 
DitherPattern::replace_pattern (unsigned int i, const DitherPatternInfo &p)
{
  bool chg = false;

  while (i >= count ()) {
    m_pattern.push_back (DitherPatternInfo ());
    chg = true;
  }

  if (m_pattern [i] != p) {
    if (manager () && manager ()->transacting ()) {
      manager ()->queue (this, new ReplaceDitherPatternOp (i, m_pattern [i], p));
    }
    m_pattern [i] = p;
    chg = true;
  }

  //  if something has changed emit the signal
  if (chg) {
    emit changed ();
  }
}

unsigned int 
DitherPattern::add_pattern (const DitherPatternInfo &p)
{
  unsigned int oi = 0;
  lay::DitherPattern::iterator iempty = end ();
  for (lay::DitherPattern::iterator i = begin_custom (); i != end (); ++i) {
    if (i->order_index () == 0) {
      iempty = i;
    } else if (i->order_index () > oi) {
      oi = i->order_index ();
    } 
  }

  unsigned int index = std::distance (begin (), iempty);

  replace_pattern (index, p); 
  m_pattern [index].set_order_index (oi + 1);

  return index;
}

namespace {
  struct display_order
  {
    bool operator () (lay::DitherPattern::iterator a, lay::DitherPattern::iterator b)
    {
      return a->order_index () < b->order_index ();
    }
  };
}

void 
DitherPattern::renumber ()
{
  //  renumber the order indices
  std::vector <lay::DitherPattern::iterator> iters; 
  for (lay::DitherPattern::iterator i = begin_custom (); i != end (); ++i) {
    iters.push_back (i);
  }
  std::sort (iters.begin (), iters.end (), display_order ());

  unsigned int oi = 1;
  for (std::vector <lay::DitherPattern::iterator>::const_iterator i = iters.begin (); i != iters.end (); ++i) {
    if ((*i)->order_index () > 0) {
      lay::DitherPatternInfo p (**i);
      p.set_order_index (oi++);
      replace_pattern (std::distance (begin (), *i), p);
    }
  }
}

DitherPattern::iterator 
DitherPattern::begin_custom () const 
{
  return m_pattern.begin () + sizeof (dither_strings) / sizeof (dither_strings [0]) / 2;
}

const DitherPattern &
DitherPattern::default_pattern () 
{
  static DitherPattern empty;
  return empty;
}

void 
DitherPattern::undo (db::Op *op)
{
  const ReplaceDitherPatternOp *rop = dynamic_cast <const ReplaceDitherPatternOp *> (op);
  if (rop) {
    replace_pattern (rop->index, rop->m_old);
  }
}

void 
DitherPattern::redo (db::Op *op)
{
  const ReplaceDitherPatternOp *rop = dynamic_cast <const ReplaceDitherPatternOp *> (op);
  if (rop) {
    replace_pattern (rop->index, rop->m_new);
  }
}

struct pattern_less_f
{
  bool operator() (const DitherPatternInfo &a, const DitherPatternInfo &b) const
  {
    return a.less_bitmap (b);
  }
};

void 
DitherPattern::merge (const DitherPattern &other, std::map<unsigned int, unsigned int> &index_map)
{
  //  insert the standard pattern into the map (for completeness)
  for (iterator c = begin (); c != begin_custom (); ++c) {
    index_map.insert (std::make_pair (std::distance (begin (), c), std::distance (begin (), c)));
  }

  //  build an index of present pattern
  std::map <DitherPatternInfo, unsigned int, pattern_less_f> patterns;
  for (iterator c = begin_custom (); c != end (); ++c) {
    patterns.insert (std::make_pair (*c, std::distance (begin (), c)));
  }

  //  map the pattern of other into *this, possibly creating new ones
  for (iterator c = other.begin_custom (); c != other.end (); ++c) {
    std::map <DitherPatternInfo, unsigned int, pattern_less_f>::const_iterator p = patterns.find (*c);
    unsigned int new_index;
    if (p == patterns.end ()) {
      new_index = add_pattern (*c);
      patterns.insert (std::make_pair (*c, new_index));
    } else {
      new_index = p->second;
    }
    index_map.insert (std::make_pair (std::distance (other.begin (), c), new_index));
  }
}

}

