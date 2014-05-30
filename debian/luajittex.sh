#!/bin/sh

jobname=""
while [ -n "$1" ] ; do
  case "$1" in
    -jobname=*) 
	jobname=`echo "$1" | sed -e 's/^-jobname=//'`
	break
	;;
    *) shift ;;
  esac
done

if [ -z "$jobname" ] ; then
  jobname=luajittex
fi

touch $jobname.log
touch $jobname.fmt

echo "luajittex is not supported on this architecture!" >&2
echo "Generating fake $jobname.fmt and $jobname.log." >&2


exit 0

