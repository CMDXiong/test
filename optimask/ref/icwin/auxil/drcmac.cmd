! Reload drc.* macros.  When you exit a cell most macros, except those 
! starting with "KEY.", are lost.  Hopefully, the drc.* macros were saved
! in a file, cell_name.DRC, and the macro KEY.DRC.MACROS evaluates as 
! "@cell_name.DRC; remove #key.drc.macros".  
!
! If if the macro KEY.DRC.MACROS does not exist, and the file cell_name.DRC 
! does exist, it probably contains the drc.* macros.  If it doesn't exist, 
! the macros required by DRCTAG.CMD can be loaded from the .TAG file generated 
! by the DRC's rules compiler.
! 
if(macro_exists(#key.drc.macros)){
   %key.drc.macros;
}
else{
   if(file_exists(%cell^.drc)){
      @_get_ans; #prompt=                                                   &
      "Load DRC macros from D=>.DRC file, T=>.TAG file, or Q=>Quit [Dtq]?"; &
      #choices="TQD";                                                       &
      #default="D";
   }
   else{
      @_get_ans; #prompt=                                      &
         "Load DRC macros from T=>.TAG file or Q=>Quit [Tq]?"; &
         #choices="TQ";                                        &
         #default="T";
   }
   if(%ret.value==3) @%cell^.DRC
   else if(%ret.value==1){
      global #drc.rules=$menu=drc:tag_list
      @"%drc.rules^.tag"
   }
   else{
      error "Command cancelled"
   }
}
!
! Set up for saving the drc.* macros when you EXIT a cell
!
global #iced.exit.root=@_exit_iced.cmd
global #iced.exit.subcell=@_exit_iced.cmd
