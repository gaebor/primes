top_level_dir=`pwd`

ls -d -1 */ | while read directory
do
    cd "$top_level_dir/$directory"
    bash build.sh
done
