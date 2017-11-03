xselect off;
while(%n.select==0){
   select near;
}
if(%n.select > 1){
   list global #show.one.list
   unselect all
   select list #show.one.list
   item global #show.one.item
   #show.one.item.layer = %layer.name.%show.one.item.layer;
   shell menu=*:show1; $ %add.show.one.item
   remove list #show.one.list
}
if(%n.select==1){
   item global #show.one.item
   #show.one.item.layer = %layer.name.%show.one.item.layer;
   $ %add.show.one.item
}
