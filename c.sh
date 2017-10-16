TYPE="Debug"

if [ ! -z "$1" ]; then
    if [ "$1" = "release" ]; then
        TYPE="Release"
    fi
fi

../"$TYPE"/Vii.exe -stdc
g++ -c io.cpp
../"$TYPE"/Vii.exe -w
g++ -o io.o -c test.cpp
g++ -o test io.o
