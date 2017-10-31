if(macro_exists(#drc.rules)==0) @drcmac.cmd

unselect all;
view on;
while(%n.select==0){
   if(macro_exists(#drc.error.layers)==0){
      error "No error layers for this rule set"
   }
   select tags=* layers %drc.error.layers near;
}
if(%n.select > 1){
   list global #xdrc.list
   unselect all
   select list #xdrc.list
   item global #xdrc.err
   default global #drc.tag.%xdrc.err.tag="No information available";
   shell menu=drc:tag_shell; $ TAG=%xdrc.err.tag:%drc.tag.%xdrc.err.tag;
   remove list #xdrc.list
}
if(%n.select==1){
   item global #xdrc.err;
   default global #drc.tag.%xdrc.err.tag="No information available";
   $ TAG=%xdrc.err.tag:%drc.tag.%xdrc.err.tag;
}
