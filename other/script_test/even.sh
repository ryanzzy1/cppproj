#!/bin/bash

is_even(){
if [ ! $1 ];then
    echo "missing argument;"
    echo "usage should be: ./even.sh arg."
else
    if (( $1 % 2 == 0 ));then
        echo "$1 is a even. "
    else
        echo "$1 is a odd."
    fi
fi
}

#echo "script running"
is_even "$@"

