if(macro_exists(#op)!=2) local #op=next
if(cmp("%op", init)==0){
   if(list_exists(step.error.list)){
      if(%list.empty.step.error.list==0){
         @_get_ans; &
             #prompt="Do you want replace the existing error list[Yn]?"; &
             #choices="Yn";                                              &
             #default="Y";
         if(%ret.value==2){
            return; error "Command cancelled"
         }
      }
   }
   if(%n.select==0) select tag=* all
   view on
   pause 0.5
   list global #step.error.list
   #op = first
   view off
}
unselect all
if(%list.eol.step.error.list==1 && cmp("%op", "NEXT")==0) #op = first;
else if(%list.eol.step.error.list==-1 && cmp("%op", "PREV")==0) #op = last;
select list #step.error.list %op
if(%n.select==1){
   if(macro_exists(#drc.rules)==0) @drcmac.cmd
   item local #xdrc.err;
   default local #drc.tag.%xdrc.err.tag="No information available";
   $(%list.index.step.error.list/%list.len.step.error.list) &
     TAG=%xdrc.err.tag:%drc.tag.%xdrc.err.tag;
}
else{
   if(%list.empty.step.error.list){
      $ List empty -- all components deleted
   }
   else if(%list.eol.step.error.list==1){
      $ Passed end of list
   }
   else if(%list.eol.step.error.list==-1){
      $ Before start of list
   }
   else{ ! Should no longer happen
      $ Component protected or blanked
   }
}
