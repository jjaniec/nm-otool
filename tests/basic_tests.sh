#!/bin/bash
# Basic tests on local binaries

set -o nounset
set -o verbose

dirs=(
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
		DIFF_NM_PAGER=cat ./diff_nm.sh ${i}
		r=${?}
		if [ ${r} -ne 0 ];
		then
			failed_count=$[${failed_count}+1]
			echo -e "\t${i} failed to match, total: ${failed_count}"
			exit_code=${r}
		fi;
	done;
done;

echo "${failed_count} files failed to match"

exit ${exit_code}
