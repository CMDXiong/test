view off
global #node.pos=$prompt "Click on node"  pos
dos '^outliner %cell^.P9K "%tmp^tmp.cmd" "%node.pos" 0 > "%tmp^nn.dbg"' 
@"%tmp^tmp.cmd"; view off
$ %outline.msg
