remove ret.value
local #valid=0
local #value=""
default local #prompt="integer"

! Add default value to prompt

if(macro_exists(#default)==2) #prompt="Enter %prompt [%default]:"
else                          #prompt="Enter %prompt:"

! Insist that min and max were passed as arguments or use "infinity"

if(macro_exists(#min) < 2) local #min=-2000000000
if(macro_exists(#max) < 2) local #max=2000000000

! Prompt user for integer

while(%valid==0){
   #value=$PROMPT="%prompt"
   if(cmp("%value", "")==0 && (macro_exists(#default)==2)){
      #value = %default;
   }
   #valid = {valid_int("%value")}
   if(%valid){
      if((%min > %value) || (%max < %value)){
         #valid=0
         $ Value (%value) out of range [%min:%max]
         pause 0
      }
   }
   else{
      $Invalid input (%value)
      pause 0;
   }
}
global #ret.value=%value
