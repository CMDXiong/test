#!/bin/sh

EXEC_NAME=klayout
IS_MAC="no"
EXEC_HOLDER=""
OTHER_BIN=""

CURR_DIR=`pwd`
PLATFORM="linux-32-gcc-release"
SOURCE=$CURR_DIR/src
RUN_MAKE=1
GCC_MINGW=0
WIN_CYGWIN=0
HAVE_QTBINDINGS=0
HAVE_64BIT_COORD=0

case `uname` in
MINGW32*)
  PLATFORM="win-32-mingw-release"
  ;;
CYGWIN*)
  PLATFORM="win-32-mingw-release"
  # On cygwin, provide a windows path in SOURCE
  if ( cygpath -m $CURR_DIR >/dev/null 2>/dev/null ); then
    WIN_CYGWIN=1
    SOURCE=`cygpath -m $CURR_DIR`/src
    if ( gcc -mno-cygwin -MM -MG -x c++ $SOURCE/tlLog.cc -I$SOURCE >/dev/null 2>/dev/null ); then
      GCC_MINGW=1
    else
      echo "*** ERROR: detected cygwin build environment but with a gcc that is a native Windows MinGW compiler"
      exit 1
    fi
  else
    echo "*** ERROR: cannot run cygpath"
    exit 1
  fi
  ;;
Darwin*)
  PLATFORM="mac-yosemite-gcc-release"
  EXEC_HOLDER="klayout.app/Contents/MacOS"
  ;;
*)
  # Assume Linux for the remaining variants
  case `arch` in
  x86_64)
    PLATFORM="linux-64-gcc-release"
    ;;
  *)
    PLATFORM="linux-32-gcc-release"
    ;;
  esac
  ;;
esac

# Initialize the Qt paths
if [ "$QTDIR" != "" ]; then
  qtdir_wo_backslash=`echo "$QTDIR" | sed 's/\\\\/\\//g'`
  QTLIB="$qtdir_wo_backslash/lib"
  QTINCLUDE="$qtdir_wo_backslash/include"
  QTBIN="$qtdir_wo_backslash/bin"
else
  QTLIB="/usr/lib"
  QTINCLUDE="/usr/include/qt4"
  QTBIN="/usr/bin"
fi

RUBYINCLUDE=""
RUBYINCLUDE2=""
RUBYLIBFILE=""
RUBYVERSIONCODE=""

PYTHONINCLUDE=""
PYTHONLIBFILE=""

RUBY=""
PYTHON=""
BUILD=""
BIN=""

MAKE_OPT=""

