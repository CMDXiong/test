if(macro_exists(#drc.file)==0) @drcmac

if(file_exists("%drc.file")==0){
   error "Could not find %drc.file
}
if(macro_exists(#drc.file.time)){
   if(file_time("%drc.file")!=%drc.file.time){
      error "Cannot reload -- %drc.file has been modified"
   }
}
if(cmp(%drc.root, %cell)!=0){
   error "Cannot reload DRC data -- DRC was run in a different cell (%drc.root)"
}
if(macro_exists(#drc.undo.mode)){
   if(%drc.undo.mode==0){
      xselect off
      select ids after %drc.undo.id0
      unselect ids after %drc.undo.id1
      delete
   }
}
global #drc.undo.mode = -1;
global #drc.log.failed=0
local #error.cmd = "#drc.log.failed=1"
@"%drc.file"
global #iced.exit.root=@_exit_iced.cmd
global #iced.exit.subcell=@_exit_iced.cmd
remove #key.drc.macros
$ DRC data reloaded from file: %drc.file
