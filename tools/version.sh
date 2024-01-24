#!/bin/sh
if [ -z "$1" ]; then
   echo "Usage: version.sh path/to/version.h"
   exit 1
fi

echo "VERSION_MAJOR.VERSION_MINOR.VERSION_PATCH" | /usr/bin/cpp -include $1 -nostdinc -P | tr -d " " | sed '/^[[:space:]]*$/d'