while [ "$*" != "" ]; do
  a=$1
  shift

  case $a in
  -qt)
    QTLIB="$1/lib"
    QTBIN="$1/bin"
    QTINCLUDE="$1/include"
    shift
    ;;
  -with-qtbinding)
    HAVE_QTBINDINGS=1
    ;;
  -with-64bit-coord)
    HAVE_64BIT_COORD=1
    ;;
  -qtlib)
    QTLIB="$1"
    shift
    ;;
  -qtbin)
    QTBIN="$1"
    shift
    ;;
  -qtinc)
    QTINCLUDE="$1"
    shift
    ;;
  -platform)
    PLATFORM="$1"
    shift
    ;;
  -python)
    PYTHON="$1"
    shift
    ;;
  -nopython)
    PYTHON="-"
    ;;
  -pyinc)
    PYTHONINCLUDE="$1"
    shift
    ;;
  -pylib)
    PYTHONLIBFILE="$1"
    shift
    ;;
  -ruby)
    RUBY="$1"
    shift
    ;;
  -noruby)
    RUBY="-"
    ;;
  -rbvers)
    RUBYVERSIONCODE="$1"
    shift
    ;;
  -rbinc)
    RUBYINCLUDE="$1"
    shift
    ;;
  -rbinc2)
    RUBYINCLUDE2="$1"
    shift
    ;;
  -rblib)
    RUBYLIBFILE="$1"
    shift
    ;;
  -build)
    BUILD="$1"
    shift
    ;;
  -bin)
    BIN="$1"
    shift
    ;;
  -dry-run)
    RUN_MAKE=0
    ;;
  -option)
    MAKE_OPT="$MAKE_OPT $1"
    shift
    ;;
  -h|--help|-help)
    echo "usage: build [options]"
    echo ""
    echo "options:"
    echo "  -qt <dir>         Set Qt installation directory if Qt is installed in a"
    echo "                    common directory (-prefix path, default is '/usr/lib/qt4')."
    echo "                    This will set -qtbin to <dir>/bin, -qtlib to <dir>/lib"
    echo "                    and -qtinc to <dir>/include."
    echo "  -qtbin <dir>      Set Qt tools directory explicitly"
    echo "  -qtinc <dir>      Set Qt header directory explicitly"
    echo "  -qtlib <dir>      Set Qt library directory explicitly"
    echo "  -noruby           Don't build with Ruby support"
    echo "  -ruby <prog>      Use Ruby interpreter 'prog'"
    echo "  -nopython         Don't build with Python support"
    echo "  -python <prog>    Use Python interpreter 'prog'"
    echo "  -platform <key>   Select a platform"
    echo "  -build <path>     Directory where to do the build"
    echo "  -bin <path>       Directory where to install the binary"
    echo "  -option <option>  'make' options (i.e. -j2)"
    echo ""
    echo "  -with-qtbinding   Create Qt bindings for ruby scripts"
    echo "  -with-64bit-coord Use long (64bit) coordinates - EXPERIMENTAL FEATURE"
    echo "                    (only available for gcc>=4.4 for 64bit build)"
    echo ""
    echo "  -dry-run          Don't build"
    echo ""
    echo "Special options (normally determined from ruby installation):"
    echo "  -rblib <file>     Location of the .so/.dll to link for Ruby support"
    echo "  -rbinc <dir>      Location of the Ruby headers (in particular 'ruby.h')"
    echo "                    -rbinc and -rblib must be set to enable Ruby support"
    echo "  -rbinc2 <dir>     Second include path for Ruby 1.9 (containing 'ruby/config.h')"
    echo "  -rbvers <xyyzz>   Ruby version code"
    echo "  -pylib <file>     Location of the .so/.dll to link for Python support"
    echo "  -pyinc <dir>      Location of the Python headers (in particular 'Python.h')"
    echo ""
    echo "Available platforms are:"
    for pf in `ls -1 config/*.conf.*`; do
      echo $pf | sed "s/config\/Makefile\.conf\./  /"
    done
    exit 0
    ;;
  *)
    echo "*** ERROR: unknown option $a (use -help to show options)"
    exit 1
    ;;
  esac

done

echo "Scanning installation .."
echo ""

# if not given, locate ruby interpreter 
if [ "$RUBY" != "-" ]; then
  for ruby in "ruby2.0" "ruby1.9" "ruby" "ruby-mri" "ruby1.8"; do
    if [ "$RUBY" = "" ] && [ "`$ruby -v 2>/dev/null`" != "" ]; then 
      RUBY="$ruby" 
    fi
  done
fi
if [ "$RUBY" != "" ] && [ "$RUBY" != "-" ]; then

  echo "Using Ruby interpreter: $RUBY"

  if [ "`$RUBY -e 'puts 1' 2>/dev/null`" != "1" ]; then
    echo "*** ERROR: unable to run Ruby interpreter $RUBY"
    exit 1
  fi

  # Get ruby version
  if [ "$RUBYVERSIONCODE" = "" ]; then
    RUBYVERSIONCODE=`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['MAJOR'] || 0).to_i*10000+(RbConfig::CONFIG['MINOR'] || 0).to_i*100+(RbConfig::CONFIG['TEENY'] || 0).to_i"`
  fi
  echo "    Ruby version code is $RUBYVERSIONCODE"

  # Get ruby installation files
  if [ "$RUBYLIBFILE" = "" ]; then
    RUBYLIBFILENAME_SO=`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['LIBRUBY_SO'] || '')"`
    RUBYLIBFILENAME_A=`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['LIBRUBY_A'] || '')"`
    if [ "$RUBYLIBFILENAME_A" = "" ] && [ "$RUBYLIBFILENAME_SO" = "" ]; then
      echo "*** WARNING: Could not get Ruby library name"
    else
      RUBYLIBFILEPATH=`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['archlibdir'] || '')"`
      if [ "$RUBYLIBFILEPATH" = "" ] || [ ! -e "$RUBYLIBFILEPATH" ]; then
        RUBYLIBFILEPATH=`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['libdir'] || '')"`
      fi
      if [ "$RUBYLIBFILEPATH" = "" ]; then
        echo "*** WARNING: Could not get Ruby library path"
      elif [ -f "$RUBYLIBFILEPATH/$RUBYLIBFILENAME_SO" ]; then
        RUBYLIBFILE="$RUBYLIBFILEPATH/$RUBYLIBFILENAME_SO"
        echo "    Ruby library found: $RUBYLIBFILE"
      elif [ -f "$RUBYLIBFILEPATH/$RUBYLIBFILENAME_A" ]; then
        RUBYLIBFILE="$RUBYLIBFILEPATH/$RUBYLIBFILENAME_A"
        echo "    Ruby library found: $RUBYLIBFILE"
      else
        echo "*** WARNING: Could not locate Ruby library"
        echo "    Candidates are:"
        echo "      $RUBYLIBFILEPATH/$RUBYLIBFILENAME_SO"
        echo "      $RUBYLIBFILEPATH/$RUBYLIBFILENAME_A"
        echo "    (none of them could be found)"
      fi
    fi
  fi

  if [ "$RUBYLIBFILE" != "" ]; then
    RUBYHDRDIR=`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['rubyhdrdir'] || '')"`
    if [ "$RUBYHDRDIR" = "" ]; then
      if [ "$RUBYINCLUDE" = "" ]; then
        RUBYINCLUDE=`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['archdir'] || '')"`
      fi
      echo "    Ruby headers found: $RUBYINCLUDE"
    else
      if [ "$RUBYINCLUDE" = "" ]; then
        RUBYINCLUDE="$RUBYHDRDIR"
      fi
      if [ "$RUBYINCLUDE2" = "" ]; then
        RUBYINCLUDE2=`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['rubyarchhdrdir'] || '')"`
      fi
      if [ "$RUBYINCLUDE2" = "" ] || [ ! -e "$RUBYINCLUDE2" ]; then
        RUBYINCLUDE2="$RUBYHDRDIR"/`$RUBY -rrbconfig -e "puts (RbConfig::CONFIG['arch'] || '')"`        
      fi
      echo "    Ruby headers found: $RUBYINCLUDE and $RUBYINCLUDE2"
    fi
  fi

  echo "    Ruby installation is in:"
  echo "    - $RUBYLIBFILE (lib)"
  echo "    - $RUBYINCLUDE (includes)"
  echo "    - $RUBYINCLUDE2 (config for 1.9.x)"
  echo ""

