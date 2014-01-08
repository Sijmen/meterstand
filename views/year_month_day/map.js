function(doc){
	emit([
		new Date(doc.unix_time*1000).getFullYear(),
		new Date(doc.unix_time*1000).getMonth(),
		new Date(doc.unix_time*1000).getDate(),
		new Date(doc.unix_time*1000).getHours(),
		new Date(doc.unix_time*1000).getMinutes()
	],{
		watt:doc.watt,
		kilowatt:doc.kilowatt
	});
}