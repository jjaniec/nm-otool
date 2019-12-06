tmp1=$(mktemp)
tmp2=$(mktemp)

./ft_nm ${1} > ${tmp1} 2> /dev/null
nm ${1} > ${tmp2}
diff ${tmp1} ${tmp2} > /dev/null 2>&1
r=$?
if [ ${r} -ne 0 ];
then
	echo ${tmp1} ${tmp2}
	diff -y ${tmp1} ${tmp2} | less
else
	echo "${1} OK"
	rm ${tmp1} ${tmp2}
fi
