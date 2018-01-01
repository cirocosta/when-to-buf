#!/bin/bash

main () {
  for i in $(seq 1 16); do
    run_all_shifts
  done
}

run_all_shifts () {
  local bufsize

  for i in $(seq 8 24); do
    bufsize=$(./shift 1 $i)

    ./client 127.0.0.1 "$bufsize"
  done
}

main "$@"
