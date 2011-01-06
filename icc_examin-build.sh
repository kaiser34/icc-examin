#/bin/sh

if [ $# -gt 1 ] ; then
  if [ $1 = "-h" ] || [ $1 = "--help" ] || [ $1 = "-?" ]; then
    echo ""
    echo "$0 help:"
    echo "any argument is passed to the configure scripts"
    echo "e.g. --prefix=/usr => ./configure --prefix=/usr"
    echo ""
    exit 0
  fi
fi

top="`pwd`"
stop_build=0
target=iccexamin


### Testing the system ###

# get host 64-bit capabilities
echo \
"#include <stdio.h>

int main(int argc, char**argv)
{
  fprintf(stdout, \"%d\", (int)sizeof(int*));
  return 0;
}" > ptr_size.c
gcc -Wall -g ptr_size.c -o ptr-size
BARCH=""
INTPTR_SIZE=`./ptr-size`
  if [ $INTPTR_SIZE -eq 4 ]; then
    echo_="32-bit build            detected"
  elif [ $INTPTR_SIZE -gt 4 ]; then
    echo_="64-bit build            detected"
    BARCH=64
    test -n "$ECHO" && $ECHO "BUILD_64 = 1" >> $CONF
  elif [ $INTPTR_SIZE -ne 0 ]; then
    echo_="$INTPTR_SIZE-byte intptr_t          detected"
  else
    echo_="CPU bus width not         detected"
  fi
echo "$echo_"
LIB=lib$BARCH

# get processor count
echo \
"#include <stdio.h>
#include <omp.h>

int main(int argc, char**argv)
{
  int i;
  #pragma omp parallel
  for(i = 0; i < 2; ++i) ;
  fprintf(stdout, \"%d\", omp_get_num_procs());
  return 0;
}" > omp.c
gcc -fopenmp -Wall -g omp.c -o processors
UNAME_=`uname`
if [ $UNAME_ = "Darwin" ]; then
  MD5SUM=md5
  SHA1SUM=shasum
  OSX_ARCH="-arch `uname -p`"
else
  MD5SUM=md5sum
  if [ "`which sha1`" != "" ]; then
    SHA1SUM=sha1
  else
    SHA1SUM=sha1sum
  fi
  NON_OSX_SHARED=--enable-shared
fi

cpus=`./processors`
if [ -n "$cpus" ]; then
  MAKE_CPUS="-j $cpus"
  echo will add MAKE_CPUS=$MAKE_CPUS
fi

prefix=/usr/local
switch=prefix
if [ "`echo \"$1\" | sed s/\"--$switch=\"//`" != "$1" ]; then
  prefix="`echo \"$1\" | sed s/\"--$switch=\"//`"
fi
libdir=$prefix/$LIB
switch=libdir
if [ "`echo \"$1\" | sed s/\"--$switch=\"//`" != "$1" ]; then
  libdir="`echo \"$1\" | sed s/\"--$switch=\"//`"
else
  libopt="--libdir=$libdir"
fi
conf_opts="$libopt"

if [ -z "$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH=$libdir
else
  LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$libdir
fi
if [ -z "$PKG_CONFIG_PATH" ]; then
  PKG_CONFIG_PATH=$libdir/pkgconfig
else
  PKG_CONFIG_PATH=$PKG_CONFIG_PATH:$libdir/pkgconfig
fi
if [ -z "$PATH" ]; then
  PATH=$prefix/bin
else
  PATH=$PATH:$prefix/bin
fi

export PATH PKG_CONFIG_PATH LD_LIBRARY_PATH


### dependency testing ###

# pkg-config
packet=pkg-config
packet_dir=$packet-0.23
packet_file=$packet_dir.tar.gz
url=http://pkgconfig.freedesktop.org/releases/
packet_ready=0
checksum=b59dddd6b5320bd74c0f74b3339618a327096b2a
pkg-config --version
if [ $? -eq 0 ]; then
  echo "$packet found, skipping $packet build and installation"
  packet_ready=1
else
  if [ -d $packet_dir ]; then
    echo "$packet + $packet_dir found, skipping $packet download"
    packet_ready=1
  fi
  if [ -f $packet_file ]; then
    echo $packet_file already here
  else
    echo downloading $url/$packet_file
    which curl && curl -L $url/$packet_file -o $packet_file || wget $url/$packet_file
    sleep 1
  fi
  if [ `$SHA1SUM $packet_file | grep $checksum | wc -l` -eq 1 ]; then
    echo sha1sum for $packet_file passed
  else
    echo sha1sum for $packet_file failed
    exit 1
  fi
  if [ -d $packet_dir ]; then
    echo remove $packet_dir
    sleep 1
    rm -r $packet_dir
  fi
  tar xzf $packet_file
  cd $packet_dir
   make clean
  ./configure $conf_opts $@
  make $MAKE_CPUS
  make install
fi
sleep 1

cd "$top"

  fltk="FLTK:           `fltk-config --version`"
if [ $? -gt 0 ]; then 
  fltk="!!! FLTK is missed"
  #stop_build=1

  # FLTK
  echo building FLTK ...
  packet=fltk
  #packet_dir=$packet-1.1.10
  #packet_file="$packet_dir"-source.tar.gz
  #url="http://ftp.rz.tu-bs.de/pub/mirror/ftp.easysw.com/ftp/pub/fltk/1.1.10/"
  #checksum=e6378a76ca1ef073bcb092df1ef3ba55
  packet_dir=$packet-1.3.x-r7216
  packet_file="$packet_dir".tar.gz
  checksum=df8bc3f65e5e90591f2196a8957f6c6d
  if [ -f $packet_file ]; then
    echo $packet_file already here
  else
    echo "downloading $url$packet_file"
    which curl && curl -L "$url$packet_file" -o $packet_file || wget "$url$packet_file"
    sleep 1
  fi
  if [ `$MD5SUM $packet_file | grep $checksum | wc -l` -eq 1 ]; then
    echo md5sum for $packet_file passed
  else
    echo md5sum for $packet_file failed
    exit 1
  fi
  packet_ready=0
  pkg-config --atleast-version=1.1.7 $packet
  if [ $? -eq 0 ]; then
    if [ -d $packet_dir ]; then
      echo "$packet + $packet_dir found, skipping $packet build and installation"
      packet_ready=1
    fi
  else
    echo PKG_CONFIG_PATH=$PKG_CONFIG_PATH
    pkg-config --modversion $packet
  fi
  if [ $packet_ready -lt 1 ]; then
    if [ -d $packet_dir ]; then
      echo remove $packet_dir
      sleep 1
      rm -r $packet_dir
    fi
    echo unpacking $packet_file ...
    tar xzf $packet_file
    cd $packet_dir
    make clean
    ./configure $conf_opts --enable-gl $NON_OSX_SHARED --enable-threads --enable-xinerama --enable-xft $@
    make $MAKE_CPUS
    make install
  fi
  sleep 1

  cd "$top"
fi
  ftgl="FTGL:           `pkg-config --modversion ftgl`"
if [ $? -gt 0 ]; then 
  ftgl="FTGL is missed or not ready"
#  stop_build=1
fi
pkg-config  --atleast-version=1.14 lcms
if [ $? -eq 0 ]; then 
  lcms="littleCMS:      `pkg-config --modversion lcms`"
else
  lcms="littleCMS version is too old; need at least lcms-1.14"
  #stop_build=1

  # lcms
  echo building lcms ...
  packet=lcms
  packet_dir=$packet-1.19
  packet_file="$packet_dir".tar.gz
  url="http://www.littlecms.com/"
  checksum=d5b075ccffc0068015f74f78e4bc39138bcfe2d4
  if [ -f $packet_file ]; then
    echo $packet_file already here
  else
    echo "downloading $url$packet_file"
    which curl && curl -L "$url$packet_file" -o $packet_file || wget "$url$packet_file"
    sleep 1
  fi
  if [ `$SHA1SUM $packet_file | grep $checksum | wc -l` -eq 1 ]; then
    echo sha1sum for $packet_file passed
  else
    echo sha1sum for $packet_file failed
    exit 1
  fi
  packet_ready=0
  pkg-config --atleast-version=1.17 $packet
  if [ $? -eq 0 ]; then
    if [ -d $packet_dir ]; then
      echo "$packet + $packet_dir found, skipping $packet build and installation"
      packet_ready=1
    fi
  else
    echo PKG_CONFIG_PATH=$PKG_CONFIG_PATH
    pkg-config --modversion $packet
  fi
  if [ $packet_ready -lt 1 ]; then
    if [ -d $packet_dir ]; then
      echo remove $packet_dir
      sleep 1
      rm -r $packet_dir
    fi
    echo unpacking $packet_file ...
    tar xzf $packet_file
    cd $packet_dir
    make clean
    CFLAGS="$CFLAGS $OSX_ARCH" CXXFLAGS="$CXXFLAGS $OSX_ARCH" LDFLAGS="$LDFLAGS $OSX_ARCH" ./configure $conf_opts $@
    make $MAKE_CPUS
    make install
  fi
  sleep 1

  cd "$top"
fi
echo "$fltk"
echo "$ftgl"
echo "$lcms"


if [ $stop_build -gt 0 ]; then
  echo ""
  echo "some dependencies are missed; see above"
  echo ""
  exit 1
fi
sleep 3

# Elektra
packet=elektra
packet_dir=$packet-0.7.0
packet_file=$packet_dir.tar.gz
checksum=1d0aac0a654660cba0965e7b2998b3f312a9ba96
if [ -f $packet_file ]; then
  echo $packet_file already here
else
  echo downloading http://www.markus-raab.org/ftp/$packet_file
  which curl && curl -L http://www.markus-raab.org/ftp/$packet_file -o $packet_file || wget http://www.markus-raab.org/ftp/$packet_file
  sleep 1
fi
if [ `$SHA1SUM $packet_file | grep $checksum | wc -l` -eq 1 ]; then
  echo sha1sum for $packet_file passed
else
  echo sha1sum for $packet_file failed
  exit 1
fi
packet_ready=0
pkg-config --atleast-version=0.7.0 $packet
if [ $? -eq 0 ]; then
  if [ -d $packet_dir ]; then
    echo "$packet + $packet_dir found, skipping $packet build and installation"
    packet_ready=1
  fi
else
  echo PKG_CONFIG_PATH=$PKG_CONFIG_PATH
  pkg-config --modversion $packet
fi
if [ $packet_ready -lt 1 ]; then
  if [ -d $packet_dir ]; then
    echo remove $packet_dir
    sleep 1
    rm -r $packet_dir
  fi
  echo unpacking $packet_file ...
  tar xzf $packet_file
  cd $packet_dir
  make clean
  CFLAGS="$CFLAGS $OSX_ARCH" CXXFLAGS="$CXXFLAGS $OSX_ARCH" LDFLAGS="$LDFLAGS $OSX_ARCH" ./configure $conf_opts $@
  make $MAKE_CPUS
  make install
  UNAME_=`uname`
  if [ $? = 0 ] && [ $UNAME_ = "Darwin" ]; then
    if [ -f $libdir/libelektra.dylib ]; then
      cp -v elektra.pc $libdir/pkgconfig/
    fi
  fi
fi
sleep 1

cd "$top"

# Git
packet=git
packet_dir=$packet-1.7.0.2
packet_file=$packet_dir.tar.gz
url=http://kernel.org/pub/software/scm/git/
checksum=070ac19c65367ca55138e5e6674d0f35f5113a6b
packet_ready=0
git --version
if [ $? -eq 0 ]; then
  echo "$packet found, skipping $packet build and installation"
  packet_ready=1
else
  if [ -d $packet_dir ]; then
    echo "$packet + $packet_dir found, skipping $packet download"
  fi
  echo PKG_CONFIG_PATH=$PKG_CONFIG_PATH
  pkg-config --modversion $packet
  if [ -f $packet_file ]; then
    echo $packet_file already here
  else
    echo downloading $url/$packet_file
    which curl && curl -L $url/$packet_file -o $packet_file || wget $url/$packet_file
    sleep 1
  fi
  if [ `$SHA1SUM $packet_file | grep $checksum | wc -l` -eq 1 ]; then
    echo sha1sum for $packet_file passed
  else
    echo sha1sum for $packet_file failed
    exit 1
  fi
  if [ -d $packet_dir ]; then
    echo remove $packet_dir
    sleep 1
    rm -r $packet_dir
  fi
  tar xzf $packet_file
  cd $packet_dir
  make clean
  ./configure $conf_opts $@
  make $MAKE_CPUS
  make install
fi
sleep 1

cd "$top"

# Xcolor
git_repo=xcolor
  echo checkout $git_repo
  if [ -d $git_repo ]; then
    cd $git_repo
    git pull
  else
    git clone git://www.oyranos.org/git/$git_repo $git_repo
    cd $git_repo
    git checkout master
  fi
  sleep 2
  ./configure --disable-verbose $conf_opts $@
  make $MAKE_CPUS
  make install
sleep 1

cd "$top"


# Xcm
git_repo=xcm
  echo checkout $git_repo
  if [ -d $git_repo ]; then
    cd $git_repo
    git pull
  else
    git clone git://www.oyranos.org/git/$git_repo $git_repo
    cd $git_repo
    git checkout master
  fi
  sleep 2
  ./configure --disable-verbose $conf_opts $@
  make $MAKE_CPUS
  make install
sleep 1

cd "$top"


# OpenICC default profiles
packet=openicc-data
packet_dir=$packet-1.0.0
packet_file=$packet_dir.tar.gz
checksum=dd2ed93f0862d52a590b7bfcda415483954e1b5c
loc=http://downloads.sourceforge.net/project/openicc/OpenICC-Profiles/
if [ -f $packet_file ]; then
  echo $packet_file already here
else
  echo downloading http://downloads.sourceforge.net/project/openicc/OpenICC-Profiles/$packet_file
  which curl && curl -L $loc$packet_file -o $packet_file || wget $loc$packet_file
  sleep 1
fi
if [ `$SHA1SUM $packet_file | grep $checksum | wc -l` -eq 1 ]; then
  echo sha1sum for $packet_file passed
  echo unpacking $packet_file ...
  tar xzf $packet_file
  if [ -d $packet_dir ]; then
    echo $packet_dir in place
    cd $packet_dir
    ./configure --disable-verbose $conf_opts $@
    make
    make install
  fi
  echo hier: `pwd`
else
  echo sha1sum for $packet_file failed
  exit 1
fi

cd "$top"

sleep 2


# Oyranos
git_repo=oyranos
  echo checkout $git_repo
  if [ -d $git_repo ]; then
    cd $git_repo
    git pull
  else
    git clone git://www.oyranos.org/git/$git_repo $git_repo
    cd $git_repo
    git checkout master
  fi
  sleep 2
  make clean
  CFLAGS="$CFLAGS $OSX_ARCH" CXXFLAGS="$CXXFLAGS $OSX_ARCH" LDFLAGS="$LDFLAGS $OSX_ARCH" ./configure $conf_opts $@  --disable-verbose --enable-debug
  make $MAKE_CPUS
  make install
  make check
sleep 1

cd "$top"


# CompIcc
git_repo=compicc
  echo checkout $git_repo
  if [ -d $git_repo ]; then
    cd $git_repo
    git pull
  else
    git clone git://$git_repo.git.sourceforge.net/gitroot/$git_repo/$git_repo
    cd $git_repo
    git checkout master
  fi
  sleep 2
  ./configure --disable-verbose $conf_opts $@
  make $MAKE_CPUS
  make install
sleep 1

cd "$top"


# ICC Examin
git_repo=icc_examin
  echo checkout $git_repo
  if [ -d $git_repo ]; then
    cd $git_repo
    git pull
  else
    git clone git://www.oyranos.org/git/$git_repo $git_repo
    cd $git_repo
    git checkout master
  fi
  sleep 2
  make clean
  CFLAGS="$CFLAGS $OSX_ARCH" CXXFLAGS="$CXXFLAGS $OSX_ARCH" LDFLAGS="$LDFLAGS $OSX_ARCH" ./configure $conf_opts --disable-verbose --enable-debug $@
  make $MAKE_CPUS
  if [ $? = 0 ] && [ $UNAME_ = "Darwin" ]; then
    make bundle
  fi
#make install
sleep 1

cd "$top"

echo ""
if [ -f "$git_repo/$target" ]; then
  echo ICC Examin is in $git_repo/$target
  echo You can test it now with one of:
  echo   $git_repo/$target http://www.oyranos.org/wiki/images/3/31/SRGB_linear.icc
  echo   $git_repo/$target openicc-data-1.0.0/default_profiles/printing/coated_FOGRA39L_argl.icc
  echo   hint: the 3D gamut hull is shown with the Ctrl-h shortcut
else
  echo Could not build $git_repo/$target
fi

