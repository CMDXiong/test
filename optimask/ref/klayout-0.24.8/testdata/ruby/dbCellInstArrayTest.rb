
$:.push(File::dirname($0))

load("test_prologue.rb")

class DBCellInst_TestClass < TestBase

  # CellInstArray functions
  def test_1_CellInstArray

    i = RBA::CellInstArray.new
    assert_equal(i.to_s, "#0 r0 0,0")

    i = RBA::CellInstArray.new(7, RBA::Trans.new(RBA::Point.new(1, 2)))
    assert_equal(i.to_s, "#7 r0 1,2")
    assert_equal(i.cell_index.to_s, "7")
    i.cell_index = 8
    assert_equal(i.cell_index.to_s, "8")

    assert_equal(i.is_complex?, false)
    i.trans = RBA::Trans.new(3)
    assert_equal(i.is_complex?, false)
    assert_equal(i.to_s, "#8 r270 0,0")

    i.cplx_trans = RBA::CplxTrans.new(1.5)
    assert_equal(i.is_complex?, true)
    assert_equal(i.to_s, "#8 r0 *1.5 0,0")

    assert_equal(i.is_regular_array?, false)
    i.a = RBA::Point.new(10, 20)
    assert_equal(i.is_regular_array?, true)
    assert_equal(i.to_s, "#8 r0 *1.5 0,0 [10,20*1;0,0*1]")

    i.na = 5
    assert_equal(i.is_regular_array?, true)
    assert_equal(i.to_s, "#8 r0 *1.5 0,0 [10,20*5;0,0*1]")

    i.b = RBA::Point.new(30, 40)
    assert_equal(i.is_regular_array?, true)
    assert_equal(i.to_s, "#8 r0 *1.5 0,0 [10,20*5;30,40*1]")

    i.nb = 3
    assert_equal(i.is_regular_array?, true)
    assert_equal(i.to_s, "#8 r0 *1.5 0,0 [10,20*5;30,40*3]")

    ii = i.dup 

    i.nb = 0
    assert_equal(i.is_regular_array?, false)
    assert_equal(i.to_s, "#8 r0 *1.5 0,0")

    assert_equal(ii.to_s, "#8 r0 *1.5 0,0 [10,20*5;30,40*3]")
    ii.na = 0
    assert_equal(ii.to_s, "#8 r0 *1.5 0,0")

    ii.trans = RBA::Trans.new(3)
    assert_equal(ii.is_complex?, false)
    assert_equal(ii.to_s, "#8 r270 0,0")

  end

end

load("test_epilogue.rb")
