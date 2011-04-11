#!/bin/bash
# Copyright (C) 2001-2011 Warmux Team.
DIRS="src lib tools"

new_year="$1"
[[ -z $new_year ]] && echo "Missing parameter: year" && exit 1

[[ ! -e src ]] && echo "This script should be ran from the top warmux dir" && exit 2

tmp_file="w$RANDOM$RANDOM$RANDOM$RANDOM"
[[ -e $tmp_file ]] && tmp_file="w$RANDOM$RANDOM$RANDOM$RANDOM"

# update the dates, creating the interval if it doesn't exist yet
find $DIRS -iname "*.cpp" -or -iname "*.h" -or -iname "*.sh" -or -iname "*.py" |
  xargs sed -i "/Copyright.*Warmux Team/ s,\(20[0-9]*\) \|\(20[0-9]*\)-20[0-9]* ,\1\2-$new_year ,"

# do a semi-automated commit check
svn diff $DIRS > $tmp_file
echo "The next +/- counts mentioning copyrights should match:"
grep "^[-+][^-+]" $tmp_file | sort | uniq -c
echo "If they don't, try finding the offending files with grep -rl <\$bad_line>"

#rm -f src/config.h # it isn't tracked - generated by autotools
#svn commit $(find -iname "*.cpp" -or -iname "*.h")
rm $tmp_file
