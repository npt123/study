for filename in $(ls)
do
    cp $filename backup/$filename
    if [ $? -ne 0 ]
    then
        echo "copy $filename failed"
    fi
done
