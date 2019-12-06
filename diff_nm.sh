tmp1=$(mktemp)
tmp2=$(mktemp)

make && ./ft_nm ${1} > ${tmp1}
nm ${1} > ${tmp2}
diff -y ${tmp1} ${tmp2} | less
echo ${tmp1} ${tmp2}
#rm ${tmp1} ${tmp2}
