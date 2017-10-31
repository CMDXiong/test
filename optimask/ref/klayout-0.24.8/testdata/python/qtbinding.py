
import pya
import unittest
import os
import sys
import gc

testlog = ""

# an event filter class

class EventFilter(pya.QObject):

  _log = []

  def log(self):
    return self._log

  def eventFilter(self, obj, event):
    self._log.append(type(event).__name__ + ": " + str(event.type()))
    pya.QObject.eventFilter(self, obj, event)

# QAction implementation

class MyAction(pya.QAction):

  ce = None

  def __init__(self, p, n):
    pya.QAction.__init__(self, p)
    self.objectName = n
    ce = None

  def childEvent(self, ev):
    if self.ce:
      self.ce(ev)
  
  def on_child_event(self, _ce):
    self.ce = _ce
  
# Another event filter 

class MyObject(pya.QObject):

  ef = None

  def eventFilter(self, watched, event):
    if self.ef(watched, event):
      return True
    return pya.QObject.eventFilter(self, watched, event)

  def on_event_filter(self, ef):
    self.ef = ef
  
# The Qt binding tests

class QtBindingTest(unittest.TestCase):

  def test_00(self):

    # all references of PA are released now:
    pass

  # ...

  def test_10(self):

    a = MyAction(None, "a")
    a.text = "mytext"
    a.checkable = True
    self.assertEqual(a.isChecked(), False)
    a.checked = True
    self.assertEqual(a.text, "mytext")
    self.assertEqual(a.objectName, "a")
    a.text += "."
    self.assertEqual(a.text, "mytext.")
    self.assertEqual(a.checked, True)

    global testlog
    testlog = ""

    def f(checked):
      global testlog
      testlog += "[" + str(checked) + "]"

    a.triggered(f)
    self.assertEqual(testlog, "")
    a.trigger() # also toggles checked state
    self.assertEqual(testlog, "[False]")
    testlog = ""
    a.trigger() # also toggles checked state
    self.assertEqual(testlog, "[True]")

  def test_11(self):

    a = pya.QAction(None)
    aa = MyAction(a, "aa")
    self.assertEqual(aa.objectName, "aa")

    # destroying a will also destroy aa
    a.destroy()
    self.assertEqual(a.destroyed(), True)
    self.assertEqual(aa.destroyed(), True)

  def test_12(self):

    a = pya.QAction(None)
    aa = pya.QAction(a)
    aa.objectName = "aa"

    # destroying a will also destroy aa
    a = None

    self.assertEqual(aa.destroyed(), True)

  def test_13(self):

    a = pya.QAction(None)
    aa = pya.QAction(a)
    aa.objectName = "aa"
    aa.text = "aatext"

    cc = []
    for c in a.children():
      cc.append(c.objectName)
    self.assertEqual(",".join(cc), "aa")

    # aa now is kept by a
    aa = None

    # fetch aa again
    for c in a.children():
      if c.objectName == "aa":
        aa = c
    self.assertEqual(aa != None, True)
    self.assertEqual(type(aa), pya.QAction)
    self.assertEqual(aa.text, "aatext")
    self.assertEqual(aa.destroyed(), False)

  def test_20(self):

    global no_event
    global ce_log

    no_event = False

    def event_filter(watched, event):
      global no_event
      return no_event

    ef = MyObject()
    ef.on_event_filter(event_filter)

    def child_event(ce):
      global ce_log
      ce_log.append(str(ce.added()) + ":" + ce.child().objectName)

    ce_log = []
    a = MyAction(None, "a")
    a.on_child_event(child_event)

    a.installEventFilter(ef)

    aa = MyAction(None, "aa")
    self.assertEqual(",".join(ce_log), "")
    aa.setParent(a)
    self.assertEqual(",".join(ce_log), "True:aa")
    ce_log = []

    # destroy aa
    aa.destroy()
    aa = None
    self.assertEqual(",".join(ce_log), "False:aa")
    ce_log = []

    no_event = True
    aa = MyAction(None, "aa")
    aa.setParent(a)
    self.assertEqual(",".join(ce_log), "")
    ce_log = []

    no_event = False
    aa.destroy()
    aa = None
    self.assertEqual(",".join(ce_log), "False:aa")
    ce_log = []

  def test_30(self):

    # dialog construction, cleanup, object dependency ...

    mw = None 

    dialog = pya.QDialog(mw)
    label = pya.QLabel(dialog)
    layout = pya.QHBoxLayout(dialog)
    layout.addWidget(label)

    dialog = pya.QDialog(mw)
    label = pya.QLabel(dialog)
    layout = pya.QHBoxLayout(dialog)
    layout.addWidget(label)
    label.destroy()

    dialog = pya.QDialog(mw)
    label = pya.QLabel(dialog)
    layout = pya.QHBoxLayout(dialog)
    layout.addWidget(label)
    layout.destroy()

    dialog = pya.QDialog(mw)
    label = pya.QLabel(dialog)
    layout = pya.QHBoxLayout(dialog)
    layout.addWidget(label)
    dialog.destroy()

    dialog = pya.QDialog(mw)
    label = pya.QLabel(dialog)
    layout = pya.QHBoxLayout(dialog)
    layout.addWidget(label)

    dialog = None
    label = None
    layout = None

  def test_31(self):

    # Optional arguments, enums, QFlag's

    mw = None

    mb = pya.QMessageBox(pya.QMessageBox.Critical, "title", "text")
    self.assertEqual(mb.icon.to_i() != pya.QMessageBox.Warning.to_i(), True)
    self.assertEqual(mb.icon.to_i() == pya.QMessageBox.Critical.to_i(), True)
    self.assertEqual(mb.standardButtons.to_i() == pya.QMessageBox.NoButton.to_i(), True)

    mb = pya.QMessageBox(pya.QMessageBox.Critical, "title", "text", pya.QMessageBox.Ok)
    self.assertEqual(mb.standardButtons.to_i() == pya.QMessageBox.Ok.to_i(), True)

    mb = pya.QMessageBox(pya.QMessageBox.Critical, "title", "text", pya.QMessageBox.Ok | pya.QMessageBox.Cancel)
    self.assertEqual(mb.standardButtons.to_i() == pya.QMessageBox.Ok.to_i() + pya.QMessageBox.Cancel.to_i(), True)

  def test_40(self):

    # Lifetime management of objects/methods not using QObject.parent
    # QTreeWidget (parent)/QTreeWidgetItem (child)

    # constructor with parent-like argument:
    tw = pya.QTreeWidget()
    ti = pya.QTreeWidgetItem(tw)
    # strange, but true:
    self.assertEqual(ti.parent(), None)
    self.assertEqual(tw.topLevelItemCount, 1)
    ti = None
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    self.assertEqual(tw.topLevelItemCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.topLevelItem(0)
    tw._destroy()
    # gives true, because tw owns ti too.
    self.assertEqual(ti._destroyed(), True)

    # The same works for insert too
    tw = pya.QTreeWidget()
    ti = pya.QTreeWidgetItem()
    tw.insertTopLevelItem(0, ti)
    self.assertEqual(tw.topLevelItemCount, 1)
    ti = None
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    self.assertEqual(tw.topLevelItemCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.topLevelItem(0)
    tw._destroy()
    # gives true, because tw owns ti
    self.assertEqual(ti._destroyed(), True)

    # And add:
    tw = pya.QTreeWidget()
    ti = pya.QTreeWidgetItem()
    tw.addTopLevelItem(ti)
    self.assertEqual(tw.topLevelItemCount, 1)
    ti = None
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    self.assertEqual(tw.topLevelItemCount, 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.topLevelItem(0)
    tw._destroy()
    # gives true, because tw owns ti
    self.assertEqual(ti._destroyed(), True)

    # But the item is released when we take it and add:
    tw = pya.QTreeWidget()
    ti = pya.QTreeWidgetItem()
    tw.addTopLevelItem(ti)
    self.assertEqual(tw.topLevelItemCount, 1)
    ti = None
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    self.assertEqual(tw.topLevelItemCount, 1)

    ti = tw.takeTopLevelItem(0)
    tw._destroy()
    # gives false, because we took ti and tw no longer owns it
    self.assertEqual(ti._destroyed(), False)

    # And we can destroy a child too
    tw = pya.QTreeWidget()
    ti = pya.QTreeWidgetItem()
    tw.addTopLevelItem(ti)
    self.assertEqual(tw.topLevelItemCount, 1)
    ti._destroy()
    self.assertEqual(tw.topLevelItemCount, 0)

  def test_41(self):

    # Lifetime management of objects/methods not using QObject.parent
    # QTreeWidgetItem (parent)/QTreeWidgetItem (child)

    # constructor with parent-like argument (supported by QObject parent/child relationship):
    tw = pya.QTreeWidgetItem()
    ti = pya.QTreeWidgetItem(tw)
    # that's not QObject.parent - this one still is 0 (not seen by RBA)
    self.assertEqual(ti.parent(), tw)
    self.assertEqual(tw.childCount(), 1)
    ti = None
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    self.assertEqual(tw.childCount(), 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.child(0)
    tw._destroy()
    # gives true, because tw owns ti too.
    self.assertEqual(ti._destroyed(), True)

    # The same works for insert too
    tw = pya.QTreeWidgetItem()
    ti = pya.QTreeWidgetItem()
    tw.insertChild(0, ti)
    self.assertEqual(tw.childCount(), 1)
    ti = None
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    self.assertEqual(tw.childCount(), 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.child(0)
    tw._destroy()
    # gives true, because tw owns ti
    self.assertEqual(ti._destroyed(), True)

    # And add:
    tw = pya.QTreeWidgetItem()
    ti = pya.QTreeWidgetItem()
    tw.addChild(ti)
    self.assertEqual(tw.childCount(), 1)
    ti = None
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    self.assertEqual(tw.childCount(), 1)

    # the tree item belongs to the widget, hence it's destroyed with
    # the widget
    ti = tw.child(0)
    tw._destroy()
    # gives true, because tw owns ti
    self.assertEqual(ti._destroyed(), True)

    # But the item is released when we take it and add:
    tw = pya.QTreeWidgetItem()
    ti = pya.QTreeWidgetItem()
    tw.addChild(ti)
    self.assertEqual(tw.childCount(), 1)
    ti = None
    # gives 1, because the tree widget item is kept by
    # the tree widget:
    self.assertEqual(tw.childCount(), 1)

    ti = tw.takeChild(0)
    tw._destroy()
    # gives false, because we took ti and tw no longer owns it
    self.assertEqual(ti._destroyed(), False)

    # And we can destroy a child too
    tw = pya.QTreeWidgetItem()
    ti = pya.QTreeWidgetItem()
    tw.addChild(ti)
    self.assertEqual(tw.childCount(), 1)
    ti._destroy()
    self.assertEqual(tw.childCount(), 0)

  def test_42(self):

    # QKeyEvent and related issues

    ef = EventFilter()

    widget = pya.QLineEdit()
    widget.setText("ABC")

    pya.QApplication.processEvents()

    widget.installEventFilter(ef)

    ke = pya.QKeyEvent(pya.QEvent.KeyPress, pya.Qt.Key_O.to_i(), pya.Qt.ShiftModifier, "O")
    pya.QCoreApplication.postEvent(widget, ke)

    ke = pya.QKeyEvent(pya.QEvent.KeyPress, pya.Qt.Key_Left.to_i(), pya.Qt.NoModifier)
    pya.QCoreApplication.postEvent(widget, ke)

    ke = pya.QKeyEvent(pya.QEvent.KeyPress, pya.Qt.Key_P.to_i(), pya.Qt.NoModifier, "p")
    pya.QCoreApplication.postEvent(widget, ke)

    pya.QApplication.processEvents()

    self.assertEqual("\n".join(ef.log()), "QKeyEvent: ShortcutOverride (51)\nQKeyEvent: KeyPress (6)\nQKeyEvent: ShortcutOverride (51)\nQKeyEvent: KeyPress (6)\nQKeyEvent: ShortcutOverride (51)\nQKeyEvent: KeyPress (6)") 
    ef = None

    self.assertEqual(widget.text, "ABCpO")
    widget = None

  def test_43(self):

    # QVariant 

    v = pya.QVariant()
    self.assertEqual(str(v.type()), "Invalid (0)")
    self.assertEqual(v.toInt(), 0)
    self.assertEqual(v.toDouble(), 0.0)
    self.assertEqual(v.toBool(), False)
    self.assertEqual(v.toString(), None)

    v = pya.QVariant(0)
    self.assertEqual(str(v.type()), "Int (2)")
    self.assertEqual(v.toInt(), 0)
    self.assertEqual(v.toDouble(), 0.0)
    self.assertEqual(v.toBool(), False)
    self.assertEqual(v.toString(), "0")

    v = pya.QVariant(17)
    self.assertEqual(str(v.type()), "Int (2)")
    self.assertEqual(v.toInt(), 17)
    self.assertEqual(v.toDouble(), 17.0)
    self.assertEqual(v.toBool(), True)
    self.assertEqual(v.toString(), "17")

    v = pya.QVariant(True)
    self.assertEqual(str(v.type()), "Bool (1)")
    self.assertEqual(v.toInt(), 1)
    self.assertEqual(v.toString(), "true")

    v = pya.QVariant(42.25)
    self.assertEqual(str(v.type()), "Double (6)")
    self.assertEqual(v.toInt(), 42)
    self.assertEqual(v.toDouble(), 42.25)
    self.assertEqual(v.toBool(), True)
    self.assertEqual(v.toString(), "42.25")

    v = pya.QVariant([ pya.QVariant(1), pya.QVariant(2), pya.QVariant(5) ])
    self.assertEqual(str(v.type()), "List (9)")
    self.assertEqual(v.toString(), None)
    l = []
    for i in v.toList():
      l.append(i.toInt())
    self.assertEqual(l, [1, 2, 5])

    v = pya.QVariant(pya.QPoint(5, 7))
    self.assertEqual(str(v.type()), "Point (25)")
    self.assertEqual(v.toPoint().x, 5)
    self.assertEqual(v.toPoint().y, 7)

    v = pya.QVariant(pya.QColor(10, 20, 30))
    self.assertEqual(str(v.type()), "Color (67)")
    self.assertEqual(v.toColor().red, 10)
    self.assertEqual(v.toColor().green, 20)
    self.assertEqual(v.toColor().blue, 30)

    v = pya.QVariant(pya.QFont("Helvetica", 22))
    self.assertEqual(str(v.type()), "Font (64)")
    self.assertEqual(v.toFont().key(), "Helvetica,22,-1,5,50,0,0,0,0,0")

    v = pya.QVariant(pya.QBitmap())
    self.assertEqual(str(v.type()), "Pixmap (65)")
    self.assertEqual(type(v.toBitmap()), pya.QBitmap_Native)

    v = pya.QVariant(pya.QIcon())
    self.assertEqual(str(v.type()), "Icon (69)")
    self.assertEqual(type(v.toIcon()), pya.QIcon)

    v = pya.QVariant(pya.QPixmap())
    self.assertEqual(str(v.type()), "Pixmap (65)")
    self.assertEqual(type(v.toPixmap()), pya.QPixmap_Native)

    v = pya.QVariant(pya.QImage())
    self.assertEqual(str(v.type()), "Image (70)")
    self.assertEqual(type(v.toImage()), pya.QImage_Native)

    v = pya.QVariant(pya.QKeySequence("Shift+F5"))
    self.assertEqual(str(v.type()), "KeySequence (76)")
    self.assertEqual(v.toKeySequence().toString(), "Shift+F5")

    v = pya.QVariant(pya.QCursor())
    self.assertEqual(str(v.type()), "Cursor (74)")
    self.assertEqual(type(v.toCursor()), pya.QCursor)

    v = pya.QVariant(pya.QBrush())
    self.assertEqual(str(v.type()), "Brush (66)")
    self.assertEqual(type(v.toBrush()), pya.QBrush)

    v = pya.QVariant(pya.QPen())
    self.assertEqual(str(v.type()), "Pen (77)")
    self.assertEqual(type(v.toPen()), pya.QPen)

    v = pya.QVariant(pya.QPalette())
    self.assertEqual(str(v.type()), "Palette (68)")
    self.assertEqual(type(v.toPalette()), pya.QPalette)

    v = pya.QVariant(pya.QTransform())
    self.assertEqual(str(v.type()), "Transform (81)")
    self.assertEqual(type(v.toTransform()), pya.QTransform)

    v = pya.QVariant(pya.QQuaternion())
    self.assertEqual(str(v.type()), "Quaternion (86)")
    self.assertEqual(type(v.toQuaternion()), pya.QQuaternion)

    v = pya.QVariant(pya.QMatrix4x4())
    self.assertEqual(str(v.type()), "Matrix4x4 (82)")
    self.assertEqual(type(v.toMatrix4x4()), pya.QMatrix4x4)

    v = pya.QVariant(pya.QTextFormat())
    self.assertEqual(str(v.type()), "TextFormat (79)")
    self.assertEqual(type(v.toTextFormat()), pya.QTextFormat)

    v = pya.QVariant(pya.QTextLength())
    self.assertEqual(str(v.type()), "TextLength (78)")
    self.assertEqual(type(v.toTextLength()), pya.QTextLength)

    v = pya.QVariant(pya.QVector2D())
    self.assertEqual(str(v.type()), "Vector2D (83)")
    self.assertEqual(type(v.toVector2D()), pya.QVector2D)

    v = pya.QVariant(pya.QVector3D())
    self.assertEqual(str(v.type()), "Vector3D (84)")
    self.assertEqual(type(v.toVector3D()), pya.QVector3D)

    v = pya.QVariant(pya.QVector4D())
    self.assertEqual(str(v.type()), "Vector4D (85)")
    self.assertEqual(type(v.toVector4D()), pya.QVector4D)
  

# Run only a single test:
#sys.argv = ["qtbinding.py", "QtBindingTest.test_999"]
sys.argv = ["qtbinding.py"]

# run unit tests
if __name__ == '__main__':
  unittest.main()

