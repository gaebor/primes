top_level_dir=`pwd`

ls -d -1 */ | while read directory
do
    cd "$top_level_dir/$directory"
    if [ -f run_env.sh ]; then bash run_env.sh; fi
    python3 "$top_level_dir/run.py"
done
