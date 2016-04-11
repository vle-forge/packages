#!/bin/bash
#
# Copyright (C) 2014 INRA
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

set ex

output=ext.log
pkgs="vle.output vle.extension.celldevs vle.extension.cellqss vle.extension.decision vle.extension.difference-equation vle.extension.differential-equation vle.extension.dsdevs vle.extension.fsa vle.extension.petrinet vle.examples ext.lua ext.muparser vle.ibm vle.forrester"

clean_package () {
  echo -e -n " -  clean:"
  rm -fr "$1"/buildvle
  if vle-1.1 -P "$1" clean rclean 2>> $output; then
    echo -e "\e[0;34m ok\e[m"
  else
    echo -e "\e[0;31m failed\e[m"
  fi
}

build_package () {
  oldpwd=$PWD

  echo -e -n " -  configure:"
  if vle-1.1 -P "$1" configure 2>> $output; then
    echo -e "\e[0;34m ok\e[m"
    echo -e -n " -  build:"
    if vle-1.1 -P "$1" build 2>> $output; then
      echo -e "\e[0;34m ok\e[m"
      echo -e -n " -  test:"
      if vle-1.1 -P "$1" test 2>> $output; then
        echo -e "\e[0;34m ok\e[m"
      else
        echo -e "\e[0;31m failed\e[m"
      fi
    else
      echo -e "\e[0;31m failed\e[m"
    fi
  else
    echo -e "\e[0;31m failed\e[m"
  fi

  cd $oldpwd
}

usage () {
  echo "Usage [-c] [-h]"
  echo ""
  echo "-c  clean with rclean"
  echo "-h  this help page"
  exit 0
}

clean=1

echo "" > $output

while getopts ch flags
do
  case $flags in
    c) clean=1
      ;;
    h) usage
      ;;
  esac
done

if [ $clean = 1 ]; then
  for dir in $pkgs; do
    echo -e Package $dir 2>> $output
    clean_package $dir
    build_package $dir
  done
else
  for dir in $pkgs; do
    echo -e Package $dir  2>> $output
    build_package $dir
  done
fi
