view off 

layer 250 color 15
color 15 level 15

global #ed.cmd = %tmp^work.cmd 
global #uned.cmd = %tmp^uned.cmd 
global #ed.id0=%id.max
global #node.no=$prompt "Enter node number"

default local #max.lines=10000
dos '^outliner %cell^.P9K "%ed.cmd" %node.no %max.lines > "%tmp^n0.dbg"'
dos '^copy "%ed.cmd" "%uned.cmd" > nul'
@"%ed.cmd"; log off
global #ed.id1=%id.max
view on
blink
$ %outline.msg
