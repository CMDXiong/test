if(macro_exists(#drc.undo.mode)==0) @drcmac

xselect off
if(cmp(%drc.root,%cell)!=0){
   error "Cannot unload DRC data -- DRC was run in a different cell (%drc.root)"
}
else if(%drc.undo.mode==0){
   local #n.sel = %n.select
   if(%n.sel!=0) unselect push   
   select ids after %drc.undo.id0
   unselect ids after %drc.undo.id1
   delete
   global #iced.exit.root=@_exit_iced.cmd
   global #iced.exit.subcell=@_exit_iced.cmd
   if(%n.sel!=0) unselect pop
   $ DRC data erased
}
else if(%drc.undo.mode==-1){
   error "Cannot unload DRC data -- DRC failed"
}
else{
   error "Cannot unload DRC data -- DRC.UNDO.MODE=%drc.undo.mode is an invalid mode number"
}

