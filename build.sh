top_level_dir=`pwd`

ls -1 */build.sh | while read build_script
do
    cd "`dirname "$top_level_dir/$build_script"`"
    pwd
    bash ./build.sh
done
