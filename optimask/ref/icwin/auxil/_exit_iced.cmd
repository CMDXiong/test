local #error.cmd=exit now
if(macro_exists(#drc.root)!=0){
   if(cmp(%drc.root, %cell)==0){
      global #key.drc.macros = "@%cell^.drc; remove #key.drc.macros"
      show user #drc.* file=%cell^.drc
      show user #iced.exit.* append
   }
}
