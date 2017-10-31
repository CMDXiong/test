#include "./Layer/extractor.h"
#include <ctype.h>

Extractor::Extractor (const char *s)
  : m_cp (s)
{
  //  .. nothing yet ..
}

Extractor &
Extractor::read (unsigned int &value)
{
  if (! try_read (value)) {
    //error (tl::to_string (QObject::tr ("Expected an unsigned integer value")));
  }
  return *this;
}

bool
Extractor::try_read (unsigned int &value)
{
  if (! *skip ()) {
    return false;
  }

  if (! isdigit (*m_cp)) {
    return false;
  }

  value = 0;
  while (isdigit (*m_cp)) {
    if ((value * 10) / 10 != value) {
      //throw Exception (tl::to_string (QObject::tr ("Range overflow on unsigned integer")));
    }
    value *= 10;
    value += (*m_cp - '0');
    ++m_cp;
  }

  return true;
}

Extractor &
Extractor::expect_end ()
{
  if (! at_end ()) {
    //error (tl::to_string (QObject::tr ("Expected end of text")));
  }
  return *this;
}

Extractor &
Extractor::expect_more ()
{
  if (at_end ()) {
    //error (tl::to_string (QObject::tr ("Expected more text")));
  }
  return *this;
}

Extractor &
Extractor::expect (const char *token)
{
  if (! test (token)) {
    //error (tl::sprintf (tl::to_string (QObject::tr ("Expected '%s'")).c_str (), token));
  }
  return *this;
}

bool
Extractor::test (const char *token)
{
  skip ();

  const char *cp = m_cp;
  while (*cp && *token) {
    if (*cp != *token) {
      return false;
    }
    ++cp;
    ++token;
  }

  if (! *token) {
    m_cp = cp;
    return true;
  } else {
    return false;
  }
}

const char *
Extractor::skip ()
{
  while (isspace (*m_cp) && *m_cp) {
    ++m_cp;
  }
  return m_cp;
}

//void
//Extractor::error (const std::string &msg)
//{
//  std::string m (msg);

//  if (at_end ()) {
//    m += tl::to_string (QObject::tr (", but text ended"));
//  } else {
//    m += tl::to_string (QObject::tr (" here: "));
//    const char *cp = m_cp;
//    for (unsigned int i = 0; i < 10 && *cp; ++i, ++cp) {
//      m += *cp;
//    }
//    if (*cp) {
//      m += " ..";
//    }
//  }

//  throw tl::Exception (m);
//}

