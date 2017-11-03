log level=brief screen=off;

while(1){
   unselect cell * all
   unselect text all
   unselect parts
   if(%n.select!=0) skip ok
   select near
}
ok:
default local #angle  = $prompt="Enter ccw rotation angle: "
default local #origin = $prompt="Digitize pivot point" POS

local #c = {cos(%angle)}
local #s = {sin(%angle)}

remove #ed.cmd    ! Patch for obsolete .CMD files -- Do not remove
remove #uned.cmd  ! Patch for obsolete .CMD files -- Do not remove

global #ed.file.name = "uninitialized"
global #uned.file.name = %tmp^unwork.cmd 
global #ed.id0 = %id.max

show file=%uned.file.name

list local #tmp
unselect all
while(1){
   select list #tmp next
   if(%n.select==0) skip_to done;
   item local #tmp.item box=poly
   local #n=1;
   while(%n <= %tmp.item.n.points){
      local #pt = {%tmp.item.pos.%n - %origin}
      local #x = x(%pt);
      local #y = y(%pt);
      #tmp.item.pos.%n = {((%c*%x-%s*%y), (%c*%y+%s*%x)) + %origin}
      #n={%n+1}
   }
   %add.tmp.item
   delete
}
done:

global #ed.id1=%id.max
global #uned.enabled = 1;
$ uned.enabled=%uned.enabled

