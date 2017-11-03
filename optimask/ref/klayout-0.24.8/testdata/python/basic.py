
import pya
import unittest
import os
import sys
import gc

# Set this to true to disable some tests involving exceptions
leak_check = "TEST_LEAK_CHECK" in os.environ

# see test_21
class AEXT(pya.PA):
  def __init__(self):
    self.offset = None
  def s(self, o):
    self.offset = o
  def g(self):
    return self.offset
  def m(self):
    return self.offset+self.a1()
  # Note: there are no protected methods, but this emulates the respective test for RBA
  def call_a10_prot(self, f):
    a10_prot(f)

class XEdge(pya.Edge):
  def __init__(self):
    super(XEdge, self).__init__(pya.Point(1,2), pya.Point(3,4))

class EEXT(pya.PE):
  def func(self, x):
    self.m = x.n
  def xfunc(self, x):
    return len(x)
  def __init__(self):
    self.m = None
    self.n = 42

class X(object):
  nothing = False

class C_IMP1(pya.PC):
  def f(self, s):
    return 615

class C_IMP2(pya.PC):
  def f(self, s):
    return len(s)

class C_IMP3(pya.PC):
  anything = None

class Z_IMP1(pya.PZ):
  def f(self, x):
    return x.cls_name()

class Z_IMP2(pya.PZ):
  def f(self, x):
    return type(x).__name__

class Z_IMP3(pya.PZ):
  def f(self, x):
    return super(Z_IMP3, self).f(x) + "*"

