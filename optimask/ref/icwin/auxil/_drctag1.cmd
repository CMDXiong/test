if(macro_exists(#op)!=2) local #op=next
unsel all; 
if(%list.eol.xdrc.list==1 && cmp("%op", "NEXT")==0) #op = first;
else if(%list.eol.xdrc.list==-1 && cmp("%op", "PREV")==0) #op = last;
select list #xdrc.list %op
if(%n.select!=0){
   item local #xdrc.err; 
   if(%n.select!=0){
      default global #drc.tag.%xdrc.err.tag="No information available";
      $ TAG=%xdrc.err.tag:%drc.tag.%xdrc.err.tag;
   }
}
else{
   if(%list.empty.xdrc.list){
      $ List empty -- all components deleted
   }
   else if(%list.eol.xdrc.list==1){
      $ Passed end of list
   }
   else if(%list.eol.xdrc.list==-1){
      $ Before start of list
   }
}
