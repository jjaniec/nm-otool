tmp1=$(mktemp)
tmp2=$(mktemp)

../ft_nm ${1} > ${tmp1} 2> /dev/null
nm ${1} > ${tmp2} 2> /dev/null
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
