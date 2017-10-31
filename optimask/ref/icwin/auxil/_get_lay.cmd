remove ret.value
local #valid=0
local #value=""
default local #prompt="layer"

! Add default value to prompt

if(macro_exists(#default)==2) #prompt="Enter %prompt [%default]:"
else                          #prompt="Enter %prompt:"

! Insist that accept.layer.0 passed as an argument or use "no"

if(macro_exists(#accept.layer.0) < 2) local #accept.layer.0 = 0

! Prompt user for layer

while(%valid==0){
   #value=$PROMPT="%prompt"
   if(cmp("%value", "")==0 && (macro_exists(#default)==2)){
      #value = %default;
   }
   #valid = {valid_layer("%value")}
   if(%valid){
      #value=%layer.number.%value
      if(%value==0 && %accept.layer.0==0){
         #valid=0
         $ Cannot accept layer 0
         pause 0
      }
   }
   else{
      $Invalid layer name or number (%value)
      pause 0;
   }
}
global #ret.value=%value
$ %ret.value
