#!/bin/bash
# for i in /bin/*; do
# 	./../ft_otool ${i} > a # 2> /dev/null ;
# 	otool -t ${i} > b ;
# 	diff a b > /dev/null;
# 	echo $i":"$? ;
# done;
#./ft_otool ${1} > a 2> /dev/null ; otool -t ${1} > b ; diff a b > /dev/null; exit $?


tmp1=$(mktemp)
tmp2=$(mktemp)

../ft_otool ${1} 2> /dev/null | grep -e "\t" > ${tmp1} #2> /dev/null
#otool -t ${1} > ${tmp2} 2> /dev/null | grep "\t"
otool -t ${1} 2> /dev/null | grep -e "\t" > ${tmp2} #-e "Contents" -e ${1} > ${tmp2} 
diff ${tmp1} ${tmp2} > /dev/null 2>&1
r=$?
if [ ${r} -ne 0 ];
then
	echo "${1} KO"
	echo ${tmp1} ${tmp2}
	if [ "${DIFF_NM_PAGER}" == "" ];
	then
		diff -y ${tmp1} ${tmp2} | less
	else
		diff -y ${tmp1} ${tmp2} | ${DIFF_NM_PAGER}
	fi;
elif [ "${VERBOSE}" != "0" ];
then
  echo "${1} OK"
	rm ${tmp1} ${tmp2}
fi

exit ${r}
