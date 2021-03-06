range=`seq 15 25`
n=$((2**30))
thread=0
executable=./`hostname`/segmented

while [[ $# -gt 0 ]]
do
key="$1"
case $key in
    -n)
    n="$2"
    shift # past argument
    shift # past value
    ;;
    -r|--range)
    range="$2"
    shift # past argument
    shift # past value
    ;;
    -e|--executable)
    executable="$2"
    shift # past argument
    shift # past value
    ;;
    -t|--thread)
    thread="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
    shift # past argument
    ;;
esac
done

echo "delta [count,cache]	time [sec]	memory [KiByte]"

for delta in $range
do
    echo -n "2^$delta=$((2**delta/(8*1024)))K	"
    result=`env time -f "%esec	%MKiByte" $executable -n $n -d $((2**delta)) -t $thread -o test.bin 2>&1`
    err="$?"
    difference=`diff -q test.bin test/primes.2_30.bin`
    if [ $err -ne 0 ]
    then 
        result="err$err"
    else
        if [ -n "$difference" ]
        then
            result="DIFF"
        fi
    fi
    echo "$result"
done
