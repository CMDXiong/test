!*****************************************************************
! MASKOP.CMD is based on BOOLOP.CMD.  It uses the DRC to perform 
! cut operations on boxes, polygons, and (optionally) wires.
!*****************************************************************
!
! You might want to change the following HOG value:
!  
default local #bool.op.hog = 8 ! DRC will use 8 Meg of RAM
!
! For now, we just set the DRC's cut_resolution to twice ICED's 
! resolution step.  We may revise this at a later date.
!
default local #bool.op.cut.res = {2*%res.step}
!
! #bool.op can be passed as an argument or set using the menus
!
if(macro_exists(#bool.op)!=2) local #bool.op = $menu drc:mask_ops
!
default local #mask_layer = $menu drc:mask_op_layer_names
!
! Prompt for keyboard input when #mask_layer="%".  
! Force layer numbers (not names).
!
if(cmp("%mask_layer", "%")==0){
   @_get_lay; #prompt = "mask layer name or number";
   #mask_layer = %ret.value;
}
#mask_layer = %layer.number.%mask_layer;
  
unselect cell * all
unselect lines all
unselect text all
while(%n.select==0){
   select layers 1:255 in
   unselect lines all
   unselect text all
}
local #n0.select = %n.select
unselect wires all
local #n.wires = {%n0.select - %n.select};
if(%n.wires!=0){
   undo
   select push
   unselect polygons all
   unselect text all
   $ "%n.wires wire(s) selected.  Convert to polygon(s)?"
   local #convert = $menu drc:convert_wires;
   if(%%convert==1){ ! Yes
      select pop
   }
   else if(%%convert==2){ ! No
      select pop
      unselect wires all
      if(%n.select==0){
         undo
         error "Nothing selected"
      }
   }
   else{ ! %%convert==3 => cancel
      error "Command cancelled"
   }
}
local #delete.input = 1;
!
! Create file name
!
local #int.res = {int(1000*%bool.op.cut.res)}
local #rules.file.name = %tmp^%bool.op^$%int.res^$%mask_layer
!
! If .BB file doesn't exist, create it.  This requires creating a .RUL 
! file and then compiling it to create the .BB file.
!
if(file_exists("%rules.file.name^.BB")==0){
   dos '^mkrule MASK "%rules.file.name^.RUL" %bool.op.cut.res %mask_layer &
      %bool.op'
   dos '^d3rules "%rules.file.name^.RUL" hog=2'
   if(file_exists("%rules.file.name^.BB")==0){
      error "Error compiling %rules.file.name^.RUL"
   }
}
drc select
global #dop.file.name = %tmp^drcop.out
dos '^del "%dop.file.name"'
local #t0 = %timer;
dos '^drc3 "%rules.file.name" %cell "%dop.file.name" &
    quickpass hog=%bool.op.hog iced macros="dop"'
if(file_exists("drccrash.log")){
   @drccrash.log
   global #dop.log.crash.message = %drccrash.message
   global #dop.log.crash.code = %drccrash.code
   if(%dop.log.crash.code==4){ ! Recreate then recompile rules file and retry
      dos '^mkrule MASK "%rules.file.name^.RUL" %bool.op.cut.res &
           %mask_layer %bool.op'
      dos '^d3rules "%rules.file.name^.RUL" hog=2'
      if(file_exists("%rules.file.name^.BB")==0){
         error "Error compiling %rules.file.name^.RUL"
      }
      local #t0 = %timer;
      dos '^drc3 "%rules.file.name" %cell "%dop.file.name" &
         quickpass hog=%bool.op.hog iced macros="dop"'
   }
   if(file_exists(drccrash.log)){
      if(macro_exists(drccrash.log.file)==0) remove #dop.log.file
      else              global #dop.log.file = %drccrash.log.file
      remove drccrash.*
      error "DRC CRASHED -- %dop.log.crash.message";
   }
}
local #t1 = %timer;
global #dop.id0 = %id.max;
global #undop.file.name = "Do Nothing";
global #undop.enabled = 1;
local #cmd.failed = 0;
local #error.cmd = #cmd.failed = 1;
@"%dop.file.name"
if(%cmd.failed==1){
   !
   ! The only known ways of getting here are if the .CMD files are 
   ! nested more than 32 deep or there is a bug in the DRC.
   !
   unselect push;
   #undop.enabled = 0;
   select ids after %dop.id0;
   if(%n.select!=0) delete
   select pop
   error 
}
global #dop.id1 = %id.max;
!
! Delete input components
!
if(%delete.input){
   select parts all
   show file=%tmp^undop.cmd
   #undop.file.name = %tmp^undop.cmd
   delete
}
$ DRC run time={%t1-%t0}
