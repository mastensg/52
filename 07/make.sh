#!/bin/sh

set -e

go build

test -f hosts.json || echo "[]" > hosts.json
