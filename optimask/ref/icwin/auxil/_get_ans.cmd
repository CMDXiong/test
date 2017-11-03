! EXAMPLE OF CALLING SEQUENCE
!
! @_get_ans; &
!     #prompt="Do you want to continue [Yn]?"; &
!     #choices="Yn";                           &
!     #default="Y";
!
! #prompt and #choices must be defined.
!
! Defining #default is optional.  If #default is defined, we suggest showing
! the default value as the only capital letter in #choices and the [choices]
! part of #prompt.  If #default is not defined we suggest using all capital 
! letters in these places.
!
! %ret.value will be 1 for Y, y or <Enter> and 2 for N or n
!
view on
remove ret.value
local #value_no=0
local #value=""
local #len={len("%choices")}
local #chr=0
local #i=0

while(1){
   #value=$PROMPT="%prompt"
   #i=0
   if(cmp("%value", "")==0 && (macro_exists(#default)==2)) #value = %default;
   while(%i<%len){
      #i = {%i+1};
      #chr = {char(%i, "%choices")}
      if(cmp("%chr", "%value")==0){
         global #ret.value=%i;
         return; ! **************** Only Loop Exit ********************
      }
   }
   $ Invalid input: '%value'  Valid inputs are any of the characters in '%choices' 
   pause 0;
}
