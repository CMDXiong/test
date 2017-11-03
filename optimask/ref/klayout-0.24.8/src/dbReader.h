
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



#ifndef HDR_dbReader
#define HDR_dbReader

#include "config.h"

#include "tlException.h"
#include "tlInternational.h"
#include "tlString.h"

#include "tlStream.h"
#include "dbLoadLayoutOptions.h"
#include "dbMetaInfo.h"

#include <vector>

namespace db
{

class Layout;
class ReaderBase;

/**
 *  @brief Generic base class of reader exceptions
 */
class KLAYOUT_DLL ReaderException
  : public tl::Exception 
{
public:
  ReaderException (const std::string &msg)
    : tl::Exception (msg)
  { }
};

/**
 *  @brief The generic reader base class
 */
class KLAYOUT_DLL ReaderBase
{
public:
  ReaderBase ();
  virtual ~ReaderBase ();

  typedef std::vector<MetaInfo> meta_info;
  typedef meta_info::const_iterator meta_info_iterator;

  virtual const db::LayerMap &read (db::Layout &layout, const db::LoadLayoutOptions &options) = 0;
  virtual const db::LayerMap &read (db::Layout &layout) = 0;
  virtual const char *format () const = 0;

  meta_info_iterator begin_meta () const
  {
    return m_meta_info.begin ();
  } 

  meta_info_iterator end_meta () const
  {
    return m_meta_info.end ();
  } 

  /**
   *  @brief Sets a flag indicating that warnings shall be treated as errors
   *  If this flag is set, warnings will be handled as errors.
   */
  void set_warnings_as_errors (bool f);

  /**
   *  @brief Gets a flag indicating that warnings shall be treated as errors
   */
  bool warnings_as_errors () const
  {
    return m_warnings_as_errors;
  }

protected:
  void add_meta_info (const MetaInfo &i)
  { 
    m_meta_info.push_back (i);
  }

private:
  meta_info m_meta_info;
  bool m_warnings_as_errors;
};

/**
 *  @brief The generic stream reader 
 *
 *  This reader is supposed to fork to one of the specific readers
 *  depending on the format detected.
 */
class KLAYOUT_DLL Reader
{
public: 
  typedef ReaderBase::meta_info meta_info;
  typedef ReaderBase::meta_info_iterator meta_info_iterator;

  /**
   *  @brief Construct a reader object
   *
   *  If no valid format can be detected, the constructor will throw 
   *  an exception. The stream must be opened already in order to allow
   *  format detection.
   *
   *  @param s The stream object from which to read stream data from
   */
  Reader (tl::InputStream &s);

  /**  
   *  @brief Destructor
   */
  ~Reader ();

  /** 
   *  @brief The basic read method 
   *
   *  This method will read the stream data and translate this to
   *  insert calls into the layout object. This will not do much
   *  on the layout object beside inserting the objects.
   *  It can be passed options with a layer map which tells which
   *  OASIS layer(s) to read on which logical layers.
   *  In addition, a flag can be passed that tells wheter to create 
   *  new layers. The returned map will contain all layers, the passed
   *  ones and the newly created ones.
   *
   *  @param layout The layout object to write to
   *  @param options The LayerMap object
   */
  const db::LayerMap &read (db::Layout &layout, const db::LoadLayoutOptions &options) 
  {
    return mp_actual_reader->read (layout, options);
  }

  /** 
   *  @brief The basic read method (without mapping)
   *
   *  This method will read the stream data and translate this to
   *  insert calls into the layout object. This will not do much
   *  on the layout object beside inserting the objects.
   *  This version will read all input layers and return a map
   *  which tells which OASIS layer has been read into which logical
   *  layer.
   *
   *  @param layout The layout object to write to
   *  @return The LayerMap object
   */
  const db::LayerMap &read (db::Layout &layout)
  {
    return mp_actual_reader->read (layout);
  }

  /**
   *  @brief Returns a format describing the file format found
   */
  const char *format () const
  {
    return mp_actual_reader->format ();
  }

  /**
   *  @brief Returns a "begin" iterator for the meta info
   */
  meta_info_iterator begin_meta () const
  {
    return mp_actual_reader->begin_meta ();
  } 

  /**
   *  @brief Returns a "end" iterator for the meta info 
   */
  meta_info_iterator end_meta () const
  {
    return mp_actual_reader->end_meta ();
  } 

  /**
   *  @brief Sets a flag indicating that warnings shall be treated as errors
   *  If this flag is set, warnings will be handled as errors.
   */
  void set_warnings_as_errors (bool f)
  {
    mp_actual_reader->set_warnings_as_errors (f);
  }

  /**
   *  @brief Gets a flag indicating that warnings shall be treated as errors
   */
  bool warnings_as_errors () const
  {
    return mp_actual_reader->warnings_as_errors ();
  }

private:
  ReaderBase *mp_actual_reader;
  tl::InputStream &m_stream;
};

}

#endif

