#!/bin/bash
# Basic tests on local binaries

set -o nounset
set -o verbose

dirs=(
	"/bin/*"
	"/usr/bin/*"
)

exit_code=0

for dir in ${dirs[@]};
do
	for i in ${dir};
	do
		DIFF_NM_PAGER=cat ./diff_nm.sh ${i}
		r=${?}
		if [ ${r} -ne 0 ];
		then
			exit_code=${r}
		fi;
	done;
done;

exit ${exit_code}
