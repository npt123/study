sudo apt-get install -y tmux
tmux
ls -l > filedir.txt
mkdir testdir2
cp *.c testdir2/
chmod -r testdir2/*
who >> filedir.txt
tmux
tmux split-window
vim filedir.txt
