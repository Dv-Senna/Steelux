tests=($@)

if ((${#tests[@]} == 0))
then
	for entry in src/*.cpp
	do
		filename=$(basename $entry)
		tests+=("${filename%.*}")
	done
fi


for test in ${tests[@]}
do
	echo -e "\e[33m-------------- RUNNING TEST $test ------------------\e[m"
	./../build/tests/Steelux_tests_$test
done
