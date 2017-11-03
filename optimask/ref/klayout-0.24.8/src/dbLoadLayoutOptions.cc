
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


#include "dbLoadLayoutOptions.h"
#include "dbStream.h"
#include "tlClassRegistry.h"
#include "tlStream.h"

namespace db
{
  LoadLayoutOptions::LoadLayoutOptions ()
    : m_create_other_layers (true),
      m_enable_text_objects (true),
      m_enable_properties (true)
  {
    // .. nothing yet ..
  }

  LoadLayoutOptions::LoadLayoutOptions (const LoadLayoutOptions &d)
    : m_create_other_layers (true),
      m_enable_text_objects (true),
      m_enable_properties (true)
  {
    operator= (d);
  }

  LoadLayoutOptions &
  LoadLayoutOptions::operator= (const LoadLayoutOptions &d)
  {
    if (&d != this) {

      m_format = d.m_format;
      m_layer_map = d.m_layer_map;
      m_create_other_layers = d.m_create_other_layers;
      m_enable_text_objects = d.m_enable_text_objects;
      m_enable_properties = d.m_enable_properties;

      release ();
      for (std::map <std::string, const FormatSpecificReaderOptions *>::const_iterator o = d.m_options.begin (); o != d.m_options.end (); ++o) {
        m_options.insert (std::make_pair (o->first, o->second->clone ()));
      }

    }
    return *this;
  }

  LoadLayoutOptions::~LoadLayoutOptions ()
  {
    release (); 
  }

  void 
  LoadLayoutOptions::release ()
  {
    for (std::map <std::string, const FormatSpecificReaderOptions *>::const_iterator o = m_options.begin (); o != m_options.end (); ++o) {
      delete (const_cast<FormatSpecificReaderOptions *> (o->second));
    }
    m_options.clear ();
  }

  bool 
  LoadLayoutOptions::set_format_from_filename (const std::string &fn)
  {
    for (tl::Registrar<db::StreamFormatDeclaration>::iterator fmt = tl::Registrar<db::StreamFormatDeclaration>::begin (); fmt != tl::Registrar<db::StreamFormatDeclaration>::end (); ++fmt) {
      if (tl::match_filename_to_format (fn, fmt->file_format ())) {
        m_format = fmt->format_name ();
        return true;
      }
    }
    return false;
  }

  void 
  LoadLayoutOptions::set_specific_options (const FormatSpecificReaderOptions &options, const std::string &format)
  {
    set_specific_options (options.clone (), format);
  }

  void 
  LoadLayoutOptions::set_specific_options (const FormatSpecificReaderOptions *options, const std::string &format)
  {
    std::map <std::string, const FormatSpecificReaderOptions *>::iterator o = m_options.find (format);
    if (o != m_options.end ()) {
      delete const_cast<FormatSpecificReaderOptions *> (o->second);
      m_options.erase (o);
    }

    m_options.insert (std::make_pair (format, options));
  }

  const FormatSpecificReaderOptions *
  LoadLayoutOptions::get_specific_options (const std::string &format) const
  {
    std::map <std::string, const FormatSpecificReaderOptions *>::const_iterator o = m_options.find (format);
    if (o != m_options.end ()) {
      return o->second;
    } else {
      return 0;
    }
  }
}

