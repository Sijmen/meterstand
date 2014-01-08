function(doc){
	emit(doc.unix_time,{watt:doc.watt,kilowatt:doc.kilowatt});
}