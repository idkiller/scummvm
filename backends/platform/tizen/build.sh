#!/usr/bin/env bash

platform_dir=$(dirname $0)
scummvm_home=`readlink -e ${platform_dir}/../../../`

VERBOSE=false
SDK=`readlink -e ~/tizen-sdk/`
PROFILE="mobile"
VERSION="2.4"
HOST="tizen-device"

usage="$(basename "$0") [-h] [-e] [-s=<SDK Path>] [-p=<Profile>] [-v=<Version number>] -- Program to build the ScummVM for Tizen

Options:
  -e, --emulator      set target to emulator (default: $EMULATOR)
  -s, --sdk           set Tizen SDK root path (default: $SDK)
  -p, --profile       set Tizen Profile (default: $PROFILE)
                      (only mobile is supported, other is not yet)
  -v, --version       set Tizen Version (default: 2.4)
                      (2.3, 2.3.1, 2.4 is supported but only 2.4 is tested)
  -h, --help          display help message and exit"

for i in "$@"
do
case $i in
    -h|--help)
      echo "$usage"
      exit
    ;;
    --verbose)
    VERBOSE=true
    shift # past argument
    ;;
    -e|--emulator)
    EMULATOR=true
    shift # past argument=value
    ;;
    -s=*|--sdk=*)
    SDK="${i#*=}"
    shift # past argument=value
    ;;
    -p=*|--profile=*)
    PROFILE="${i#*=}"
    shift # past argument=value
    ;;
    -v=*|--version=*)
    VERSION="${i#*=}"
    shift # past argument=value
    ;;
    --default)
    DEFAULT=YES
    shift #past argument with no value
    ;;
    *)
          # unknown option
    ;;
esac
done

if [ "$VERBOSE" = true ]; then
  CONFIG_OPTION="$CONFIG_OPTION --enable-verbose-build"
fi
if [ "$EMULATOR" = true ]; then
	HOST="tizen-emulator"
	TARGET="emulator"
else
	HOST="tizen-arm"
	TARGET="device"
fi
CONFIG_OPTION="$CONFIG_OPTION --host=$HOST"

ROOTSTRAP="${SDK}/platforms/tizen-${VERSION}/${PROFILE}/rootstraps/${PROFILE}-${VERSION}-${TARGET}.core"

if [ ! -d $ROOTSTRAP ]; then
  echo "Access Error : $ROOTSTRAP" >&2
  echo "$usage" >&2
  exit 1
fi

echo "Tizen SDK    : ${SDK}"
echo "Profile      : ${PROFILE}"
echo "Version      : ${VERSION}"
echo "Target       : ${TARGET}"
echo "Rootstrap    : ${ROOTSTRAP}"


export TIZEN_SDK=$SDK
export TIZEN_ROOTSTRAP=$ROOTSTRAP
$scummvm_home/configure $CONFIG_OPTION
