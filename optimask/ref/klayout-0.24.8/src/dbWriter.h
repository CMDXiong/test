
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


#ifndef HDR_dbWriter
#define HDR_dbWriter

#include "config.h"

#include "tlException.h"
#include "dbSaveLayoutOptions.h"
#include "dbMetaInfo.h"

namespace tl 
{
  class OutputStream;
  class OutputStreamBase;
}

namespace db
{

class Layout;

/**
 *  @brief The generic writer base class
 */
class KLAYOUT_DLL WriterBase
{
public:
  typedef std::vector<MetaInfo> meta_info;
  typedef meta_info::const_iterator meta_info_iterator;

  /**
   *  @brief Constructor
   */
  WriterBase () { }

  /**
   *  @brief Destructor
   */
  virtual ~WriterBase () { }

  /**
   *  @brief Actually write the layout
   */
  virtual void write (const db::Layout &layout, tl::OutputStream &stream, const db::SaveLayoutOptions &options) = 0;

  /**
   *  @brief Returns the begin iterator for the meta information produced by the writer
   *  The writer may produce meta information which can be attached to the layout object
   *  after it has been written.
   */
  meta_info_iterator begin_meta () const
  {
    return m_meta_info.begin ();
  } 

  /**
   *  @brief Meta information end iterator
   */
  meta_info_iterator end_meta () const
  {
    return m_meta_info.end ();
  } 

protected:
  /**
   *  @brief Adds a meta info item
   */
  void add_meta_info (const MetaInfo &i)
  { 
    m_meta_info.push_back (i);
  }

  meta_info m_meta_info;
};

/**
 *  @brief A generic stream format writer
 */
class KLAYOUT_DLL Writer
{
public:
  typedef std::vector<MetaInfo> meta_info;
  typedef meta_info::const_iterator meta_info_iterator;

  /**
   *  @brief The constructor
   */
  Writer (const SaveLayoutOptions &options);

  /**
   *  @brief The destructor
   */
  ~Writer ();

  /**
   *  @brief The generic write method
   */
  void write (const db::Layout &layout, tl::OutputStream &stream);

  /**
   *  @brief True, if for this format a valid writer is provided
   */
  bool is_valid () const
  {
    return mp_writer != 0;
  }

  /**
   *  @brief Returns the begin iterator for the meta information produced by the writer
   *  The writer may produce meta information which can be attached to the layout object
   *  after it has been written.
   */
  meta_info_iterator begin_meta () const
  {
    return mp_writer->begin_meta ();
  } 

  /**
   *  @brief Meta information end iterator
   */
  meta_info_iterator end_meta () const
  {
    return mp_writer->end_meta ();
  } 

private:
  WriterBase *mp_writer;
  db::SaveLayoutOptions m_options;
};

}

#endif

