top_level_dir=`pwd`

sudo apt-get install -y python3

ls -d -1 */ | while read directory
do
    cd "$top_level_dir/$directory"
    echo [ "`pwd`" ]
    if [ -f run_env.sh ]; then bash run_env.sh; fi
    python3 "$top_level_dir/run.py"
done
