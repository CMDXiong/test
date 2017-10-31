unsel all; 
sel list show.one.list; 
if(%list.eol.show.one.list!=0){
   menu *:show1a
   $ At end of list
}
else{
   item global #show.one.item; 
   #show.one.item.layer = %layer.name.%show.one.item.layer;
   $ %add.show.one.item  
}

