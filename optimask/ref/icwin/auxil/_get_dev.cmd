remove ret.value
local #valid=0
local #value=""

if(macro_exists(#default)==2){
   if(device_exists("%default")){
      local #prompt="Enter device or port name [%default]:"
   }
   else remove #default
}

if(macro_exists(#prompt)!=2){
   local #prompt="Enter device or port name (e.g. \\share\name or lpt1):"
}

! Prompt user for real

while(%valid==0){
   #value=$PROMPT="%prompt"

   if(cmp("%value", "")==0 && (macro_exists(#default)==2)) #value = %default;
   #valid = {device_exists("%value")}
   if(%valid==0){
      $Invalid input (%value) -- Device does not exist
      pause 0;
   }
}
global #ret.value=%value
