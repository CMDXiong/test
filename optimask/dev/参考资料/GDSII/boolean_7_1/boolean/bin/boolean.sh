#! /bin/sh
PATH=/opt/protools/boolean/6.6/boolean/apps/edit:$PATH

GDSROOT=/opt/protools/boolean/6.6/boolean/apps/edit
export GDSROOT

echo "boolean operation program"
echo boolean : $GDSROOT

if test ! -d ${HOME}/gdsuser ; then 
   echo copying gdsuser to your HOME directory, It can be used for personal settings
   cp -r $GDSROOT/gdsuser $HOME
else
#    if test ! -f  ${HOME}/gdsuser/6.6_version ; then
#        echo copying gdsuser to your HOME directory because the one there is too old
#        rm -r ${HOME}/gdsuser
#        cp -r $GDSROOT/gdsuser $HOME
#    fi
    if test ! -f  ${HOME}/gdsuser/6.6_version ; then
        echo "The files in your $HOME/gdsuser directory are not recent enough."
        echo "Please remove this directory and run this program again."
        echo "A new set of files will be installed to $HOME/gdsuer."   
    fi    
fi

edit $1 $2