fi

# if not given, locate Python interpreter 
if [ "$PYTHON" != "-" ]; then
  for python in "python"; do
    if [ "$PYTHON" = "" ] && [ "`$python -V 2>&1`" != "" ]; then 
      PYTHON="$python" 
    fi
  done
fi
if [ "$PYTHON" != "" ] && [ "$PYTHON" != "-" ]; then

  echo "Using Python interpreter: $PYTHON"

  if [ "`$PYTHON -c 'print(1)' 2>/dev/null`" != "1" ]; then
    echo "*** ERROR: unable to run Python interpreter $PYTHON"
    exit 1
  fi

  # Get Python installation files
  if [ "$PYTHONLIBFILE" = "" ]; then
    PYTHONLIBFILENAME=`$PYTHON -c "import sysconfig; print(sysconfig.get_config_vars('LDLIBRARY')[0])" 2>/dev/null`
    if [ "$PYTHONLIBFILENAME" = "" ]; then
      PYTHONLIBFILENAME=`$PYTHON -c "import sysconfig; print(sysconfig.get_config_vars('LIBRARY')[0])" 2>/dev/null`
    fi
    if [ "$PYTHONLIBFILENAME" = "" ]; then
      PYTHONLIBFILENAME=`$PYTHON -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_vars('LDLIBRARY')[0])" 2>/dev/null`
    fi
    if [ "$PYTHONLIBFILENAME" = "" ]; then
      PYTHONLIBFILENAME=`$PYTHON -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_vars('LIBRARY')[0])" 2>/dev/null`
    fi
    if [ "$PYTHONLIBFILENAME" = "" ]; then
      echo "*** WARNING: Could not get Python library name"
    else
      PYTHONLIBFILEPATH=`$PYTHON -c "import sysconfig; print(sysconfig.get_config_vars('LIBDIR')[0])" 2>/dev/null`
      if [ "$PYTHONLIBFILEPATH" = "" ]; then
        PYTHONLIBFILEPATH=`$PYTHON -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_vars('LIBDIR')[0])" 2>/dev/null`
      fi
      if [ "$PYTHONLIBFILEPATH" = "" ]; then
        echo "*** WARNING: Could not get Python library path"
      else
        PYTHONLIBFILE="$PYTHONLIBFILEPATH/$PYTHONLIBFILENAME"
        if [ ! -f "$PYTHONLIBFILE" ]; then
          echo "    INFO: Python library not in default path, trying to use MULTIARCH"
          PYTHONMULTIARCH=`$PYTHON -c "import sysconfig; print(sysconfig.get_config_vars('MULTIARCH')[0])" 2>/dev/null`
          PYTHONLIBFILE="$PYTHONLIBFILEPATH/$PYTHONMULTIARCH/$PYTHONLIBFILENAME"
        fi
        echo "    Python library found: $PYTHONLIBFILE"
      fi
    fi
  fi

  if [ "$PYTHONINCLUDE" = "" ]; then
    PYTHONINCLUDE=`$PYTHON -c "import sysconfig; print(sysconfig.get_config_vars('INCLUDEPY')[0])" 2>/dev/null`
    if [ "$PYTHONINCLUDE" = "" ]; then
      PYTHONINCLUDE=`$PYTHON -c "import distutils.sysconfig; print(distutils.sysconfig.get_config_vars('INCLUDEPY')[0])" 2>/dev/null`
    fi
    echo "    Python headers found: $PYTHONINCLUDE"
  fi

  echo "    Python installation is in:"
  echo "    - $PYTHONLIBFILE (lib)"
  echo "    - $PYTHONINCLUDE (includes)"
  echo ""

