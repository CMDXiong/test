
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



#ifndef HDR_layDitherPattern
#define HDR_layDitherPattern

#include "config.h"

#include <QtCore/QObject>
#include <QtGui/QBitmap>

#include "dbObject.h"

#include <stdint.h>

#include <vector>
#include <string>
#include <map>

namespace lay
{

/**
 *  @brief A class representing a single dither pattern
 */
class KLAYOUT_DLL DitherPatternInfo
{
public:
  /** 
   *  @brief The default constructor
   */
  DitherPatternInfo ();
  
  /** 
   *  @brief The copy constructor
   */
  DitherPatternInfo (const DitherPatternInfo &d);
  
  /**
   *  @brief Assignment operator
   */
  DitherPatternInfo &operator= (const DitherPatternInfo &d);

  /**
   *  @brief Comparison of pattern bitmap
   */
  bool same_bitmap (const DitherPatternInfo &d) const;

  /**
   *  @brief Comparison of pattern bitmap (operator<)
   */
  bool less_bitmap (const DitherPatternInfo &d) const;

  /**
   *  @brief Equality operator
   *
   *  This operator compares bitmaps, names and order index
   */
  bool operator== (const DitherPatternInfo &d) const;

  /**
   *  @brief Equality operator
   *
   *  This operator compares bitmaps, names and order index
   */
  bool operator< (const DitherPatternInfo &d) const;

  /**
   *  @brief Inequality operator
   */
  bool operator!= (const DitherPatternInfo &d) const
  {
    return !operator== (d);
  }

  /**
   *  @brief Read access to the name
   */
  const std::string &name () const
  {
    return m_name;
  }

  /**
   *  @brief Write access to the name
   */
  void set_name (const std::string &name) 
  {
    m_name = name;
  }

  /**
   *  @brief Read access to the order index
   */
  unsigned int order_index () const
  {
    return m_order_index;
  }

  /**
   *  @brief Write access to the name
   */
  void set_order_index (unsigned int oi) 
  {
    m_order_index = oi;
  }

  /**
   *  @brief Get a monochrome bitmap object for this pattern
   *
   *  @param width The desired width (-1 for default)
   *  @param height The desired height (-1 for default)
   */
  QBitmap get_bitmap (int width = -1, int height = -1) const;

  /**
   *  @brief Access to the dither pattern
   *
   *  The pattern returned is guaranteed to be at least of
   *  size 32x64.
   */
  const uint32_t *pattern () const
  {
    return m_pattern;
  }

  /**
   *  @brief Replace the dither pattern
   *
   *  'w' and 'h' denote the width and height of the pattern passed.
   *  If 'w' is less than 32, the lowest 'w' bits must contain the pattern.
   *  'w' and 'h' are supposed to be a integer fraction of 32.
   */
  void set_pattern (const uint32_t *pattern, unsigned int w, unsigned int h);

  /**
   *  @brief Load from a string
   */
  void from_string (const std::string &s);

  /**
   *  @brief Convert to string
   */
  std::string to_string () const;

  /**
   *  @brief Load from a set of strings (one per line)
   */
  void from_strings (const std::vector<std::string> &s);

