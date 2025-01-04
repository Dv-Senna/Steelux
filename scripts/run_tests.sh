tests=($@)

if ((${#tests[@]} == 0))
then
	for entry in src/*.cpp
	do
		filename=$(basename $entry)
		tests="${filename%.*}"
	done
fi


for test in ${tests[@]}
do
	./../build/tests/Steelux_tests_$test
done
