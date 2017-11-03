#ifndef EXTRACTOR
#define EXTRACTOR

class Extractor
{
public:
  struct end { };

  Extractor (const char *s = "");

  virtual ~Extractor () { }

  Extractor &read (unsigned int &value);
//  Extractor &read (unsigned long &value);
//  Extractor &read (unsigned long long &value);
//  Extractor &read (double &value);
//  Extractor &read (int &value);
//  Extractor &read (long &value);
//  Extractor &read (long long &value);
//  Extractor &read (bool &value);
//  template <class T>
//  Extractor &read (T &value)
//  {
//    tl::extractor_impl (*this, value);
//    return *this;
//  }
//  Extractor &read (std::string &value, const char *term = "");
//  Extractor &read_word (std::string &value, const char *non_term = "_.$");
//  Extractor &read_quoted (std::string &value);
//  Extractor &read_word_or_quoted (std::string &value, const char *non_term = "_.$");


  bool try_read (unsigned int &value);
//  bool try_read (int &value);
//  bool try_read (unsigned long &value);
//  bool try_read (unsigned long long &value);
//  bool try_read (long &value);
//  bool try_read (long long &value);
//  bool try_read (double &value);
//  bool try_read (bool &value);
//  bool try_read (std::string &string, const char *term = "");
//  bool try_read_word (std::string &value, const char *non_term = "_.$");
//  bool try_read_quoted (std::string &value);
//  bool try_read_word_or_quoted (std::string &value, const char *non_term = "_.$");
//  template <class T>
//  bool try_read (T &value)
//  {
//    return tl::test_extractor_impl (*this, value);
//  }


  Extractor &expect (const char *token);
  Extractor &expect_end ();
  Extractor &expect_more ();

  bool test (const char *token);
  const char *skip ();


  char operator* () const
  {
    return *m_cp;
  }
  const char *get () const
  {
    return m_cp;
  }

  Extractor &operator++ ()
  {
    ++m_cp;
    return *this;
  }

  bool at_end ()
  {
    return *skip () == 0;
  }

//  virtual void error (const std::string &msg);
  template <class X>
  Extractor &operator>> (X &x)
  {
    return read (x);
  }
  Extractor &operator>> (const char *token)
  {
    return expect (token);
  }
  Extractor &operator>> (end)
  {
    return expect_end ();
  }

private:
  const char *m_cp;
};


#endif // EXTRACTOR

