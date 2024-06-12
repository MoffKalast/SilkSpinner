var spreadsheet = [];

document.getElementById("l_entry").addEventListener("click", new_entry);
document.getElementById("r_entry").addEventListener("click", new_entry);
document.getElementById("download").addEventListener("click", download_spreadsheet);

function update_modal_spreadsheet(){
	let table = document.createElement('table');
	let tableBody = document.createElement('tbody');

	let i = 0;
	spreadsheet.forEach(function(rowData) {
		let row = document.createElement('tr');

		rowData.forEach(function(cellData) {
			let cell = document.createElement('td');
			cell.appendChild(document.createTextNode(cellData));
			row.appendChild(cell);
		});

		let delbutton = document.createElement('td');
		delbutton.innerHTML = "<button class='xbutton' type='button' onclick='delete_entry("+i+")'>X</button>";
		row.appendChild(delbutton);

		tableBody.appendChild(row);
		i++;
	});

	table.appendChild(tableBody);

	let cont = document.getElementById("tablecontainer");
	cont.innerHTML = "";
	cont.appendChild(table);
}

function delete_entry(i){
	spreadsheet.splice(i,1);
	localStorage.setItem("spreadsheet", JSON.stringify(spreadsheet));
	update_modal_spreadsheet();
}

async function new_entry(event){

	let oznaka = await prompt("Enter the label (oznaka):", "Murva 1");
	let number = await prompt("Enter number of combined cocoons:", "1");

	let data = telemetry.right;
	let setup = settings.right;

	if(event.currentTarget.id[0] == "l"){
		data = telemetry.left;
		setup = settings.left;
	}

	let total = (data.counter*0.06)

	let newline = [
		new Date().toISOString().split("T")[0],
		oznaka,
		number,
		(setup.duration/60000).toFixed(1),
		total.toFixed(2)
	];

	let infostring = "";
	infostring += "Date: "+newline[0]+"\n";
	infostring += "Label: "+newline[1]+"\n";
	infostring += "Cocoons: "+newline[2]+"\n";
	infostring += "Duration: "+newline[3]+"\n";
	infostring += "Total Length: "+newline[4]+"\n";

	if(setup.mode == 2)
	{
		let i = 1;
		while(total > 0){
	
			if(total > setup.group_len){
				newline.push(setup.group_len+"");
				infostring += "Layer "+i+": "+setup.group_len+"\n";
				total -= setup.group_len;
			}
			else{
				infostring += "Layer "+i+": "+total.toFixed(2)+"\n";
				newline.push(total.toFixed(2));
				break;
			}
			i++;
		}
	}

	if(await confirm("Add this data point?\n"+infostring)){
		spreadsheet.push(newline);
		localStorage.setItem("spreadsheet", JSON.stringify(spreadsheet));
	}

	update_modal_spreadsheet();
	Modal.open("modal_measurements")
}

function download_spreadsheet(){
	
	let csvContent = "data:text/csv;charset=utf-8,";

	csvContent += "Datum,";
	csvContent += "Oznaka,";
	csvContent += "Stevilo zdruzenih kokonov,";
	csvContent += "Cas odvijanja (min),";
	csvContent += "Skupna dolzina odvite niti (m),";

	let maxlen = 0;
	for(let i = 0; i < spreadsheet.length; i++){
		if(spreadsheet[i].length > maxlen)
			maxlen = spreadsheet[i].length;
	}

	for(let i = 5; i < maxlen; i++){
		csvContent += "Plast "+(i-4)+" (m),";
	}

	csvContent += "\r\n";
	
	for(let i = 0; i < spreadsheet.length; i++){
		csvContent += spreadsheet[i].join(",") + "\r\n";
	}
	
	window.open(encodeURI(csvContent));
}

window.addEventListener("load", function(){
	if(localStorage.hasOwnProperty("spreadsheet")){
		spreadsheet = JSON.parse(localStorage.getItem("spreadsheet"))
	}	
});