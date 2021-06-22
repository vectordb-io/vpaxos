awk 'BEGIN{
	me=""; peers=""
}
{
	mp[NR]=$0
} END {
	count = 0
	for(i in mp)
	{
		count = count + 1
	}

	for (i = 1; i <= count; i++) {
		key = ""
		value = ""
		for (j = 1; j <= count; j++) {
			if (j == i) {
				key = mp[j]
			} else {
				value = value","mp[j]
			}
		}
		me = key
		peers = substr(value, 2)

		print me, peers
	}
}'
