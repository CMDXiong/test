local pos = NULL;
unselect push;

select layers 1:* near
if(%n.select==0){
   #pos = %last.pos;
   pedit view_only=true near %pos;
   #pos = %last.pos;
   unselect push;
   select near %pos;
   show file=*;
   select pop;
   quit;
}
else show file=*;
unselect all
select pop
