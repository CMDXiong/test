#ifndef LAYSTIPPLEPALETTE_H
#define LAYSTIPPLEPALETTE_H
#include <vector>
#include <string>

class  StipplePalette
{
public:
 /*
  *  @brief Default constructor
  *  This initializes the palette with the standard stipples.
  */
  StipplePalette ();
  /*
   *  @brief Constructor from the data
   *
   *  @param stipples The stipples as a vector
   *  @param standard The list of standard stipple indices as a vector
   */
  StipplePalette (const std::vector<unsigned int> &stipples, const std::vector<unsigned int> &standard);
  // @brief Copy constructor
  StipplePalette (const StipplePalette &d);
  // @brief Assignment operator
  StipplePalette operator= (const StipplePalette &d);
  //@brief Equality operator
  bool operator== (const StipplePalette &d) const;
  //@brief Inequality operator
  bool operator!= (const StipplePalette &d) const
  {
    return ! operator== (d);
  }
  //@brief Change a specific stipple
  void set_stipple (unsigned int n, unsigned int s);
  // @brief Clear the stipple list
  void clear_stipples ();
  // @brief Set a standard stipple index
  void set_standard_stipple_index (unsigned int n, unsigned int si);
  // @brief Clear the standard stipple list
  void clear_standard_stipples ();
  // @brief Retrieve the stipple by index
  unsigned int stipple_by_index (unsigned int n) const;
  /*
   * @brief Retrieve the number of stipples in the palette
   * Warning: it is not guaranteed that this number is non-zero.
   */
  unsigned int stipples () const;
  //@brief Retrieve the standard stipple by index
  unsigned int
  standard_stipple_by_index (unsigned int n) const
  {
    return stipple_by_index (standard_stipple_index_by_index (n));
  }
   /*
    *  @brief Retrieve the standard stipple index by index (0 to standard_stipples()-1)
    *
    *  The index returned is the index of the stipple referenced. The actual stipple
    *  can be obtained with stipple_by_index().
    */
  unsigned int standard_stipple_index_by_index (unsigned int n) const;
    /*
     *  @brief Retrieve the number of standard stipples in the palette
     *
     *  The standard stipples are used for automatically selecting stipples for the
     *  layers for example. They are accessible by stipple index 0..st-1,
     *  where st is the number returned by this functions.
     *  Warning: it is not guaranteed that this number is non-zero.
     */
  unsigned int standard_stipples () const;
  // @brief Conversion to a string
  //std::string to_string () const;
  /*
    *  @brief Conversion from a string
    *
    *  This method will throw an exception if the string does not have a valid format
    *  like the one returned by the to_string method.
    */
  void from_string (const char *&s);
  // @brief Deliver the default palette
  static StipplePalette default_palette ();

private:
  std::vector <unsigned int> m_stipples;
  std::vector <unsigned int> m_standard;

};

#endif // LAYSTIPPLEPALETTE_H