fi

echo "Qt installation directory is $QTLIB (lib), $QTBIN (tools) and $QTINCLUDE (includes)"
if [ $HAVE_QTBINDINGS != 0 ]; then
  echo "    Qt bindings enabled"
fi
echo ""

echo "Platform is $PLATFORM"
if [ $HAVE_64BIT_COORD != 0 ]; then
  echo "    64 bit coordinates enabled"
fi

gcc_path=`type gcc 2>/dev/null`
if [ "$gcc_path" = "" ]; then
  echo "*** ERROR: unable to locate gcc in path"
  exit 1
fi

if [ ! -r config/Makefile.conf.$PLATFORM ]; then
  echo "*** ERROR: unknown platform $PLATFORM"
  exit 1
fi

case $PLATFORM in
mac-*)
  IS_MAC="yes"
  echo "*** WARNING: no check done of Qt-headers."
  ;;
*)
  if [ ! -r $QTINCLUDE/QtCore/Qt ]; then
    echo "*** ERROR: no Qt headers in $QTINCLUDE"
    exit 1
  fi
  ;;
esac

if [ ! -r $QTBIN/moc ]; then
  echo "*** ERROR: unable to locate Qt MOC in $QTBIN"
  exit 1
fi
if [ ! -r $QTBIN/uic ]; then
  echo "*** ERROR: unable to locate Qt UIC in $QTBIN"
  exit 1
fi

if [ "$RUBYINCLUDE" != "" ]; then
  HAVE_RUBY=1
  if [ "$RUBYLIBFILE" = "" ]; then
    echo "*** ERROR: -rblib not specified or library path cannot be determined from Ruby installation"
    exit 1
  fi
  if [ "$RUBYVERSIONCODE" = "" ]; then
    echo "*** ERROR: -rbvers not specified or version cannot be determined from Ruby installation"
    exit 1
  fi
  if [ ! -f "$RUBYINCLUDE/ruby.h" ]; then
    echo "*** ERROR: wrong -rbinc path: $RUBYINCLUDE/ruby.h does not exist or is not a file"
    exit 1
  fi
  if [ "$RUBYINCLUDE2" != "" ] && [ ! -f "$RUBYINCLUDE2/ruby/config.h" ]; then
    echo "*** ERROR: wrong -rbinc2 path: $RUBYINCLUDE2/ruby/config.h does not exist or is not a file"
    exit 1
  fi
  if [ ! -f "$RUBYLIBFILE" ]; then
    echo "*** ERROR: wrong -rblib path: $RUBYLIBFILE does not exist or not a file"
    exit 1
  fi
else
  HAVE_RUBY=0
fi

if [ "$PYTHONINCLUDE" != "" ]; then
  HAVE_PYTHON=1
  if [ "$PYTHONLIBFILE" = "" ]; then
    echo "*** ERROR: -pylib not specified or library path cannot be determined from Python installation"
    exit 1
  fi
  if [ ! -f "$PYTHONINCLUDE/Python.h" ]; then
    echo "*** ERROR: wrong -pyinc path: $PYTHONINCLUDE/Python does not exist or is not a file"
    exit 1
  fi
  if [ ! -f "$PYTHONLIBFILE" ]; then
    echo "*** ERROR: wrong -pylib path: $PYTHONLIBFILE does not exist or not a file"
    exit 1
  fi
else
  HAVE_PYTHON=0
fi

if [ "$BUILD" = "" ]; then
  BUILD=$CURR_DIR/build.$PLATFORM
fi
if [ "$BIN" = "" ]; then
  BIN=$CURR_DIR/bin.$PLATFORM
else
  mkdir -p $BIN
  cd $BIN
  BIN=`pwd`
  cd $CURR_DIR
fi

