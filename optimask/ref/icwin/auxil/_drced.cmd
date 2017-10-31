default local #editor="-notepad"
if(file_exists("%drc.rules^%ext")) spawn %editor %drc.rules^%ext
else{
   error "Could not find %drc.rules^%ext"
}