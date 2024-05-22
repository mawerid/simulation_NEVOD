#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <path_to_nevod> <number_of_threads>"
    exit 1
fi

# Path to the nevod command
NEVOD_PATH=$1

# Number of threads
N=$2

# Run the command in parallel
for (( i=0; i<N; i++ ))
do
    $NEVOD_PATH "$i" 0 &
done

# Wait for all background jobs to finish
wait

echo "All commands have completed."