if ( gmake -v >/dev/null 2>/dev/null ); then
  MAKE_PRG=gmake
else
  MAKE_PRG=make
fi

mkdir -p $BUILD
echo "WIN_CYGWIN=$WIN_CYGWIN"  >$BUILD/Makefile
echo "EXEC_NAME=$EXEC_NAME"   >>$BUILD/Makefile
echo "OTHER_BIN=$OTHER_BIN"   >>$BUILD/Makefile
echo "INSTALL_BINDIR=$BIN"    >>$BUILD/Makefile
cat   src/Makefile.top        >>$BUILD/Makefile
cp -f config/Makefile.conf.$PLATFORM $BUILD/Makefile.conf

mkdir -p $BUILD/main
echo "TOP_SOURCE=$SOURCE" >$BUILD/main/Makefile
echo "SOURCE=$SOURCE" >>$BUILD/main/Makefile
echo 'default: all' >>$BUILD/main/Makefile
echo 'include ../Makefile.env' >>$BUILD/main/Makefile
echo 'include ../Makefile.conf' >>$BUILD/main/Makefile
echo 'include $(SOURCE)/Makefile.body' >>$BUILD/main/Makefile

PLUGINS=""
if [ -d src ] && [ -d src/plugins ]; then
  cd src/plugins
  PLUGINS=`echo *`
  cd ../..
  for plugin in $PLUGINS; do
    mkdir -p $BUILD/$plugin
    echo "TOP_SOURCE=$SOURCE" >$BUILD/$plugin/Makefile
    echo "SOURCE=$SOURCE/plugins/$plugin" >>$BUILD/$plugin/Makefile
    echo 'default: all' >>$BUILD/$plugin/Makefile
    echo 'include ../Makefile.env' >>$BUILD/$plugin/Makefile
    echo 'include ../Makefile.conf' >>$BUILD/$plugin/Makefile
    echo 'include $(SOURCE)/Makefile.body' >>$BUILD/$plugin/Makefile
  done
fi

echo "QTBIN=$QTBIN" >$BUILD/Makefile.env
echo "QTINCLUDE=$QTINCLUDE" >>$BUILD/Makefile.env
echo "QTLIB=$QTLIB" >>$BUILD/Makefile.env
echo "PLUGINS=$PLUGINS" >>$BUILD/Makefile.env
echo "RUBYLIBFILE=$RUBYLIBFILE" >>$BUILD/Makefile.env
echo "RUBYINCLUDE=$RUBYINCLUDE" >>$BUILD/Makefile.env
echo "RUBYINCLUDE2=$RUBYINCLUDE2" >>$BUILD/Makefile.env
echo "RUBYVERSIONCODE=$RUBYVERSIONCODE" >>$BUILD/Makefile.env
echo "HAVE_RUBY=$HAVE_RUBY" >>$BUILD/Makefile.env
echo "PYTHONLIBFILE=$PYTHONLIBFILE" >>$BUILD/Makefile.env
echo "PYTHONINCLUDE=$PYTHONINCLUDE" >>$BUILD/Makefile.env
echo "HAVE_PYTHON=$HAVE_PYTHON" >>$BUILD/Makefile.env
echo "HAVE_QTBINDINGS=$HAVE_QTBINDINGS" >>$BUILD/Makefile.env
echo "HAVE_64BIT_COORD=$HAVE_64BIT_COORD" >>$BUILD/Makefile.env

echo "Building plugins: $PLUGINS"
echo ""

if [ $RUN_MAKE = 0 ]; then
  exit 0
fi

# -- Running build  --

echo "Running build .."
echo "$MAKE_PRG"
cd $BUILD
$MAKE_PRG $MAKE_OPT all || exit 1
echo ""

cd $CURR_DIR

# -- Installing binaries  --

echo "Installing binaries .."
cd $BUILD
$MAKE_PRG install
cd $CURR_DIR
echo "Build done."

if [ $WIN_CYGWIN = 0 ]; then
  if [ "$IS_MAC" = "yes" ]; then
  	EXEC_FULL_PATH=$BIN/$EXEC_HOLDER/$EXEC_NAME
  else
  	EXEC_FULL_PATH=$BIN/$EXEC_NAME
  fi
  echo ""
  echo "Final binary is ready in $EXEC_FULL_PATH."
  echo "To run it, you may need to add the following to your LD_LIBRARY_PATH:"
  echo "$QTLIB"

  LD_LIBRARY_PATH=$QTLIB:$LD_LIBRARY_PATH
  $EXEC_FULL_PATH -v
  $EXEC_FULL_PATH -h
fi

exit 0