  /**
   *  @brief Convert to strings (one per line)
   */
  std::vector<std::string> to_strings () const;

private:
  uint32_t m_pattern[64];
  unsigned int m_order_index;
  std::string m_name;
};

/**
 *  @brief This class represents the set of dither pattern available
 *
 *  The main method for accessing the pattern is through the "pattern"
 *  method which delivers a 32 word set per index. The pattern can be 
 *  replaced with a new pattern, except for the first 16 pattern which
 *  cannot be changed. 
 *  There is a global instance representing the active pattern set.
 *  The pattern set can deliver a signal if one pattern is changed.
 */
class KLAYOUT_DLL DitherPattern
  : public QObject, public db::Object
{
Q_OBJECT

public:
  typedef std::vector<DitherPatternInfo> pattern_vector;
  typedef pattern_vector::const_iterator iterator;

  /**
   *  @brief The default constructor
   *
   *  This method initializes the first 16 pattern.
   */
  DitherPattern ();

  /**
   *  @brief The copy constructor
   */
  DitherPattern (const DitherPattern &d);

  /**
   *  @brief The destructor
   */
  ~DitherPattern ();

  /**
   *  @brief Assignment operator
   */
  DitherPattern &operator= (const DitherPattern &p);

  /**
   *  @brief Equality
   */
  bool operator== (const DitherPattern &p) const
  {
    return m_pattern == p.m_pattern;
  }

  /**
   *  @brief Inequality
   */
  bool operator!= (const DitherPattern &p) const
  {
    return m_pattern != p.m_pattern;
  }

  /**
   *  @brief Get a monochrome bitmap object for this pattern
   *
   *  If the index is not valid, an empty bitmap is returned.
   *
   *  @param i The index of the pattern to get the bitmap of
   *  @param width The desired width (-1 for default)
   *  @param height The desired height (-1 for default)
   */
  QBitmap get_bitmap (unsigned int i, int width = -1, int height = -1) const;

  /**
   *  @brief Deliver the pattern with the given index
   *
   *  If the index is not valid, an empty pattern is returned.
   *  The pattern returned are two adjacent 32 word blocks containing
   *  the pattern twice.
   */
  const uint32_t *pattern (unsigned int i) const;

  /**
   *  @brief Replace the pattern with the given index
   *
   *  The first 16 pattern cannot be replaced. In this case, the change
   *  request is simply ignored. 
   *  By replacing the pattern with one with an order_index of 0, 
   *  the pattern is virtually deleted (such pattern are not shown in the editor)
   */
  void replace_pattern (unsigned int i, const DitherPatternInfo &p);

  /**
   *  @brief Add a new pattern, searching for a empty slot and returning that index
   *
   *  This method will look for the first pattern with a order index of 0 
   *  or create a new entry if no such pattern exists. This entry will be used
   *  to place the pattern to. The order_index will be set to the highest value
   *  plus one thus placing the new pattern at the end of the list in the editor.
   */
  unsigned int add_pattern (const DitherPatternInfo &p);

  /**
   *  @brief Renumber the order indices to numbers increasing by 1 only
   *
   *  This method should be called when a pattern is deleted by setting it's
   *  order_index to 0.
   */
  void renumber ();

  /**
   *  @brief Merge two dither pattern lists
   *
   *  *this is filled with all the pattern of "other" which are not
   *  member of this list yet. A mapping table is filled, mapping 
   *  an index of "other" to an index inside *this;
   */
  void merge (const DitherPattern &other, std::map<unsigned int, unsigned int> &index_map);

  /**
   *  @brief Return the number stipples 
   */
  unsigned int count () const
  {
    return (unsigned int) m_pattern.size ();
  }

  /**
   *  @brief The begin iterator delivering the custom pattern objects
   * 
   *  The corresponding end iterator is delivered with end()
   */
  iterator begin_custom () const;

  /**
   *  @brief The begin iterator delivering all pattern objects
   */
  iterator begin () const 
  {
    return m_pattern.begin ();
  }

  /**
   *  @brief The begin iterator delivering the past-the-end pattern object
   */
  iterator end () const 
  {
    return m_pattern.end ();
  }

  /**
   *  @brief Implementation of the db::Object interface
   */
  void undo (db::Op *op);

  /**
   *  @brief Implementation of the db::Object interface
   */
  void redo (db::Op *op);

  /**
   *  @brief Accessor to the default dither pattern set
   */
  static const DitherPattern &default_pattern ();

signals:
  /**
   *  @brief This signal is emitted if a pattern is changed 
   */
  void changed ();
  
private:
  std::vector<DitherPatternInfo> m_pattern;
};

}

#endif

