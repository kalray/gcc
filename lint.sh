#!/bin/bash
diff "$1" <(indent "$1" -o /dev/stdout) | grep -o '^[0-9]*' | sed 's/.*/warning:& Lint error/'
