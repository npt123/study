function rand() {
    min=$1
    max=$(($2-$min+1))
    num=$(($RANDOM+10000000000))
    echo $(($num%$max+$min))
}
num=$(rand 1 $(ls | wc -l))
i=0
for filename in $(ls)
do
    i=$[ $i + 1 ]
    if [ $i = $num ]
    then
        if [ -d $filename ]
        then
            echo "$filename is a directory"
        else
            echo "$filename is not a directory"
        fi
        if [ -f $filename ]
        then
            echo "$filename is a regular file"
        else
            echo "$filename is not a regular file"
        fi
        if [ -r $filename ]
        then
            echo "$filename has read permission"
        else
            echo "$filename does not have read permission"
        fi
        if [ -w $filename ]
        then
            echo "$filename has write permission"
        else
            echo "$filename does not have write permission"
        fi
        if [ -x $filename ]
        then
            echo "$filename has execute permission"
        else
            echo "$filename does not have execute permission"
        fi
    fi
done
