#!/bin/sh

BASE_URL="http://localhost:8080"
HOSTNAME="$(hostname)"

curl "$BASE_URL"/"$HOSTNAME" --data hostname="$HOSTNAME"
