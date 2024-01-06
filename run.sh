top_level_dir=`pwd`

ls -d -1 */ | while read directory
do
    cd "$top_level_dir/$directory"
    bash run_env.sh
    python3 "$top_level_dir/run.py"
done
