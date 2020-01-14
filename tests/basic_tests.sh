#!/bin/bash
# Basic tests on local binaries

set -o nounset
set -o verbose

dirs=(
  "$PWD/../objs/*"
	"$PWD/corrupted_binaries/*"
	"/bin/*"
	"/usr/bin/*"
	"/usr/lib/*"
)

failed_count=0
exit_code=0

for dir in ${dirs[@]};
do
	for i in ${dir};
	do
		echo -n -e "ft_${1}:\t\t"
		DIFF_NM_PAGER=cat "./diff_${1}.sh" ${i}
		r=${?}
		if [ ${r} -ne 0 ];
		then
			failed_count=$[${failed_count}+1]
			echo -e "ft_${1}:\t\t${i} failed to match, total: ${failed_count}"
			exit_code=${r}
		fi;
	done;
done;

echo "${failed_count} files failed to match"

exit ${exit_code}
