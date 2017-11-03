!*****************************************************************
! BOOLOP.CMD uses the DRC to perform Boolean operations on boxes, 
! polygons, and (optionally) wires.
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
if(macro_exists(#bool.op)!=2) local #bool.op = $menu drc:bool_ops
!
! Bloat and shrink operations require some special handling -- flag them
! by setting bloat.op = 1
!
local #bloat.op = {cmp("%bool.op", SHRINK)==0 || cmp("%bool.op", BLOAT)==0};
if(%bloat.op){
   local #bloat.len = $menu drc:bloat_len
   if(cmp("%bloat.len", "%")==0){
      @_get_real; #prompt="bloat/shrink length"; &
                  #min =.001;
      #bloat.len = %ret.value;
   }
}
!
! Trickery to allow 1 $menu operation to set more than 1 parameter.
!
if(%bloat.op || cmp("%bool.op", NOT_A)==0){
   local #bool.op.layers = $menu drc:one_op_layer_names
   local #n.ops = 1;
   %bool.op.layers;
}
else{
   local #bool.op.layers = $menu drc:two_op_layer_names_1
   local #n.ops = 2;
   %bool.op.layers;
}
!
! Prompt for keyboard input when #layer.a="%", #layer.b="%", or 
! #layer.c="%" specified.  Force layer numbers (not names).
!
if(cmp("%layer.a", "%")==0){
   @_get_lay; #prompt = "layer name or number for first operand-A";
   #layer.a = %ret.value;
}
#layer.a = %layer.number.%layer.a;
  
if(%n.ops==2){
   if(cmp("%layer.b", "%")==0){
      @_get_lay; #prompt = "layer name or number for first operand-B";
      #layer.b = %ret.value;
   }
   #layer.b = %layer.number.%layer.b;
   if(%layer.a==%layer.b){
      error "Operand layers cannot be the same.  layer-A = layer-B = %layer.a"
   }
}
else local #layer.b = %layer.a;

if(cmp("%layer.c", "%")==0){
   @_get_lay; #prompt = "result layer name or number";
   #layer.c = %ret.value;
}
#layer.c = %layer.number.%layer.c;

unselect layers 0:255-%layer.a-%layer.b all
unselect lines all
unselect text all
while(%n.select==0){
   select layers %layer.a+%layer.b in
   unselect lines all
   unselect text all
}
local #n0.select = %n.select
unselect wires all
local #n.wires = {%n0.select - %n.select};
if(%n.wires!=0){
   undo
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
local #delete.input = $menu drc:delete_input
!
! Create file name
!
local #int.res = {int(1000*%bool.op.cut.res)}
local #rules.file.name = %tmp^$%bool.op^$%int.res
if(%bloat.op){
   local #int.bloat.len = {int(1000 * %bloat.len)}
   #rules.file.name = %rules.file.name^$%int.bloat.len
}
!
! If .BB file doesn't exist, create it.  This requires creating a .RUL 
! file and then compiling it to create the .BB file.
!
if(file_exists("%rules.file.name^.BB")==0){
   if(%bloat.op==0){
      dos '^mkrule BOOL "%rules.file.name^.RUL" %bool.op.cut.res %bool.op'
   }
   else{
      dos '^mkrule %bool.op "%rules.file.name^.RUL" &
           %bool.op.cut.res %bloat.len'
   }
   dos '^d3rules "%rules.file.name^.RUL" hog=2'
   if(file_exists("%rules.file.name^.BB")==0){
      error "Error compiling %rules.file.name^.RUL"
   }
}
drc select
global #dop.file.name = %tmp^drcop.out
dos '^del "%dop.file.name"'
local #t0 = %timer;
if(%n.ops==1){
   dos '^drc3 "%rules.file.name" %cell "%dop.file.name" &
      layers=(%layer.a %layer.c) quickpass hog=%bool.op.hog iced macros="dop"'
}
else if(%n.ops==2){
   dos '^drc3 "%rules.file.name" %cell "%dop.file.name" &
        layers=(%layer.a %layer.b %layer.c) quickpass hog=%bool.op.hog &
        iced macros="dop"'
}
if(file_exists("drccrash.log")){
   @drccrash.log
   global #dop.log.crash.message = %drccrash.message
   global #dop.log.crash.code = %drccrash.code
   if(%dop.log.crash.code==4){ ! Recreate then recompile rules file and retry
      if(%bloat.op==0){
         dos '^mkrule BOOL "%rules.file.name^.RUL" %bool.op.cut.res %bool.op'
      }
      else{
         dos '^mkrule %bool.op "%rules.file.name^.RUL" &
             %bool.op.cut.res %bloat.len'
      }
      dos '^d3rules "%rules.file.name^.RUL" hog=2'
      if(file_exists("%rules.file.name^.BB")==0){
         error "Error compiling %rules.file.name^.RUL"
      }
      local #t0 = %timer;
      if(%n.ops==1){
         dos '^drc3 "%rules.file.name" %cell "%dop.file.name" &
            layers=(%layer.a %layer.c) quickpass hog=%bool.op.hog &
            iced macros="dop"'
      }
      else if(%n.ops==2){
         dos '^drc3 "%rules.file.name" %cell "%dop.file.name" &
            layers=(%layer.a %layer.b %layer.c) quickpass hog=%bool.op.hog & 
            iced macros="dop"'
      }
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
