#!/bin/sh
if echo "$1" | grep -Eq 'i[[:digit:]]86-'; then
  echo i386
elif echo "$1" | grep -Eq 'x86_64-'; then
  echo amd64
else
  echo "$1" | grep -Eo '^[[:alnum:]_]*'
fi