class BasicTest(unittest.TestCase):

  def test_00(self):

    # all references of PA are released now:
    ac0 = pya.PA.a0()

    a = pya.PA.new_a(100)
    self.assertEqual( pya.PA.a0(), ac0 + 1 )

    a = None
    self.assertEqual( pya.PA.a0(), ac0 )

    a = pya.PA()
    self.assertEqual( pya.PA.a0(), ac0 )  # delayed instantiation of detached objects - A is actually created if it is used first
    a.a2()   # just check, if it can be called
    self.assertEqual( pya.PA.a0(), ac0 + 1 )

    # open question: with ruby 1.8, aa is not deleted if the self.assertEqual is missing. Why?
    # maybe the GC does not like to be called that frequently?
    aa = a.dup()
    self.assertEqual( pya.PA.a0(), ac0 + 2 )

    aa = None
    self.assertEqual( pya.PA.a0(), ac0 + 1 )

    a = None
    self.assertEqual( pya.PA.a0(), ac0 )

    a = pya.PA()
    self.assertEqual( pya.PA.a0(), ac0 )  # delayed instantiation of detached objects - A is actually created if it is used first
    a.a2()   # just check, if it can be called
    self.assertEqual( pya.PA.a0(), ac0 + 1 )

    # static and non-static methods can be mixed, but they will be made non-ambiguous too
    self.assertEqual( pya.PA.aa(), "static_a" )
    self.assertEqual( a.aa(), "a" )

    self.assertEqual( a.a1(), 17 )
    a.a5(-5)
    self.assertEqual( a.a1(), -5 )
    a.a5(0x7fffffff)
    self.assertEqual( a.a1(), 0x7fffffff )
    a.a5(-0x80000000)
    self.assertEqual( a.a1(), -0x80000000 )

    self.assertEqual( a.a3("a"), 1 )
    self.assertEqual( a.a3("ab"), 2 )
    self.assertEqual( a.a3(""), 0 )

    self.assertEqual( a.a4([1]), 1.0 )
    t = (1,)
    self.assertEqual( a.a4(t), 1.0 )
    self.assertEqual( a.a4([1, 125e-3]), 0.125 )
    t = (5, 1, -1.25)
    self.assertEqual( a.a4(t), -1.25 )

    arr = []
    for d in a.a6():
      arr.append(d)
    self.assertEqual(arr, [5, 1, -1.25])

    arr = []
    for d in a.a7():
      arr.append(d)
    self.assertEqual(arr, [5, 1, -1.25])

    arr = []
    for d in a.a8():
      arr.append(d)
    self.assertEqual(arr, [5, 1, -1.25])

    a.destroy()
    self.assertEqual( pya.PA.a0(), ac0 )

    if not leak_check:

      error_caught = False
      try: 
        a.a2()  # object has been destroyed already
      except:
        error_caught = True
      self.assertEqual( error_caught, True )

      error_caught = False
      try: 
        a.destroy()  # object has been destroyed already
      except:
        error_caught = True
      self.assertEqual( error_caught, True )

    self.assertEqual( pya.PA.a0(), ac0 )
    a = pya.PA.new_a( 55 )
    self.assertEqual( pya.PA.a0(), ac0 + 1 )
    self.assertEqual( a.a1(), 55 )
    self.assertEqual( a.a_vp1( a.a_vp2() ), "abc" )
    a.destroy()
    self.assertEqual( pya.PA.a0(), ac0 )

    a = pya.PA.new_a(0)
    self.assertEqual( str(a.a9a(5)), "True" )
    self.assertEqual( str(a.a9a(4)), "False" )
    self.assertEqual( str(a.a9b(True)), "5" )
    self.assertEqual( str(a.a9b(0)), "-5" )
    self.assertEqual( str(a.a9b(1)), "5" )
    self.assertEqual( str(a.a9b(False)), "-5" )
    self.assertEqual( str(a.a9b(None)), "-5" )

  def test_12(self):

    a1 = pya.PA()
    a1.a5( -15 )
    a2 = a1
    a3 = a2.dup()

    self.assertEqual( a1.a1(), -15 )
    self.assertEqual( a2.a1(), -15 )
    self.assertEqual( a3.a1(), -15 )

    a1.a5( 11 )
    a3.a5( -11 )
    
    self.assertEqual( a1.a1(), 11 )
    self.assertEqual( a2.a1(), 11 )
    self.assertEqual( a3.a1(), -11 )

    self.assertEqual( a1.a10_d(5.2), "5.2" )
    self.assertEqual( a1.a10_f(5.7), "5.7" )
    x = pya.Value(1.5)
    self.assertEqual( str(x.value), "1.5" )
    self.assertEqual( a1.a10_fptr(x), "6.5" )
    self.assertEqual( str(x.value), "6.5" )
    self.assertEqual( a1.a10_fptr(1), "6" )
    self.assertEqual( a1.a10_fptr(None), "nil" )
    self.assertEqual( a1.a10_fptr(pya.Value()), "nil" )
    self.assertEqual( str(x), "6.5" )
    self.assertEqual( str(x.value), "6.5" )
    x = pya.Value(2.5)
    self.assertEqual( a1.a10_dptr(x), "8.5" )
    self.assertEqual( a1.a10_dptr(2), "8" )
    self.assertEqual( a1.a10_dptr(None), "nil" )
    self.assertEqual( a1.a10_dptr(pya.Value()), "nil" )
    self.assertEqual( x.to_s(), "8.5" )
    self.assertEqual( str(x.value), "8.5" )
    x = pya.Value(2)
    self.assertEqual( a1.a10_iptr(x), "9" )
    self.assertEqual( a1.a10_iptr(3), "10" )
    self.assertEqual( a1.a10_iptr(None), "nil" )
    self.assertEqual( a1.a10_iptr(pya.Value()), "nil" )
    self.assertEqual( str(x), "9" )
    self.assertEqual( str(x.value), "9" )
    x = pya.Value(False)
    self.assertEqual( a1.a10_bptr(x), "true" )
    self.assertEqual( a1.a10_bptr(False), "true" )
    self.assertEqual( a1.a10_bptr(None), "nil" )
    self.assertEqual( a1.a10_bptr(pya.Value()), "nil" )
    self.assertEqual( str(x), "true" )
    self.assertEqual( str(x.value), "True" )
    x = pya.Value(10)
    self.assertEqual( a1.a10_uiptr(x), "20" )
    self.assertEqual( a1.a10_uiptr(11), "21" )
    self.assertEqual( a1.a10_uiptr(None), "nil" )
    self.assertEqual( a1.a10_uiptr(pya.Value()), "nil" )
    self.assertEqual( str(x), "20" )
    self.assertEqual( str(x.value), "20" )
    x = pya.Value(10)
    self.assertEqual( a1.a10_ulptr(x), "21" )
    self.assertEqual( a1.a10_ulptr(12), "23" )
    self.assertEqual( a1.a10_ulptr(None), "nil" )
    self.assertEqual( a1.a10_ulptr(pya.Value()), "nil" )
    self.assertEqual( str(x), "21" )
    self.assertEqual( str(x.value), "21" )
    x = pya.Value(10)
    self.assertEqual( a1.a10_lptr(x), "22" )
    self.assertEqual( a1.a10_lptr(11), "23" )
    self.assertEqual( a1.a10_lptr(None), "nil" )
    self.assertEqual( a1.a10_lptr(pya.Value()), "nil" )
    self.assertEqual( str(x), "22" )
    self.assertEqual( str(x.value), "22" )
    x = pya.Value(10)
    self.assertEqual( a1.a10_llptr(x), "23" )
    self.assertEqual( a1.a10_llptr(11), "24" )
    self.assertEqual( a1.a10_llptr(None), "nil" )
    self.assertEqual( a1.a10_llptr(pya.Value()), "nil" )
    self.assertEqual( str(x), "23" )
    self.assertEqual( str(x.value), "23" )
    x = pya.Value(10)
    self.assertEqual( a1.a10_ullptr(x), "24" )
    self.assertEqual( a1.a10_ullptr(12), "26" )
    self.assertEqual( a1.a10_ullptr(None), "nil" )
    self.assertEqual( a1.a10_ullptr(pya.Value()), "nil" )
    self.assertEqual( x.to_s(), "24" )
    self.assertEqual( x.value, 24 )
    x = pya.Value("z")
    self.assertEqual( a1.a10_sptr(x), "zx" )
    self.assertEqual( a1.a10_sptr("a"), "ax" )
    self.assertEqual( a1.a10_sptr(None), "nil" )
    self.assertEqual( a1.a10_sptr(pya.Value()), "nil" )
    self.assertEqual( x.to_s(), "zx" )
    self.assertEqual( x.value, "zx" )
    try:
      # passing other objects than StringValue and a string fails
      self.assertEqual( a1.a10_sptr([]), "nil" )
      err = False
    except: 
      err = True
    self.assertEqual( err, True )

    self.assertEqual( a1.a10_cfptr(6.5), "6.5" )
    self.assertEqual( a1.a10_cfptr(None), "nil" )
    self.assertEqual( a1.a10_cdptr(8.5), "8.5" )
    self.assertEqual( a1.a10_cdptr(None), "nil" )
    self.assertEqual( a1.a10_ciptr(9), "9" )
    self.assertEqual( a1.a10_ciptr(None), "nil" )
    self.assertEqual( a1.a10_cbptr(True), "true" )
    self.assertEqual( a1.a10_cbptr(None), "nil" )
    self.assertEqual( a1.a10_cuiptr(20), "20" )
    self.assertEqual( a1.a10_cuiptr(None), "nil" )
    self.assertEqual( a1.a10_culptr(21), "21" )
    self.assertEqual( a1.a10_culptr(None), "nil" )
    self.assertEqual( a1.a10_clptr(22), "22" )
    self.assertEqual( a1.a10_clptr(None), "nil" )
    self.assertEqual( a1.a10_cllptr(23), "23" )
    self.assertEqual( a1.a10_cllptr(None), "nil" )
    self.assertEqual( a1.a10_cullptr(24), "24" )
    self.assertEqual( a1.a10_cullptr(None), "nil" )
    self.assertEqual( a1.a10_csptr(None), "nil" )
    self.assertEqual( a1.a10_csptr("x"), "x" )

    x = pya.Value(1.5)
    self.assertEqual( a1.a10_fref(x), "11.5" )
    try:
      self.assertEqual( a1.a10_fref(None), "nil" )
      err = False
    except: 
      err = True
    self.assertEqual( err, True )
    try:
      self.assertEqual( a1.a10_fref(pya.Value()), "nil" )
      err = False
    except: 
      err = True
    self.assertEqual( err, True )
    self.assertEqual( x.value, 11.5 )
    x = pya.Value(2.5)
    self.assertEqual( a1.a10_dref(x), "13.5" )
    self.assertEqual( a1.a10_dref(2), "13" )
    self.assertEqual( x.value, 13.5 )
    x = pya.Value(2)
    self.assertEqual( a1.a10_iref(x), "14" )
    self.assertEqual( a1.a10_iref(1), "13" )
    self.assertEqual( x.value, 14 )
    x = pya.Value(False)
    self.assertEqual( a1.a10_bref(x), "true" )
    self.assertEqual( a1.a10_bref(False), "true" )
    self.assertEqual( x.value, True )
    x = pya.Value(10)
    self.assertEqual( a1.a10_uiref(x), "24" )
    self.assertEqual( a1.a10_uiref(11), "25" )
    self.assertEqual( x.value, 24 )
    x = pya.Value(10)
    self.assertEqual( a1.a10_ulref(x), "25" )
    self.assertEqual( a1.a10_ulref(12), "27" )
    self.assertEqual( x.value, 25 )
    x = pya.Value(10)
    self.assertEqual( a1.a10_lref(x), "26" )
    self.assertEqual( a1.a10_lref(13), "29" )
    self.assertEqual( x.value, 26 )
    x = pya.Value(10)
    self.assertEqual( a1.a10_llref(x), "27" )
    self.assertEqual( a1.a10_llref(14), "31" )
    self.assertEqual( x.value, 27 )
    x = pya.Value(10)
    self.assertEqual( a1.a10_ullref(x), "28" )
    self.assertEqual( a1.a10_ullref(11), "29" )
    self.assertEqual( x.value, 28 )
    x = pya.Value("x")
    self.assertEqual( a1.a10_sref(x), "xy" )
    self.assertEqual( a1.a10_sref("p"), "py" )
    self.assertEqual( x.value, "xy" )

    self.assertEqual( a1.a10_cfref(6.5), "6.5" )
    try:
      self.assertEqual( a1.a10_cfref(None), "nil" )
      err = False
    except: 
      err = True
    self.assertEqual( err, True )
    self.assertEqual( a1.a10_cdref(8.5), "8.5" )
    self.assertEqual( a1.a10_ciref(9), "9" )
    self.assertEqual( a1.a10_cbref(True), "true" )
    self.assertEqual( a1.a10_cuiref(20), "20" )
    self.assertEqual( a1.a10_culref(21), "21" )
    self.assertEqual( a1.a10_clref(22), "22" )
    self.assertEqual( a1.a10_cllref(23), "23" )
    self.assertEqual( a1.a10_cullref(24), "24" )
    self.assertEqual( a1.a10_csref("x"), "x" )

  def test_13(self):

    b = pya.PB()

    if not leak_check:

      err_caught = False
      try:
        b.b7().a1() # cannot call non-const method on const reference
      except: 
        err_caught = True
      self.assertEqual( err_caught, True )

    b.b7().a2()

    self.assertEqual( b.b1(), 5 )
    self.assertEqual( b.b2(), "" )
    b.b5( "xyz" )
    self.assertEqual( b.b2(), "xyz" )
    self.assertEqual( b.b5a(), "xyz" )
    b.b5b( "yx", "zz" )
    self.assertEqual( b.b2(), "yxzz" )
    self.assertEqual( b.b5a(), "yxzz" )

    arr = []

    err_caught = False

    if not leak_check:

      try:
        for a in b.b10():
          arr.append(a.a1())  # b10 is a const iterator - cannot call a1 on it
      except: 
        err_caught = True
      self.assertEqual( err_caught, True )
      self.assertEqual(arr, [])

    err_caught = False

    if not leak_check:

      try:
        for a in b.b10p():
          arr.append(a.a1())  # b10p is a const iterator - cannot call a1 on it
      except: 
        err_caught = True
      self.assertEqual( err_caught, True )
      self.assertEqual(arr, [])

    arr = []
    for a in b.b10():
      arr.append(a.dup().a1()) 
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    # NOTE: the following will crash: 
    #   for a in b.dup().b10():
    #     ...
    #   because the clone created by dup() will be
    #   destroyed too early.
    bdup = b.dup()
    for a in bdup.b10():
      arr.append(a.dup().a1()) 
    self.assertEqual(arr, [100, 121, 144])
    return

    arr = []
    for a in b.b10():
      arr.append(a.a1c()) 
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    for a in b.b10p():
      arr.append(a.dup().a1())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    # Ticket #811:
    for a in b.dup().b10p():
      arr.append(a.dup().a1())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    bdup = b.dup()
    for a in bdup.b10p():
      arr.append(a.dup().a1())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    for a in b.b10p():
      arr.append(a.a1c())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    for a in b.b11():
      arr.append(a.a1())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    bdup = b.dup()
    for a in bdup.b11():
      arr.append(a.a1())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    for a in b.b12():
      arr.append(a.a1())
    self.assertEqual(arr, [7100, 7121, 7144, 7169])

    arr = []
    bdup = b.dup()
    for a in bdup.b12():
      arr.append(a.a1())
    self.assertEqual(arr, [7100, 7121, 7144, 7169])

    aarr = b.b16a()
    arr = []
    for a in aarr:
      arr.append(a.a1())
    self.assertEqual(arr, [100, 121, 144])

    aarr = b.b16b()
    arr = []
    for a in aarr:
      arr.append(a.a1())
    self.assertEqual(arr, [100, 121, 144])

    aarr = b.b16c()
    arr = []
    for a in aarr:
      arr.append(a.a1())
    self.assertEqual(arr, [100, 121, 144])

    b.b17a( [ pya.PA.new_a( 101 ), pya.PA.new_a( -122 ) ] )
    arr = []
    for a in b.b11():
      arr.append(a.a1())
    self.assertEqual(arr, [101, -122])

    b.b17a( [] )
    arr = []
    for a in b.b11():
      arr.append(a.a1())
    self.assertEqual(arr, [])

    b.b17b( [ pya.PA.new_a( 102 ), pya.PA.new_a( -123 ) ] )
    arr = []
    for a in b.b11():
      arr.append(a.a1())
    self.assertEqual(arr, [102, -123])

    b.b17c( [ pya.PA.new_a( 100 ), pya.PA.new_a( 121 ), pya.PA.new_a( 144 ) ] )
    arr = []
    for a in b.b11():
      arr.append(a.a1())
    self.assertEqual(arr, [100, 121, 144])

    if not leak_check: 

      arr = []
      try:
        for a in b.b13():
          arr.append(a.a1())
      except: 
        err_caught = True
      self.assertEqual( err_caught, True )
      self.assertEqual(arr, [])

    arr = []
    for a in b.b13():
      arr.append(a.a1c())
    self.assertEqual(arr, [-3100, -3121])

    arr = []
    bdup = b.dup()
    for a in bdup.b13():
      arr.append(a.a1c())
    self.assertEqual(arr, [-3100, -3121])

    arr = []
    for a in b.b18():
      arr.append(a.a1c())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    for a in b.b18():
      arr.append(a.a1())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    for a in b.b18b():
      arr.append(a.a1c())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    # even though b18b returns a "const A &", calling a non-const method does not work
    # since A is a managed object and is not turned into a copy.
    err_caught = False
    try:
      for a in b.b18b():
        arr.append(a.a1())
    except:
      err_caught = True
    end
    self.assertEqual(arr, [])
    self.assertEqual(err_caught, True)

    arr = []
    for a in b.b18c():
      arr.append(a.a1c())
    self.assertEqual(arr, [100, 121, 144])

    arr = []
    # this does not work since b18c delivers a "const *" which cannot be used to call a non-const
    # method on
    err_caught = False
    try: 
      for a in b.b18c():
        arr.append(a.a1())
    except:
      err_caught = True
    end
    self.assertEqual(arr, [])
    self.assertEqual(err_caught, True)

  def test_13b(self):

    b = pya.PB()

    bb = pya.PB()
    bb.b5("a")
    b.push_b(bb)

    bb = pya.PB()
    bb.b5("y")
    b.push_b(bb)

    bb = pya.PB()
    bb.b5("uu")
    b.push_b(bb)

    arr = []
    for bb in b.each_b_copy():
      arr.append(bb.b2())
    self.assertEqual(arr, ["a", "y", "uu"])

    arr = []
    for bb in b.each_b_copy():
      bb.b5(bb.b2() + "x")
      arr.append(bb.b2())
    self.assertEqual(arr, ["ax", "yx", "uux"])

    arr = []
    for bb in b.each_b_copy():
      arr.append(bb.b2())
    self.assertEqual(arr, ["a", "y", "uu"])

    arr = []
    for bb in b.each_b_cref():
      arr.append(bb.b2())
    self.assertEqual(arr, ["a", "y", "uu"])

    arr = []
    # this works, since the "const B &" will be converted to a copy
    for bb in b.each_b_cref():
      bb.b5(bb.b2() + "x")
      arr.append(bb.b2())
    self.assertEqual(arr, ["ax", "yx", "uux"])

    arr = []
    for bb in b.each_b_cref():
      arr.append(bb.b2())
    self.assertEqual(arr, ["a", "y", "uu"])

    arr = []
    for bb in b.each_b_cptr():
      arr.append(bb.b2())
    self.assertEqual(arr, ["a", "y", "uu"])

    arr = []
    # const references cannot be modified
    err_caught = False
    try:
      for bb in b.each_b_cptr():
        bb.b5(bb.b2() + "x")
        arr.append(bb.b2())
    except: 
      err_caught = True
    self.assertEqual(err_caught, True)
    self.assertEqual(arr, [])

    arr = []
    for bb in b.each_b_cptr():
      arr.append(bb.b2())
    self.assertEqual(arr, ["a", "y", "uu"])

    arr = []
    for bb in b.each_b_ref():
      arr.append(bb.b2())
    self.assertEqual(arr, ["a", "y", "uu"])

    arr = []
    for bb in b.each_b_ref():
      bb.b5(bb.b2() + "x")
      arr.append(bb.b2())
    self.assertEqual(arr, ["ax", "yx", "uux"])

    arr = []
    for bb in b.each_b_ref():
      arr.append(bb.b2())
    self.assertEqual(arr, ["ax", "yx", "uux"])

    arr = []
    for bb in b.each_b_ptr():
      arr.append(bb.b2())
    self.assertEqual(arr, ["ax", "yx", "uux"])

    arr = []
    for bb in b.each_b_ptr():
      bb.b5(bb.b2() + "x")
      arr.append(bb.b2())
    self.assertEqual(arr, ["axx", "yxx", "uuxx"])

    arr = []
    for bb in b.each_b_ptr():
      arr.append(bb.b2())
    self.assertEqual(arr, ["axx", "yxx", "uuxx"])

  def test_14(self):

    a = pya.PA()
    a.a5( 22 )

    b = pya.PB()
    self.assertEqual( b.b3( a ), 22 )
    self.assertEqual( b.b4( a ), "b4_result: 22" )
    a.a5( -6 )
    self.assertEqual( b.b3( a ), -6 )
    self.assertEqual( b.b4( a ), "b4_result: -6" )
    self.assertEqual( b.b4( pya.PA() ), "b4_result: 17" )

  def test_15(self):

    a = pya.PA_NC()
    self.assertEqual( True, isinstance(a, pya.PA) )
    a.a5( 22 )

    b = pya.PB()
    self.assertEqual( b.b3( a ), 22 )
    self.assertEqual( b.b4( a ), "b4_result: 22" )
    a.a5( -6 )
    self.assertEqual( b.b3( a ), -6 )
    self.assertEqual( b.b4( a ), "b4_result: -6" )
    self.assertEqual( b.b4( pya.PA_NC() ), "b4_result: 17" )

  def test_16(self):

    if leak_check:
      return

    # Test, if this throws an error (object of class X passed to A argument):
    err = ""
    try:
      b = pya.PB()
      self.assertEqual( b.b4( pya.PX() ), "b4_result: -6" )
      self.assertEqual( False, True )  # this must never hit
    except Exception as e:
      err = str(e)
    self.assertEqual( err, "Unexpected object type (expected argument of class PA, got PX) in PB.b4" );
  
    # Test, if this throws an error (object of class X passed to A argument):
    err = ""
    try:
      b = pya.PB()
      bb = pya.PB()
      self.assertEqual( b.b4( bb ), "b4_result: -6" )
      self.assertEqual( False, True )  # this must never hit
    except Exception as e:
      err = str(e)
    self.assertEqual( err, "Unexpected object type (expected argument of class PA, got PB) in PB.b4" );
  
  def test_17(self):

    # test copies of objects being returned

    b = pya.PB()

    a = b.b6( 1971 );
    self.assertEqual( a.a1(), 1971 );
    self.assertEqual( b.b9( a ), 1971 );

    aa = b.b6( -61 );
    self.assertEqual( b.b9cref( aa ), -61 );
    self.assertEqual( a.a1(), 1971 );
    self.assertEqual( b.b9( a ), 1971 );
    self.assertEqual( aa.a1(), -61 );
    self.assertEqual( b.b9( aa ), -61 );

    aa.a5(98);
    a.a5(100);
    
    self.assertEqual( a.a1(), 100 );
    self.assertEqual( b.b9( a ), 100 );
    self.assertEqual( aa.a1(), 98 );
    self.assertEqual( b.b9( aa ), 98 );

  def test_18(self):

    # Test references to objects (returned by b.b7)

    b = pya.PB()
    b.b8( 77 )
    self.assertEqual( b.b7().a1c(), 77 );

    b.b8cref( 79 )
    self.assertEqual( b.b7().a1c(), 79 );

    aref = b.b7()
    err_caught = False

    if not leak_check:

      try:
        x = aref.a1() # cannot call non-const method on const reference (as delivered by b7)
      except:
        err_caught = True
      self.assertEqual( err_caught, True )
      self.assertEqual( aref.a1c(), 79 );

    b.b8( -1 )
    self.assertEqual( aref.a1c(), -1 );

  def test_19(self):

    c0 = pya.PC()
    self.assertEqual( c0.g("x"), 1977 );

    c1 = C_IMP1()
    self.assertEqual( c1.g("x"), 615 );

    c2 = C_IMP2()
    self.assertEqual( c2.g("x"), 1 );
    self.assertEqual( c2.g(""), 0 );
    self.assertEqual( c2.g("abc"), 3 );
    self.assertEqual( c1.g("x"), 615 );

    c3 = C_IMP3()
    self.assertEqual( c3.g("x"), 1977 );

    self.assertEqual( pya.PC.s1(), 4451 );
    pya.PC.s2clr()
    pya.PC.s2( 7.0 )
    self.assertEqual( pya.PC.s3( 5.5 ), "5.500" );

    arr = []
    for i in pya.PC.each():
      arr.append(i) 
    self.assertEqual( arr, [ 0, 1, 2, 3, 4, 5, 6 ] )

    self.assertEqual( C_IMP1.s1(), 4451 );
    C_IMP1.s2( 1.0 )
    self.assertEqual( C_IMP1.s3( 1.5 ), "1.500" );

    arr = []
    for i in C_IMP1.each():
      arr.append(i)
    self.assertEqual( arr, [ 0, 1, 2, 3, 4, 5, 6, 0 ] )

    self.assertEqual( C_IMP2.s1(), 4451 );
    C_IMP2.s2( 2.0 )
    self.assertEqual( C_IMP2.s3( -1.5 ), "-1.500" );

    arr = []
    for i in C_IMP2.each():
      arr.append(i)
    self.assertEqual( arr, [ 0, 1, 2, 3, 4, 5, 6, 0, 0, 1 ] )

  def test_20(self):

    b = pya.PB()

    a1 = b.b14a( True )
    a2 = b.b14b()
    self.assertEqual( a1.a1(), 17 )
    self.assertEqual( a2.a1(), 17 )
    a1.a5( 761 )
    self.assertEqual( a1.a1(), 761 )
    self.assertEqual( a2.a1(), 761 )

    a1 = b.b14a( False )
    self.assertEqual( a1, None )
    
    self.assertEqual( b.b15( b.b14b() ), True )
    self.assertEqual( b.b15( b.b14a( False ) ), False )
    self.assertEqual( b.b15( None ), False )

  def test_21(self):

    # Python does not allow extending built-in types - the following test
    # is taken from the Ruby binding. I don't know how to implement it for 
    # Python however.
    return

    # test client data binding to C++ objects 

    b = pya.PB()
    
    # b14b() returns a pya.PA object, but it cannot be extended dynamically by a method s ...
    b.b14b().s( 117 )
    self.assertEqual( b.b14b().g(), 117 )

    n = 0

    def handler(a):
      a.s(n)
      n += 1

    b.b10_nc( lambda a: handler(a) ) 

    arr = []
    b.b10( lambda a: arr.append( a.g ) )
    self.assertEqual( arr, [ 0, 1, 2 ] )

    arr = []
    b.b10p( lambda a: arr.append( a.g ) )
    self.assertEqual( arr, [ 0, 1, 2 ] )

  def test_22(self):

    # test client data binding to C++ objects 
    
    b = pya.PB()
    
    self.assertEqual( b.b20a( 5.0 ), False )
    self.assertEqual( b.b20a( None ), True )
    self.assertEqual( b.b20a( 1 ), False )
    self.assertEqual( b.b20a( "hallo" ), False )
    self.assertEqual( b.b20a( False ), False )
    self.assertEqual( b.b20a( True ), False )
    self.assertEqual( b.b20a( 10000000000000000 ), False )
    self.assertEqual( b.b20b( 5.0 ), True )
    self.assertEqual( b.b20b( None ), False )
    self.assertEqual( b.b20b( 1 ), False )
    self.assertEqual( b.b20b( "hallo" ), False )
    self.assertEqual( b.b20b( False ), False )
    self.assertEqual( b.b20b( True ), False )
    # on 64 bit platforms this fits into a long value, therefore this test returns false:
    # self.assertEqual( b.b20b( 10000000000000000 ), False )
    self.assertEqual( b.b20c( 5.0 ), False )
    self.assertEqual( b.b20c( None ), False )
    self.assertEqual( b.b20c( 10000000000 ), True )
    self.assertEqual( b.b20c( "hallo" ), False )
    self.assertEqual( b.b20c( False ), False )
    self.assertEqual( b.b20c( True ), False )
    # on 64 bit platforms this fits into a long value, therefore this test returns true:
    # self.assertEqual( b.b20c( 10000000000000000 ), False )
    self.assertEqual( b.b20d( 5.0 ), False )
    self.assertEqual( b.b20d( None ), False )
    self.assertEqual( b.b20d( 1 ), False )
    self.assertEqual( b.b20d( "hallo" ), True )
    self.assertEqual( b.b20d( False ), False )
    self.assertEqual( b.b20d( True ), False )
    self.assertEqual( b.b20d( 10000000000000000 ), False )
    self.assertEqual( b.b20e( 5.0 ), False )
    self.assertEqual( b.b20e( None ), False )
    self.assertEqual( b.b20e( 1 ), False )
    self.assertEqual( b.b20e( "hallo" ), False )
    self.assertEqual( b.b20e( False ), True )
    self.assertEqual( b.b20e( True ), True )
    self.assertEqual( b.b20e( 10000000000000000 ), False )

    self.assertEqual( b.b21a( 50 ), "50" )
    self.assertEqual( b.b21a( True ), "true" )
    self.assertEqual( b.b21a( False ), "false" )
    self.assertEqual( b.b21a( "hallo" ), "hallo" )
    self.assertEqual( b.b21a( 5.5 ), "5.5" )
    self.assertEqual( b.b21a( None ), "nil" )

    self.assertEqual( b.b21b( 50 ), 50.0 )
    self.assertEqual( b.b21b( True ), 1.0 )
    self.assertEqual( b.b21b( False ), 0.0 )
    self.assertEqual( b.b21b( 5.5 ), 5.5 )
    self.assertEqual( b.b21b( None ), 0.0 )

    self.assertEqual( b.b21c( 50 ), 50 )
    self.assertEqual( b.b21c( True ), 1 )
    self.assertEqual( b.b21c( False ), 0 )
    self.assertEqual( b.b21c( 5.5 ), 5 )
    self.assertEqual( b.b21c( None ), 0 )

    self.assertEqual( b.b22a( [ 1, "hallo", 5.5 ] ), 3 ) 
    self.assertEqual( b.b23a(), [ 1, "hallo", 5.5 ] ) 
    a = [] 
    for x in b.b24():
      a.append(x) 
    self.assertEqual( a, [ 1, "hallo", 5.5 ] ) 
    self.assertEqual( b.b22c(), 5.5 )
    self.assertEqual( b.b22d(), 5.5 )
    self.assertEqual( b.b22a( [ 1, "hallo" ] ), 2 ) 
    self.assertEqual( b.b23b(), [ 1, "hallo" ] ) 
    self.assertEqual( b.b23d(), [ 1, "hallo" ] ) 
    self.assertEqual( b.b23e(), [ 1, "hallo" ] ) 
    self.assertEqual( b.b23e_null(), None ) 
    self.assertEqual( b.b23f(), [ 1, "hallo" ] ) 
    self.assertEqual( b.b23f_null(), None ) 
    self.assertEqual( b.b22c(), "hallo" )
    self.assertEqual( b.b22d(), "hallo" )
    self.assertEqual( b.b22a( [ ] ), 0 ) 
    self.assertEqual( b.b23c(), [ ] ) 
    a = [] 
    for x in b.b24():
      a.append(x) 
    self.assertEqual( a, [ ] ) 
    self.assertEqual( b.b22b(), None )
    self.assertEqual( b.b22c(), None )
    self.assertEqual( b.b22d(), None )
    self.assertEqual( b.b22a( [ [ 1, "hallo" ], [ 10, 17, 20 ] ] ), 2 ) 
    self.assertEqual( b.b23a(), [ [ 1, "hallo" ], [ 10, 17, 20 ] ] ) 
    a = [] 
    for x in b.b24():
      a.append(x) 
    self.assertEqual( a, [ [ 1, "hallo" ], [ 10, 17, 20 ] ] ) 

    # ability to pass complex objects over tl::Variant:
    self.assertEqual( b.b22a( [ pya.Box(pya.Point(0, 0), pya.Point(10, 20)) ] ), 1 ) 
    self.assertEqual( str(b.b22c()), "(0,0;10,20)" )
    self.assertEqual( type(b.b22c()).__name__, "Box" )

    # ability to pass complex objects over tl::Variant:
    self.assertEqual( b.b22a( [ pya.DBox(pya.DPoint(0, 0), pya.DPoint(10, 20)) ] ), 1 ) 
    self.assertEqual( str(b.b22c()), "(0,0;10,20)" )
    self.assertEqual( type(b.b22c()).__name__, "DBox" )

    # ability to pass complex objects over tl::Variant:
    self.assertEqual( b.b22a( [ pya.LayerInfo("hallo") ] ), 1 ) 
    self.assertEqual( str(b.b22c()), "hallo" )
    self.assertEqual( type(b.b22c()).__name__, "LayerInfo" )

  def test_23(self):

    b = pya.PB()
    a = pya.PA()

    self.assertEqual( b.bx(), 17 )
    self.assertEqual( b.b30(), 17 )
    self.assertEqual( b.bx(5), "xz" )
    self.assertEqual( b.by(5), "xz" )
    self.assertEqual( b.b31(6), "xz" )
    self.assertEqual( b.b33(a), "aref" )
    self.assertEqual( b.bx(a), "aref" )
    self.assertEqual( b.bx("a", 15), 20.5 )
    self.assertEqual( b.b32("b", 25), 20.5 )

    err_caught = False
    try: 
      # invalid number of arguments
      self.assertEqual(b.by(), "xz")
    except:
      err_caught = True
    self.assertEqual(err_caught, True)

    err_caught = False
    try: 
      # invalid number of arguments
      self.assertEqual(b.bx( 1, 5, 7 ), "xz")
    except:
      err_caught = True
    self.assertEqual(err_caught, True)
    
    b.destroy()
    a.destroy()

  def test_24(self):

    n = [ 0, 0 , "" ]

    # Events
    e = EEXT()
    e.m = 100

    e.s1() # no event installed
    self.assertEqual( 0, n[0] )
    e.s2()
    self.assertEqual( 0, n[1] )
    e.s3()
    self.assertEqual( "", n[2] )
  
    self.assertEqual( 100, e.m )

    # This is not allowed: e.e0( lambda: n0 = n0 + 1 ), hence we use a function:
    def f0():
      n[0] = n[0] + 1
    e.e0(f0)

    # This is not allowed: e.e1( lambda x: n1 = n1 + x.m ), hence we use a function:
    def f1(x):
      n[1] = n[1] + x.m
    e.e1(f1)

    # This is not allowed: e.e2( lambda i, s: n2 = n2 + str(i) + s), hence we use a function:
    def f2(i, s):
      n[2] = n[2] + str(i) + s
    e.e2(f2)

    e.s1()
    self.assertEqual( 1, n[0] )
    e.s1()
    self.assertEqual( 2, n[0] )

    # This is not allowed: p = lambda: n0 = n0 + 2, hence we use a function:
    def f4():
      n[0] = n[0] + 2

    e.e0(f4)
    e.s1()
    self.assertEqual( 4, n[0] )

    e.s2()
    self.assertEqual( 100, n[1] )
    e.m = 1
    e.s2()
    self.assertEqual( 101, n[1] )

    e.s3()
    self.assertEqual( "18hallo", n[2] )
    e.s3()
    self.assertEqual( "18hallo18hallo", n[2] )

    def raise_excpt():
      raise Exception("X")

    e.e0( lambda x: raise_excpt() )
    error_caught = False
    try: 
      e.s1()
    except:
      error_caught = True
    self.assertEqual( error_caught, True )

    e.e0r( lambda x: 5 )
    self.assertEqual( e.s1r("x"), 5 )
    e.e0r( lambda s: len(s) + 2 )
    self.assertEqual( e.s1r("x"), 3 )
    self.assertEqual( e.s1r("abcxyz"), 8 )

    # Event bound to itself
    e.e0r(e.xfunc)
    self.assertEqual( e.s1r("x"), 1 )
    self.assertEqual( e.s1r("xxx"), 3 )

  def test_25(self):

    # destruction of an instance via c++
    pya.PA.a20(None) 
    ac0 = pya.PA.a0()
    a = pya.PA()
    a.create()
    self.assertEqual(a.destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0 + 1)
    pya.PA.a20(a)    # install static instance of A
    self.assertEqual(a.destroyed(), False)
    pya.PA.a20(None) 
    self.assertEqual(a.destroyed(), True)
    self.assertEqual(pya.PA.a0(), ac0)

    a = pya.PA()
    a.create()
    self.assertEqual(a.destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0 + 1)
    pya.PA.a20(a)    # install static instance of A
    self.assertEqual(a.destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0 + 1)
    pya.PA.a20(a)    # re-install static instance of A
    self.assertEqual(a.destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0 + 1)
    
    # install another instance
    aa = pya.PA()
    aa.create()
    self.assertEqual(aa.destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0 + 2)
    pya.PA.a20(aa)    # install static instance of A

    # original one is destroyed now, only new instance remains
    self.assertEqual(a.destroyed(), True)
    self.assertEqual(aa.destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0 + 1)
    pya.PA.a20(None)    # discard installed instance
    self.assertEqual(aa.destroyed(), True)
    self.assertEqual(pya.PA.a0(), ac0)

    # the same without create .. should work too, but not create an instance because of late 
    # instantiation in default ctor
    a = pya.PA()
    self.assertEqual(a.destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0)
    pya.PA.a20(a)    # install static instance of A
    self.assertEqual(a.destroyed(), False)
    pya.PA.a20(None) 
    self.assertEqual(pya.PA.a0(), ac0)
    self.assertEqual(a.destroyed(), True)

  def test_26(self):

    # cyclic references - event bound to itself

    base_count = EEXT.inst_count() 

    e = EEXT()
    e.e1(e.func)
    e.m = -17
    self.assertEqual(EEXT.inst_count(), base_count + 1)

    self.assertEqual(e.m, -17)
    # s2 will call e.func(self,x) with x=self and func will put x.n (which is 42) into x.m
    e.s2()
    self.assertEqual(e.m, 42)
    self.assertEqual(EEXT.inst_count(), base_count + 1)

    e = None
    self.assertEqual(EEXT.inst_count(), base_count)

  def test_27(self):

    # destruction of an instance via c++
    pya.PA.a20(None)
    ac0 = pya.PA.a0()

    a = pya.PA()
    a._create()
    self.assertEqual(a._destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0 + 1)
    pya.PA.a20(a)    
    self.assertEqual(pya.PA.a20_get() == None, False)
    # release A instance -> will delete it
    a = None
    self.assertEqual(pya.PA.a0(), ac0)
    self.assertEqual(pya.PA.a20_get() == None, True)

    a = pya.PA()
    a._create()
    self.assertEqual(a.destroyed(), False)
    self.assertEqual(pya.PA.a0(), ac0 + 1)
    pya.PA.a20(a)    # install static instance of A
    self.assertEqual(pya.PA.a20_get() == None, False)
    a._unmanage()
    # release A instance -> won't delete it since it is unmanaged
    a = None
    self.assertEqual(pya.PA.a0(), ac0 + 1)
    self.assertEqual(pya.PA.a20_get() == None, False)

    a = pya.PA.a20_get()
    a._manage()
    # release A instance -> will be deleted since now it's managed again
    a = None
    self.assertEqual(pya.PA.a0(), ac0)
    self.assertEqual(pya.PA.a20_get() == None, True)

  def test_28(self):

    self.assertEqual(pya.PB.inst() == None, True)
    self.assertEqual(pya.PB.has_inst(), False)

    b = pya.PB()
    pya.PB.set_inst(b)
    self.assertEqual(pya.PB.has_inst(), True)
    self.assertEqual(pya.PB.inst() == b, False)
    self.assertEqual(pya.PB.inst().addr(), b.addr())

    # new B instance -> will delete the old one
    b = None
    self.assertEqual(pya.PB.has_inst(), False)

    b = pya.PB()
    pya.PB.set_inst(b)
    b._unmanage()
    ba = b.addr()
    self.assertEqual(pya.PB.has_inst(), True)
    self.assertEqual(pya.PB.inst() == b, False)
    self.assertEqual(pya.PB.inst().addr(), b.addr())

    # new B instance -> will not delete the old one (since we made it unmanaged)
    b = None
    self.assertEqual(pya.PB.has_inst(), True)
    self.assertEqual(pya.PB.inst().addr(), ba)

    # Make it managed again
    pya.PB.inst()._manage()

    # new B instance -> will delete the old one (since we made it managed again)
    b = None
    self.assertEqual(pya.PB.has_inst(), False)

  def test_30(self):

    # some basic tests for the *Value boxing classes

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = 17.5
    self.assertEqual(val.value, 17.5)
    self.assertEqual(val.to_s(), "17.5")
    val.value += 1
    self.assertEqual(val.to_s(), "18.5")
    val = pya.Value(5)
    self.assertEqual(val.value, 5)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = 17.5
    self.assertEqual(val.value, 17.5)
    self.assertEqual(val.to_s(), "17.5")
    val.value += 1
    self.assertEqual(val.to_s(), "18.5")
    val = pya.Value(5)
    self.assertEqual(val.value, 5)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = True
    self.assertEqual(val.value, True)
    self.assertEqual(val.to_s(), "true")
    val = pya.Value(True)
    self.assertEqual(val.value, True)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = 17
    self.assertEqual(val.value, 17)
    self.assertEqual(val.to_s(), "17")
    val.value += 1
    self.assertEqual(val.to_s(), "18")
    val = pya.Value(5)
    self.assertEqual(val.value, 5)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = 17
    self.assertEqual(val.value, 17)
    self.assertEqual(val.to_s(), "17")
    val.value += 1
    self.assertEqual(val.to_s(), "18")
    val = pya.Value(5)
    self.assertEqual(val.value, 5)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = 17
    self.assertEqual(val.value, 17)
    self.assertEqual(val.to_s(), "17")
    val.value += 1
    self.assertEqual(val.to_s(), "18")
    val = pya.Value(5)
    self.assertEqual(val.value, 5)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = 2700000000
    self.assertEqual(val.value, 2700000000)
    self.assertEqual(val.to_s(), "2700000000")
    val.value += 1
    self.assertEqual(val.to_s(), "2700000001")
    val = pya.Value(500000000)
    self.assertEqual(val.value, 500000000)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = 170000000000
    self.assertEqual(val.to_s(), "170000000000")
    self.assertEqual(val.value, 170000000000)
    val.value += 1
    self.assertEqual(val.to_s(), "170000000001")
    val = pya.Value(50000000000)
    self.assertEqual(val.value, 50000000000)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.to_s(), "nil")
    self.assertEqual(val.value, None)
    val.value = 170000000000
    self.assertEqual(val.value, 170000000000)
    self.assertEqual(val.to_s(), "170000000000")
    val.value += 1
    self.assertEqual(val.to_s(), "170000000001")
    val = pya.Value(50000000000)
    self.assertEqual(val.value, 50000000000)
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

    val = pya.Value()
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")
    val.value = "abc"
    self.assertEqual(val.value, "abc")
    self.assertEqual(val.to_s(), "abc")
    val.value += "x"
    self.assertEqual(val.to_s(), "abcx")
    val = pya.Value("uv")
    self.assertEqual(val.value, "uv")
    val.value = None
    self.assertEqual(val.value, None)
    self.assertEqual(val.to_s(), "nil")

  def test_31(self):

    # some basic tests with derived and base classes

    pya.PX.init()
    pya.PY.init()
    x = pya.PX("hallo")
    self.assertEqual(True, isinstance(x, pya.PX))
    self.assertEqual(False, isinstance(x, pya.PA))
    self.assertEqual(False, isinstance(x, pya.PY))
    self.assertEqual("hallo", x.s)
    self.assertEqual("PYT_X", x.cls_name())
    cxp = pya.PX.x_cptr()
    self.assertEqual("PYT_X::a", cxp.s)
    try: 
      cxp.s = "x"
      error_caught = False
    except:
      error_caught = True
    self.assertEqual(True, error_caught)
    xp = pya.PX.x_ptr()
    self.assertEqual("PYT_X::a", xp.s)

    s_old = xp.s
    xp.s = "x"
    self.assertEqual("x", xp.s)
    xp.s = 41
    self.assertEqual("42", xp.s)
    xp.s = s_old

    y = pya.PY("hallo")
    self.assertEqual(True, isinstance(y, pya.PX))
    self.assertEqual(False, isinstance(y, pya.PA))
    self.assertEqual(True, isinstance(y, pya.PY))
    self.assertEqual("hallo", y.s)
    self.assertEqual("PYT_Y", y.cls_name())
    self.assertEqual(5, y.i())
    cyp = pya.PY.y_cptr()
    self.assertEqual("PYT_Y::a", cyp.s)
    self.assertEqual("PYT_Y", cyp.cls_name())
    try: 
      cyp.s = "y"
      error_caught = False
    except:
      error_caught = True
    self.assertEqual(True, error_caught)
    yp = pya.PY.y_ptr()
    self.assertEqual("PYT_Y", yp.cls_name())
    self.assertEqual("PYT_Y::a", yp.s)
    yp.s = "y"
    self.assertEqual("y", yp.s)
    self.assertEqual(1, yp.i())
    yp.s = "abc"
    self.assertEqual(3, yp.i())
    self.assertEqual("PY", type(yp).__name__)

  def test_32(self):

    # run test only if we have Qt bindings 
    if not "QStringPair" in pya.__dict__:
      return

    # QPair<String, String>
    p = pya.QStringPair()
    p.first = "a"
    p.second = "b"
    self.assertEqual("a", p.first)
    self.assertEqual("b", p.second)
    pp = p.dup()
    self.assertEqual("a", pp.first)
    self.assertEqual("b", pp.second)
    pp.first = "u"
    self.assertEqual("a", p.first)
    self.assertEqual("b", p.second)
    self.assertEqual("u", pp.first)
    self.assertEqual("b", pp.second)
    self.assertEqual(pp == p, False)
    self.assertEqual(pp != p, True)
    pp = pya.QStringPair("a", "b")
    self.assertEqual("a", pp.first)
    self.assertEqual("b", pp.second)
    self.assertEqual(pp == p, True)
    self.assertEqual(pp != p, False)

  def test_33(self):
  
    def str_from_bytearray(ba):
      return ba.decode("utf-8")  

    # run test only if we have Qt bindings 
    if not "QByteArrayPair" in pya.__dict__:
      return

    # QPair<QByteArray, QByteArray>
    p = pya.QByteArrayPair()
    p.first = "a"
    p.second = "b"
    self.assertEqual("a", str_from_bytearray(p.first))
    self.assertEqual("b", str_from_bytearray(p.second))
    pp = p.dup()
    self.assertEqual("a", str_from_bytearray(pp.first))
    self.assertEqual("b", str_from_bytearray(pp.second))
    pp.first = "u"
    self.assertEqual("a", str_from_bytearray(p.first))
    self.assertEqual("b", str_from_bytearray(p.second))
    self.assertEqual("u", str_from_bytearray(pp.first))
    self.assertEqual("b", str_from_bytearray(pp.second))
    self.assertEqual(pp == p, False)
    self.assertEqual(pp != p, True)
    pp = pya.QByteArrayPair("a", "b")
    self.assertEqual("a", str_from_bytearray(pp.first))
    self.assertEqual("b", str_from_bytearray(pp.second))
    self.assertEqual(pp == p, True)
    self.assertEqual(pp != p, False)

  def test_34(self):

    # run test only if we have Qt bindings 
    if not "QDialog" in pya.__dict__:
      return

    # QDialog and QWidget
    # Hint: QApplication creates some leaks (FT, GTK). Hence it must not be used in the leak_check case ..
    if not leak_check:
      # Note: Qt 4.8.5 with QTK+ style does not like QApplication to be initialized twice. 
      # hence we create a global instance once
      if not "g_app" in globals():
        globals()["g_app"] = pya.QApplication.new_app()
      a = globals()["g_app"]
      qd = pya.QDialog()
      pya.QApplication.setActiveWindow(qd)
      self.assertEqual(repr(pya.QApplication.activeWindow), repr(qd))
      self.assertEqual("<class 'pya.QDialog'>", str(type(pya.QApplication.activeWindow)))
      qd._destroy()
      self.assertEqual(repr(pya.QApplication.activeWindow), "None")

  def test_35(self):

    # vectors of pointers

    pya.PX.init
    pya.PY.init
    vx = pya.PX.vx()
    self.assertEqual(2, len(vx))
    self.assertEqual("PYT_X::a", vx[0].s)
    self.assertEqual("PYT_X::b", vx[1].s)

    vx_ptr = pya.PX.vx_ptr()
    vx_cptr = pya.PX.vx_cptr()
    self.assertEqual(2, len(vx_ptr))
    self.assertEqual(2, len(vx_cptr))
    self.assertEqual("PX", type(vx_ptr[0]).__name__)
    self.assertEqual("PX", type(vx_ptr[1]).__name__)

    try: 
      vx_cptr[0].s = "y"
      error_caught = False
    except:
      error_caught = True
    self.assertEqual(True, error_caught)

    vx_ptr[0].s = "u"
    self.assertEqual("u", vx_cptr[0].s)
    self.assertEqual("PYT_X::a", vx[0].s)
    self.assertEqual("PYT_X::b", vx[1].s)

    vy0_ptr = pya.PY.vy0_ptr()
    self.assertEqual(1, len(vy0_ptr))
    self.assertEqual("None", str(vy0_ptr[0]))

    vy_ptr = pya.PY.vy_ptr()
    vy_cptr = pya.PY.vy_cptr()
    self.assertEqual(2, len(vy_ptr))
    self.assertEqual(2, len(vy_cptr))
    self.assertEqual("PY", type(vy_ptr[0]).__name__)
    self.assertEqual("PY", type(vy_ptr[1]).__name__)

    try: 
      vy_cptr[0].s = "y"
      error_caught = False
    except:
      error_caught = True
    self.assertEqual(True, error_caught)

    vy_ptr[0].s = "uvw"
    self.assertEqual("uvw", vy_cptr[0].s)
    self.assertEqual(3, vy_cptr[0].i())

    vy_ptr = pya.PY.vyasx_ptr()
    vy_cptr = pya.PY.vyasx_cptr()
    self.assertEqual(2, len(vy_ptr))
    self.assertEqual(2, len(vy_cptr))
    self.assertEqual("PY", type(vy_ptr[0]).__name__)
    self.assertEqual("PY", type(vy_ptr[1]).__name__)

    try: 
      vy_cptr[0].s = "y"
      error_caught = False
    except:
      error_caught = True
    self.assertEqual(True, error_caught)

    vy_ptr[0].s = "uvw"
    self.assertEqual("uvw", vy_cptr[0].s)
    self.assertEqual(3, vy_cptr[0].i())

    y = pya.PY("")
    yc = y.vx_dyn_count()
    y.vx_dyn_make()
    self.assertEqual(yc + 1, y.vx_dyn_count())
    y.vx_dyn_destroy()
    self.assertEqual(yc, y.vx_dyn_count())

    y.vx_dyn_make()
    self.assertEqual(yc + 1, y.vx_dyn_count())
    yy = y.vx_dyn()
    self.assertEqual(1, len(yy))
    self.assertEqual("PY", type(yy[0]).__name__)
    self.assertEqual(True, yy[0] != None)
    yy[0].destroy()
    self.assertEqual(True, yy[0].destroyed())
    self.assertEqual(yc, y.vx_dyn_count())

    y.vx_dyn_make()
    self.assertEqual(yc + 1, y.vx_dyn_count())
    yy = y.vx_dyn()
    self.assertEqual(1, len(yy))
    self.assertEqual("PY", type(yy[0]).__name__)
    self.assertEqual(True, yy[0] != None)
    y.vx_dyn_destroy()
    self.assertEqual(True, yy[0].destroyed())
    self.assertEqual(yc, y.vx_dyn_count())

  def test_36(self):

    x = XEdge()
    self.assertEqual("XEdge", type(x).__name__)
    self.assertEqual("(1,2;3,4)", str(x))

  def test_37(self):

    # This test is taken from the Ruby binding, but
    # Python does not have protected methods:
    return

    # protected methods
    ok = False
    a = pya.PA()
    e = ""
    try:
      a.a10_prot() # cannot be called - is protected
      ok = True
    except Exception as ex:
      e = str(ex)
    self.assertEqual(e == "protected method `a10_prot' called", 0)
    self.assertEqual(ok, False)
    self.assertEqual(a.call_a10_prot(1.25), "1.25")

  def test_38(self):

    # mixed const / non-const reference and events
    ec = pya.PE.ic()
    self.assertEqual(ec.is_const_object(), True)
    enc = pya.PE.inc()
    # Now, ec has turned into a non-const reference as well!
    # This is strange but is a consequence of the unique C++/Ruby binding and there can 
    # only be a non-const or const ruby object!
    self.assertEqual(ec.is_const_object(), False)
    self.assertEqual(enc.is_const_object(), False)

    # the "true reference" is a not copy since E is derived from ObjectBase
    ec.x = 15
    self.assertEqual(ec.x, 15);
    ec2 = pya.PE.ic()
    self.assertEqual(ec2.x, 15);
    ec2 = pya.PE.icref()
    self.assertEqual(ec2.x, 15);
    ec2.x = 17
    self.assertEqual(ec2.x, 17);
    self.assertEqual(ec.x, 17);
    self.assertEqual(ec2.is_const_object(), False) # because it's a copy

    # the "true reference" is a not copy since E is derived from ObjectBase
    enc2 = pya.PE.incref()
    self.assertEqual(enc2.x, 17);
    enc2.x = 19
    self.assertEqual(enc2.x, 19);
    self.assertEqual(ec.x, 19);  # because the non-const reference by incref is not a copy

  def test_39(self):

    # mixed const / non-const reference and events
    fc = pya.PF.ic()
    self.assertEqual(fc.is_const_object(), True)
    fnc = pya.PF.inc()
    # In contrase to E, the fc reference is not touched because F is not derived
    # from ObjectBase
    self.assertEqual(fc.is_const_object(), True)
    self.assertEqual(fnc.is_const_object(), False)

    # the "true reference" is a copy
    fnc.x = 15
    self.assertEqual(fc.x, 15);
    fc2 = pya.PF.ic()
    self.assertEqual(fc2.x, 15);
    fc2 = pya.PF.icref()
    self.assertEqual(fc2.x, 15);
    fc2.x = 17
    self.assertEqual(fc2.x, 17);
    self.assertEqual(fc.x, 15);
    self.assertEqual(fc2.is_const_object(), False) # because it's a copy

    # the "true reference" is a copy
    fnc2 = pya.PF.incref()
    self.assertEqual(fnc2.x, 15);
    fnc2.x = 19
    self.assertEqual(fnc2.x, 19);
    self.assertEqual(fc.x, 19);  # because the non-const reference by incref is not a copy

  def test_40(self):

    # optional arguments
    g = pya.PG()

    self.assertEqual(g.iv(), 0)
    g.set_iva(2)
    self.assertEqual(g.iv(), 2)
    g.set_ivb(3)
    self.assertEqual(g.iv(), 3)
    g.set_ivb()
    self.assertEqual(g.iv(), 1)
    g.set_sv1a("hello")
    self.assertEqual(g.sv(), "hello")

    failed = False
    try:
      g.set_sv1a()
    except: 
      failed = True
    self.assertEqual(failed, True)

    g.set_sv1b("world")
    self.assertEqual(g.sv(), "world")
    g.set_sv1b()
    self.assertEqual(g.sv(), "value")
    g.set_sv2a("hello")
    self.assertEqual(g.sv(), "hello")

    failed = False
    try:
      g.set_sv2a()
    except: 
      failed = True
    self.assertEqual(failed, True)

    g.set_sv2b("world")
    self.assertEqual(g.sv(), "world")
    g.set_sv2b()
    self.assertEqual(g.sv(), "value")

    g.set_vva(17, "c")
    self.assertEqual(g.iv(), 17)
    self.assertEqual(g.sv(), "c")

    failed = False
    try:
      g.set_svva()
    except: 
      failed = True
    self.assertEqual(failed, True)

    failed = False
    try:
      g.set_svva(11)
    except: 
      failed = True
    self.assertEqual(failed, True)

    g.set_vvb(11)
    self.assertEqual(g.iv(), 11)
    self.assertEqual(g.sv(), "value")
    g.set_vvb(10, "nil")
    self.assertEqual(g.iv(), 10)
    self.assertEqual(g.sv(), "nil")

    failed = False
    try:
      g.set_svvb()
    except: 
      failed = True
    self.assertEqual(failed, True)

    g.set_vvc(11)
    self.assertEqual(g.iv(), 11)
    self.assertEqual(g.sv(), "value")
    g.set_vvc()
    self.assertEqual(g.iv(), 1)
    self.assertEqual(g.sv(), "value")
    g.set_vvc(17, "nil")
    self.assertEqual(g.iv(), 17)
    self.assertEqual(g.sv(), "nil")

  def test_41(self):

    v3 = sys.version_info >= (3, 0)

    # maps 
    b = pya.PB()

    b.insert_map1(1, "hello")
    if v3:
      self.assertEqual(repr(b.map1), "{1: 'hello'}")
    else:
      self.assertEqual(repr(b.map1), "{1L: 'hello'}")

    b.map1 = {}
    b.insert_map1(2, "hello")
    if v3:
      self.assertEqual(repr(b.map1_cref()), "{2: 'hello'}")
    else:
      self.assertEqual(repr(b.map1_cref()), "{2L: 'hello'}")

    b.map1 = {}
    b.insert_map1(3, "hello")
    if v3:
      self.assertEqual(repr(b.map1_cptr()), "{3: 'hello'}")
    else:
      self.assertEqual(repr(b.map1_cptr()), "{3L: 'hello'}")

    b.map1 = {}
    b.insert_map1(4, "hello")
    if v3:
      self.assertEqual(repr(b.map1_ref()), "{4: 'hello'}")
    else:
      self.assertEqual(repr(b.map1_ref()), "{4L: 'hello'}")

    b.map1 = {}
    b.insert_map1(5, "hello")
    if v3:
      self.assertEqual(repr(b.map1_ptr()), "{5: 'hello'}")
    else:
      self.assertEqual(repr(b.map1_ptr()), "{5L: 'hello'}")

    self.assertEqual(b.map1_cptr_null() == None, True);
    self.assertEqual(b.map1_ptr_null() == None, True);

    try: 
      # error converting 1 or True to string
      b.map1 = { 42: 1, -17: True }
      error_caught = False
    except:
      error_caught = True
    self.assertEqual(error_caught, True)

    b.map1 = { 42: "1", -17: "true" }
    if v3:
      self.assertEqual(repr(b.map1), "{42: '1', -17: 'true'}")
    else:
      self.assertEqual(repr(b.map1), "{42L: '1', -17L: 'true'}")

    b.set_map1_cref({ 42: "2", -17: "true" })
    if v3:
      self.assertEqual(repr(b.map1), "{42: '2', -17: 'true'}")
    else:
      self.assertEqual(repr(b.map1), "{42L: '2', -17L: 'true'}")

    b.set_map1_cptr({ 42: "3", -17: "true" })
    if v3:
      self.assertEqual(repr(b.map1), "{42: '3', -17: 'true'}")
    else:
      self.assertEqual(repr(b.map1), "{42L: '3', -17L: 'true'}")

    b.set_map1_cptr(None)
    if v3:
      self.assertEqual(repr(b.map1), "{42: '3', -17: 'true'}")
    else:
      self.assertEqual(repr(b.map1), "{42L: '3', -17L: 'true'}")

    b.set_map1_ref({ 42: "4", -17: "true" })
    if v3:
      self.assertEqual(repr(b.map1), "{42: '4', -17: 'true'}")
    else:
      self.assertEqual(repr(b.map1), "{42L: '4', -17L: 'true'}")

    b.set_map1_ptr({ 42: "5", -17: "true" })
    if v3:
      self.assertEqual(repr(b.map1), "{42: '5', -17: 'true'}")
    else:
      self.assertEqual(repr(b.map1), "{42L: '5', -17L: 'true'}")

    b.set_map1_ptr(None)
    if v3:
      self.assertEqual(repr(b.map1), "{42: '5', -17: 'true'}")
    else:
      self.assertEqual(repr(b.map1), "{42L: '5', -17L: 'true'}")

    b.map2 = { 'xy': 1, -17: True }
    if v3:
      self.assertEqual(repr(b.map2), "{'xy': 1, -17: True}")
    else:
      self.assertEqual(repr(b.map2), "{'xy': 1L, -17L: True}")

    self.assertEqual(b.map2_null(), None)

  def test_42(self):

    # virtual functions and sub-classes 
    z = pya.PZ()
    self.assertEqual(z.f(None), "(nil)")
    self.assertEqual(z.f(pya.PX("hello")), "hello")

    z1 = Z_IMP1()
    self.assertEqual(z1.f(pya.PX("a")), "PYT_X")
    self.assertEqual(z1.f(pya.PY("b")), "PYT_Y")
    self.assertEqual(z1.f_with_x("a"), "PYT_X")
    self.assertEqual(z1.f_with_y("b"), "PYT_Y")
    self.assertEqual(z1.f_with_yy("b"), "PYT_YY")

    z2 = Z_IMP2()
    self.assertEqual(z2.f(pya.PX("1")), "PX")
    self.assertEqual(z2.f(pya.PY("2")), "PY")
    self.assertEqual(z2.f_with_x("1"), "PX")
    self.assertEqual(z2.f_with_y("2"), "PY")
    self.assertEqual(z2.f_with_yy("3"), "PY")

    z1 = Z_IMP3()
    self.assertEqual(z1.f(pya.PX("x")), "x*")
    self.assertEqual(z1.f(pya.PY("y")), "y*")
    self.assertEqual(z1.f_with_x("x"), "x*")
    self.assertEqual(z1.f_with_y("y"), "y*")
    self.assertEqual(z1.f_with_yy("yy"), "yy*")

  def test_999(self):
    pass

# Run only a single test:
#sys.argv = ["basic.py", "BasicTest.test_999"]
sys.argv = ["basic.py"]

# run unit tests
if __name__ == '__main__':
  unittest.main()

