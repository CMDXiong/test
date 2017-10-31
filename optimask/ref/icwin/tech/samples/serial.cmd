! This command file places serial numbers on an each cell in an array.
! You can watch it work by editing the sample cell: stest_array.
!
! The font used to label the array elements was taken from Steve Stern's 
! PGTEXT routines.
!
!******************** Parameters are for cell STEST *********************
!
local #mag.stest = 5.0       ! Font magnification = text height in microns
local #offset.stest = (1, 1) ! Offset of lower left corner of text in cell 
                             ! relative to cell origin
local #layer.stest = 1       ! Text layer
local #n.digits.stest = 3    ! Number of digits in serial number
!
!******** Use text editor to add parameters for additional cells ********
!
!
!********************* Select array to be serialized *******************
!
local #n.select.pop = %n.select
if(%n.select!=0) unselect push
view on
while(%n.select != 1){
   unselect all
   prompt = "Select array to be serialized"
   select array * at
}
item local #ser
unsel all
view off
if(CMP("%ser.trans", "R0")!=0){
   return; $SERIAL.CMD cannot handle rotated or mirroed arrays
}
local #name=%ser.cell.name
!
!******************** Make sure parameter are defined *******************
!
if(macro_exists(#mag.%name)==0){
   return; $ MAG.%name undefined.  You must "text edit" SERIAL.CMD
}
if(macro_exists(#offset.%name)==0){
   return; $ OFFSET.%name undefined.  You must "text edit" SERIAL.CMD
}
if(macro_exists(#layer.%name)==0){
   return; $ LAYER.%name undefined.  You must "text edit" SERIAL.CMD
}
if(macro_exists(#n.digits.%name)==0){
   return; $ N.DIGITS.%name undefined.  You must "text edit" SERIAL.CMD
}
@_get_int; local #min=0; local #default=1; local #prompt="initial serial number"
local #n.serial = %ret.value
!
! Setup for UNED.CMD -- Part I
!
global #ed.id0 = %id.max
global #ed.file.name = %tmp^serial.out
global #uned.file.name = "do nothing"
!
!***************** Serialize cells in selected array ******************
!
test timer
local #off0 = {%ser.pos.1 + %offset.%name}
local #row = {%ser.n.rows - 1}
local #col = 0
local #row.off = "undefined"
local #off = "undefined"
local #x.off = "undefined"
local #digit = "undefined"
local #rem = "undefined"
local #q = "undefined"
local #char = "undefined"
local #max.serial = {%n.serial + (%ser.n.cols * %ser.n.rows) - 1}

log screen=off level=normal
while(%row >= 0){
   #row.off = {%off0 + (0, %ser.row.step * %row)}
   #col = 0
   while(%col < %ser.n.cols){
      #off = {%row.off + (%ser.col.step * %col, 0)}
      #col = {%col + 1}
      #digit = 1
      #rem = %n.serial
      $ n.serial = %n.serial / %max.serial
      while(%digit <= %n.digits.%name){
         #q = {int(%rem / 10)}
         #char = {%rem - 10 * %q}
         #rem = %q
         #x.off = {%mag.%name * .9 * (%n.digits.%name - %digit)}  ! For this 
                         ! font the character width is .9 * character height 
         @_char%char; &
             local #layer=%layer.%name; &
             local #fmag=%mag.%name; &
             local #off={%off + (%x.off, 0)}
         #digit = {%digit + 1}
      }
      #n.serial = {%n.serial + 1}
   }
   #row = {%row - 1}
}
log screen=on level=normal;
!
! Setup for UNED.CMD -- Part II
!
global #ed.id1 = %id.max
select ids after %ed.id0
unselect ids after %ed.id1
show file=%ed.file.name
! 
! Restore initial select state
!
if(%n.select.pop!=0) unselect pop
else unselect all

test timer
