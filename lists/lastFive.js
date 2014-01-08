function(head,req){

	start({
		"headers":{
			"Content-Type":"text/csv",
			"Content-Disposition":"attachment;filename=historie.csv"
		}
	});
	if(req.query.group && req.query.reduce){
		send('"tijd","watt"');
		while ((row = getRow())) {          
			send('"'+row.key+'","'+row.value.total/1000'"');
		}    
		
	}
}