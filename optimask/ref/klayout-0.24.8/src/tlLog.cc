
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


#include <stdio.h>

#include "tlLog.h"

namespace tl
{

// ------------------------------------------------
//  Verbosity implementation

static int m_verbosity_level = 0;

void 
verbosity (int level)
{
  m_verbosity_level = level;
}

int 
verbosity ()
{
  return m_verbosity_level;
}

// ------------------------------------------------
//  Channel implementation

Channel::Channel () 
  : m_lock (), m_no_endl (false), m_active (false)
{
  //  .. nothing else ..
}

Channel::~Channel ()
{
  // .. nothing yet ..
}

ChannelProxy 
Channel::issue_proxy ()
{
  m_no_endl = false;
  m_lock.lock ();
  if (! m_active) {
    begin ();
    m_active = true;
  }
  return ChannelProxy (this);
}

void 
Channel::release_proxy ()
{
  if (! m_no_endl) {
    endl (); //  this helps remembering that there is something implicit going on here ..
    end ();
    m_active = false;
  }
  m_no_endl = false;
  m_lock.unlock ();
}

ChannelEndl endl;
ChannelNoendl noendl;

// ------------------------------------------------
//  LogTee implementation

LogTee::LogTee ()
{
  //  .. nothing yet ..
}

LogTee::LogTee (Channel *first, bool owned)
{
  add (first, owned);
}

void 
LogTee::add (Channel *other, bool owned)
{
  m_lock.lock ();
  m_channels.insert (other, owned);
  m_lock.unlock ();
}

void 
LogTee::puts (const char *s)
{
  for (tl::Collection<tl::Channel>::iterator c = m_channels.begin (); c != m_channels.end (); ++c) {
    c->puts (s);
  }
}

void 
LogTee::endl ()
{
  for (tl::Collection<tl::Channel>::iterator c = m_channels.begin (); c != m_channels.end (); ++c) {
    c->endl ();
  }
}

void 
LogTee::end ()
{
  for (tl::Collection<tl::Channel>::iterator c = m_channels.begin (); c != m_channels.end (); ++c) {
    c->end ();
  }
}

void 
LogTee::begin ()
{
  for (tl::Collection<tl::Channel>::iterator c = m_channels.begin (); c != m_channels.end (); ++c) {
    c->begin ();
  }
}

// ------------------------------------------------
//  ChannelProxy implementation

ChannelProxy::ChannelProxy (Channel *channel)
  : mp_channel (channel)
{
}

ChannelProxy::~ChannelProxy ()
{
  if (mp_channel) {
    mp_channel->release_proxy ();
    mp_channel = 0;
  }
}

ChannelProxy &
ChannelProxy::operator= (const ChannelProxy &d)
{
  if (mp_channel != d.mp_channel) {
    mp_channel = d.mp_channel;
    // transfer channel to the assigned object
    (const_cast<ChannelProxy &> (d)).mp_channel = 0;
  }
  return *this;
}

ChannelProxy::ChannelProxy (const ChannelProxy &d)
  : mp_channel (d.mp_channel)
{
  // transfer channel to the assigned object
  (const_cast<ChannelProxy &> (d)).mp_channel = 0;
}

// ------------------------------------------------
//  InfoChannel implementation

/**
 *  @brief A special implementation for information messages to stdout
 */

class InfoChannel : public Channel
{
public:
  InfoChannel (int verbosity);
  ~InfoChannel ();

protected:
  virtual void puts (const char *s);
  virtual void endl ();
  virtual void end ();
  virtual void begin ();

private:
  int m_verbosity;
};

InfoChannel::InfoChannel (int verbosity)
  : m_verbosity (verbosity)
{
  //  .. nothing yet ..
}

InfoChannel::~InfoChannel ()
{
  //  .. nothing yet ..
}

void 
InfoChannel::puts (const char *s)
{
  if (verbosity () >= m_verbosity) {
    fprintf (stdout, "%s", s);
  }
}

void 
InfoChannel::endl ()
{
  if (verbosity () >= m_verbosity) {
    fprintf (stdout, "\n");
  }
}

void 
InfoChannel::end ()
{
  if (verbosity () >= m_verbosity) {
    fflush (stdout);
  }
}

void 
InfoChannel::begin ()
{
  //  .. nothing yet ..
}

// ------------------------------------------------
//  WarningChannel implementation

/**
 *  @brief A special implementation for warnings to stdout
 */

class WarningChannel : public Channel
{
public:
  WarningChannel ();
  ~WarningChannel ();

protected:
  virtual void puts (const char *s);
  virtual void endl ();
  virtual void end ();
  virtual void begin ();
};

WarningChannel::WarningChannel ()
{
  //  .. nothing yet ..
}

WarningChannel::~WarningChannel ()
{
  //  .. nothing yet ..
}

void 
WarningChannel::puts (const char *s)
{
  fprintf (stdout, "%s", s);
}

void 
WarningChannel::endl ()
{
  fprintf (stdout, "\n");
}

void 
WarningChannel::end ()
{
  fflush (stdout);
}

void 
WarningChannel::begin ()
{
  fprintf (stdout, "Warning: ");
}


// ------------------------------------------------
//  ErrorChannel implementation

/**
 *  @brief A special implementation for errors to stderr
 */

class ErrorChannel : public Channel
{
public:
  ErrorChannel ();
  ~ErrorChannel ();

protected:
  virtual void puts (const char *s);
  virtual void endl ();
  virtual void end ();
  virtual void begin ();
};

ErrorChannel::ErrorChannel ()
{
  //  .. nothing yet ..
}

ErrorChannel::~ErrorChannel ()
{
  //  .. nothing yet ..
}

void 
ErrorChannel::puts (const char *s)
{
  fprintf (stderr, "%s", s);
}

void 
ErrorChannel::endl ()
{
  fprintf (stderr, "\n");
}

void 
ErrorChannel::end ()
{
  fflush (stderr);
}

void 
ErrorChannel::begin ()
{
  fprintf (stderr, "*** ERROR: ");
}

// ------------------------------------------------
//  The instances of the log channels

KLAYOUT_DLL LogTee warn (new WarningChannel (), true);
KLAYOUT_DLL LogTee info (new InfoChannel (0), true);
KLAYOUT_DLL LogTee log (new InfoChannel (10), true);
KLAYOUT_DLL LogTee error (new ErrorChannel (), true);

} // namespace tl

