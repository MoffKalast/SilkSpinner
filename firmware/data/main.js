var gateway = `ws://${window.location.hostname}/ws`;

gateway = `ws://192.168.4.1/ws`;

var websocket;
var interval;

var left_spinner_vel = 0;
var right_spinner_vel = 0;

var left_spinner_pos = 0;
var right_spinner_pos = 0;

var left_arm_pos = 0;
var right_arm_pos = 0;

var connected = false;
var settings = undefined;
var telemetry = undefined;
var block_updates = false;

var MODES = ["swipe", "static", "groups"];

window.addEventListener('load', function(event) {
	//set_sliders();
	connect();
});

function connect() {
	websocket = new WebSocket(gateway);
	websocket.onopen = on_open;
	websocket.onclose = on_close;
	websocket.onmessage = on_msg;

	document.getElementById("infotext").innerHTML = "Reconnecting..."
	document.getElementById("infotext").style.backgroundColor = "#cab873";
}

function on_open(event) {
	console.log('Connection established.');
	connected = true;
}

function on_close(event) {
	console.log('Connection lost!');
	window.clearInterval(interval);

	document.getElementById("infotext").innerHTML = "Disconnected."
	document.getElementById("infotext").style.backgroundColor = "tomato";    
	connected = false;

	setTimeout(function(){
		connect();
	}, 500);
}

function on_msg(event) {
	document.getElementById("infotext").innerHTML = "Connected."
	document.getElementById("infotext").style.backgroundColor = "#99ffaa";

	let obj = JSON.parse(event.data);

	if("vel" in obj.right){
		telemetry = obj;
		updateTelemetry(telemetry.right, "r");
		updateTelemetry(telemetry.left, "l");
	}else{
		settings = obj;
		updateSettings(settings.right, "r");
		updateSettings(settings.left, "l");
	}
}

function updateTelemetry(data, side){
	let hz = data.freq.toFixed(2);
	let rpm = Math.round(data.freq*60);

	if(data.vel == 0){
		hz = 0;
		rpm = 0;
	}

	document.getElementById(side+"_freq").innerHTML = "Frequency: "+hz+" Hz / "+rpm+" RPM";
	document.getElementById(side+"_turns").innerHTML = "Turns: "+(data.counter/3).toFixed(1);

	let distance = (data.counter*0.06).toFixed(2);
	document.getElementById(side+"_dist").innerHTML = "Distance: "+distance+" m";

	let direction = "Normal";
	if(data.vel < 0)
		direction = "Reverse";

	document.getElementById(side+"_dir").innerHTML = "Direction: "+direction;

	if(!block_updates){
		document.querySelectorAll("."+side+'_armpostext').forEach(function(element) {
			element.innerHTML = "Position: "+((data.servo_pos-0.5)*160.0).toFixed(1)+" deg";
		});
	}

	if(settings != undefined){

		let started = false;

		if(side == "r")
			started = settings.right.start;
		else
			started = settings.left.start;

		if(!started){
			if(data.counter == 0){
				document.getElementById(side+"_start").style.backgroundColor = "#a8e4b1";
				document.getElementById(side+"_start").innerHTML = "Start";
				document.getElementById(side+"_entry").disabled = true;
			}
			else{
				document.getElementById(side+"_start").style.backgroundColor = "rgb(221 225 141)";
				document.getElementById(side+"_start").innerHTML = "Continue";
				document.getElementById(side+"_entry").disabled = data.vel != 0;
			}
		}
		else{
			document.getElementById(side+"_entry").disabled = true;
		}
	}
}

function updateSettings(data, side){
	if(!block_updates){
		document.getElementById(side+"_set_dir").checked = data.dir;
		document.getElementById(side+"_set_thr").value = data.throttle;
		document.getElementById(side+"_info_thr").innerHTML = "Set Throttle: "+data.throttle+"%";
		document.getElementById(side+"_swipespeed").value = data.swipe_speed;

		document.getElementById(side+"_arm_limits").low = data.angle_min;
		document.getElementById(side+"_arm_limits").high = data.angle_max;

		document.getElementById(side+"_groups_num").value = data.groups;
		document.getElementById(side+"_group_len").value = data.group_len;
		document.getElementById(side+"_swipespeed").value = data.swipe_speed;

		document.querySelectorAll("."+side+'_armmin').forEach(function(element) {
			element.innerHTML = "Left Limit: "+Math.round((data.angle_min-50)*1.6)+" deg";
		});
		document.querySelectorAll("."+side+'_armmax').forEach(function(element) {
			element.innerHTML = "Right Limit: "+Math.round((data.angle_max-50)*1.6)+" deg";
		});
	}

	if(data.start){
		document.getElementById(side+"_start").style.backgroundColor = "#ffa599";
		document.getElementById(side+"_start").innerHTML = "Stop";

		["swipe", "static", "groups"].forEach(function (item, index) {
			let e = document.getElementById(side+"_"+item+"_button");
			e.disabled = true;
			e.style.cursor = "not-allowed";
		});
	}
	else
	{
		document.getElementById(side+"_start").style.backgroundColor = "#a8e4b1";
		document.getElementById(side+"_start").innerHTML = "Start";

		["swipe", "static", "groups"].forEach(function (item, index) {
			let e = document.getElementById(side+"_"+item+"_button");
			e.disabled = false;
			e.style.cursor = "pointer";
		});
	}

	showMode(side+"_"+MODES[data.mode]);
}

setInterval(function(){

	if(!connected || telemetry == undefined)
		return;

	left_spinner_vel = left_spinner_vel * 0.8 + 0.2 * (telemetry.left.freq * 6);
	right_spinner_vel = right_spinner_vel * 0.8 + 0.2 * (telemetry.right.freq * 6);

	if(telemetry.left.vel != 0){
		if(telemetry.left.vel < 0)
			left_spinner_pos -= left_spinner_vel;
		else
			left_spinner_pos += left_spinner_vel;
	}

	if(telemetry.right.vel != 0){
		if(telemetry.right.vel < 0)
			right_spinner_pos += right_spinner_vel;
		else
			right_spinner_pos -= right_spinner_vel;
	}

	left_arm_pos = left_arm_pos * 0.9 + telemetry.left.servo_pos * 0.1;
	right_arm_pos = right_arm_pos * 0.9 + telemetry.right.servo_pos * 0.1;

	document.getElementById("spinner_left").style.transform = "rotateZ("+left_spinner_pos+"deg)";
	document.getElementById("spinner_right").style.transform = "rotateZ("+right_spinner_pos+"deg)";

	document.getElementById("arm_left").style.transform = "rotateZ("+(left_arm_pos*160-80)+"deg)";
	document.getElementById("arm_right").style.transform = "rotateZ("+(right_arm_pos*160-80)+"deg)";
},16)

setInterval(function(){

	if(settings == undefined)
		return;

	if(!settings.right.start || settings.right.time == "")
		setTime(Number(settings.right.duration), "r");
	else
		setTime(Number(settings.right.duration) + getDeltaTime(settings.right.time), "r");


	if(!settings.left.start || settings.left.time == "")
		setTime(Number(settings.left.duration), "l");
	else
		setTime(Number(settings.left.duration) + getDeltaTime(settings.left.time), "l");

}, 200);

function getDeltaTime(stamp){
	let delta = Number(new Date() - new Date(Number(stamp)));
	if(delta < 0)
		return 0;
	return delta;
}

function setTime(diff_ms, side){

	let diff_hrs = Math.floor((diff_ms % 86400000) / 3600000);
	let diff_mins = Math.floor(((diff_ms % 86400000) % 3600000) / 60000);
	let diff_secs = Math.round((((diff_ms % 86400000) % 3600000) % 60000)/ 1000);

	diff_hrs = diff_hrs.toString().padStart(2, '0');
	diff_mins = diff_mins.toString().padStart(2, '0');
	diff_secs = diff_secs.toString().padStart(2, '0');

	document.getElementById(side+"_timer").innerHTML = diff_hrs + ":" + diff_mins + ":" +diff_secs;
}

// Direction

document.getElementById("r_set_dir").addEventListener("change", function(){
	let val = document.getElementById("r_set_dir").checked;
	if(connected){
		websocket.send("R,dir,"+val);
	}
});

document.getElementById("l_set_dir").addEventListener("change", function(){
	let val = document.getElementById("l_set_dir").checked;
	if(connected){
		websocket.send("L,dir,"+val);
	}
});


// Throttle

document.getElementById("r_set_thr").addEventListener("input", function(){
	block_updates = true;
	let val = document.getElementById("r_set_thr").value;
	document.getElementById("r_info_thr").innerHTML = "Set Throttle: "+val+"%";
});

document.getElementById("r_set_thr").addEventListener("change", function(){
	let val = document.getElementById("r_set_thr").value;
	if(connected){
		websocket.send("R,throt,"+val);
	}
	document.getElementById("r_info_thr").innerHTML = "Set Throttle: "+val+"%";
	block_updates = false;
});

document.getElementById("l_set_thr").addEventListener("input", function(){
	block_updates = true;
	let val = document.getElementById("l_set_thr").value;
	document.getElementById("l_info_thr").innerHTML = "Set Throttle: "+val+"%";
});

document.getElementById("l_set_thr").addEventListener("change", function(){
	let val = document.getElementById("l_set_thr").value;
	if(connected){
		websocket.send("L,throt,"+val);
	}

	document.getElementById("l_info_thr").innerHTML = "Set Throttle: "+val+"%";
	block_updates = false;
});


document.getElementById("r_0%").addEventListener("click", (event) => {
	if(connected){
		websocket.send("R,throt,0");
		document.getElementById("r_info_thr").innerHTML = "Set Throttle: 0%";
	}
});

document.getElementById("r_100%").addEventListener("click", (event) => {
	if(connected){
		websocket.send("R,throt,100");
		document.getElementById("r_info_thr").innerHTML = "Set Throttle: 100%";
	}
});

document.getElementById("l_0%").addEventListener("click", (event) => {
	if(connected){
		websocket.send("L,throt,0");
		document.getElementById("l_info_thr").innerHTML = "Set Throttle: 0%";
	}
});

document.getElementById("l_100%").addEventListener("click", (event) => {
	if(connected){
		websocket.send("L,throt,100");
		document.getElementById("l_info_thr").innerHTML = "Set Throttle: 100%";
	}
});

// Arm Pos

document.getElementById("r_arm_pos").addEventListener("value-changing", function(){
	block_updates = true;
	let val = document.getElementById("r_arm_pos").value;
	document.querySelectorAll('.r_armpostext').forEach(function(element) {
		element.innerHTML = "Position: "+Math.round((val-50)*1.6)+" deg";
	});
});

document.getElementById("r_arm_pos").addEventListener("value-changed", function(){
	let val = document.getElementById("r_arm_pos").value;
	if(connected){
		websocket.send("R,angle,"+val);
		document.querySelectorAll('.r_armpostext').forEach(function(element) {
			element.innerHTML = "Position: "+Math.round((val-50)*1.6)+" deg";
		});
	}
	block_updates = false;
});

document.getElementById("l_arm_pos").addEventListener("value-changing", function(){
	block_updates = true;
	let val = document.getElementById("l_arm_pos").value;
	document.querySelectorAll('.l_armpostext').forEach(function(element) {
		element.innerHTML = "Position: "+Math.round((val-50)*1.6)+" deg";
	});
});

document.getElementById("l_arm_pos").addEventListener("value-changed", function(){
	let val = document.getElementById("l_arm_pos").value;
	if(connected){
		websocket.send("L,angle,"+val);
		document.querySelectorAll('.l_armpostext').forEach(function(element) {
			element.innerHTML = "Position: "+Math.round((val-50)*1.6)+" deg";
		});
	}
	block_updates = false;
});

// Arm Limits

document.getElementById("r_arm_limits").addEventListener("value-changing", function(){
	block_updates = true;
	let low = document.getElementById("r_arm_limits").low;
	let high = document.getElementById("r_arm_limits").high;
	document.querySelectorAll('.r_armmin').forEach(function(element) {
		element.innerHTML = "Left Limit: "+Math.round((low-50)*1.6)+" deg";
	});
	document.querySelectorAll('.r_armmax').forEach(function(element) {
		element.innerHTML = "Right Limit: "+Math.round((high-50)*1.6)+" deg";
	});
});

document.getElementById("r_arm_limits").addEventListener("value-changed", function(){
	let low = document.getElementById("r_arm_limits").low;
	let high = document.getElementById("r_arm_limits").high;
	if(connected){
		websocket.send("R,angle_limits,"+low+","+high);
	}
	block_updates = false;
});

document.getElementById("l_arm_limits").addEventListener("value-changing", function(){
	block_updates = true;
	let low = document.getElementById("l_arm_limits").low;
	let high = document.getElementById("l_arm_limits").high;
	document.querySelectorAll('.l_armmin').forEach(function(element) {
		element.innerHTML = "Left Limit: "+Math.round((low-50)*1.6)+" deg";
	});
	document.querySelectorAll('.l_armmax').forEach(function(element) {
		element.innerHTML = "Right Limit: "+Math.round((high-50)*1.6)+" deg";
	});
});

document.getElementById("l_arm_limits").addEventListener("value-changed", function(){
	let low = document.getElementById("l_arm_limits").low;
	let high = document.getElementById("l_arm_limits").high;
	if(connected){
		websocket.send("L,angle_limits,"+low+","+high);
	}
	block_updates = false;
});

// Swipe speed

document.getElementById("r_swipespeed").addEventListener("focus", (event) => {
	block_updates = true;
});
  
document.getElementById("r_swipespeed").addEventListener("blur", (event) => {
	let val = document.getElementById("r_swipespeed").value;

	if(val > 50)
		val = 50;
	else if(val < 0)
		val = 0;

	if(connected){
		websocket.send("R,swipe_speed,"+val);
	}
	block_updates = false;
});

document.getElementById("l_swipespeed").addEventListener("focus", (event) => {
	block_updates = true;
});
  
document.getElementById("l_swipespeed").addEventListener("blur", (event) => {
	let val = document.getElementById("l_swipespeed").value;

	if(val > 50)
		val = 50;
	else if(val < 0)
		val = 0;

	if(connected){
		websocket.send("L,swipe_speed,"+val);
	}
	block_updates = false;
});

// Groups

document.getElementById("l_groups_num").addEventListener("focus", (event) => {
	block_updates = true;
});
  
document.getElementById("l_groups_num").addEventListener("blur", (event) => {
	let val = document.getElementById("l_groups_num").value;

	if(val > 30)
		val = 30;
	else if(val < 2)
		val = 2;

	if(connected){
		websocket.send("L,groups,"+val);
	}
	block_updates = false;
});

document.getElementById("r_groups_num").addEventListener("focus", (event) => {
	block_updates = true;
});
  
document.getElementById("r_groups_num").addEventListener("blur", (event) => {
	let val = document.getElementById("r_groups_num").value;

	if(val > 30)
		val = 30;
	else if(val < 2)
		val = 2;

	if(connected){
		websocket.send("R,groups,"+val);
	}
	block_updates = false;
});

document.getElementById("r_group_len").addEventListener("focus", (event) => {
	block_updates = true;
});
  
document.getElementById("r_group_len").addEventListener("blur", (event) => {
	let val = document.getElementById("r_group_len").value;

	if(val > 5000)
		val = 5000;
	else if(val < 1)
		val = 1;

	if(connected){
		websocket.send("R,group_len,"+val);
	}
	block_updates = false;
});

document.getElementById("l_group_len").addEventListener("focus", (event) => {
	block_updates = true;
});
  
document.getElementById("l_group_len").addEventListener("blur", (event) => {
	let val = document.getElementById("l_group_len").value;

	if(val > 5000)
		val = 5000;
	else if(val < 1)
		val = 1;

	if(connected){
		websocket.send("L,group_len,"+val);
	}
	block_updates = false;
});

// Start
document.getElementById("l_start").addEventListener("click", (event) => {
	if(connected){
		websocket.send("L,start,"+!settings.left.start);
		if(!settings.left.start){
			websocket.send("L,time,"+((new Date()).getTime()).toString());
		}else{
			let duration = Number(settings.left.duration);
			if(settings.left.time && settings.left.time !== "")
				duration += getDeltaTime(settings.left.time);

			websocket.send("L,duration,"+duration.toString());
		}
	}
});

document.getElementById("r_start").addEventListener("click", (event) => {
	if(connected){
		websocket.send("R,start,"+!settings.right.start);
		if(!settings.right.start){
			websocket.send("R,time,"+((new Date()).getTime()).toString());
		}else{
			let duration = Number(settings.right.duration);
			if(settings.right.time && settings.right.time !== "")
				duration += getDeltaTime(settings.right.time);

			websocket.send("R,duration,"+duration.toString());
		}
	}
});

// Zero
document.getElementById("l_zero").addEventListener("click", (event) => {
	if(connected){
		document.getElementById("l_timer").innerHTML = "--:--:--";

		["l_timer", "l_dist", "l_turns"].forEach(function (item, index) {
			document.getElementById(item).style.backgroundColor = "#545454";
			document.getElementById(item).style.color = "white";
		});

		setTimeout(function(){
			websocket.send("L,clear");

			["l_timer", "l_dist", "l_turns"].forEach(function (item, index) {
				document.getElementById(item).style.backgroundColor = "";
				document.getElementById(item).style.color = "";
			});

			setTimeout(function(){
				if(settings.right.start)
					websocket.send("L,time,"+((new Date()).getTime()).toString());
			},200);
		},300);
		
	}
});

document.getElementById("r_zero").addEventListener("click", (event) => {
	if(connected){
		document.getElementById("r_timer").innerHTML = "--:--:--";

		["r_timer", "r_dist", "r_turns"].forEach(function (item, index) {
			document.getElementById(item).style.backgroundColor = "#545454";
			document.getElementById(item).style.color = "white";
		});

		setTimeout(function(){
			websocket.send("R,clear");

			["r_timer", "r_dist", "r_turns"].forEach(function (item, index) {
				document.getElementById(item).style.backgroundColor = "";
				document.getElementById(item).style.color = "";
			});

			setTimeout(function(){
				if(settings.right.start)
					websocket.send("R,time,"+((new Date()).getTime()).toString());
			},200);
		},300);
	}
});

// Tab modes

function setMode(mode) {
	showMode(mode);
	if(connected){

		let modenum = MODES.indexOf(mode.split("_")[1]);

		if(mode[0] == "l")
			websocket.send("L,mode,"+modenum);
		else if(mode[0] == "r")
			websocket.send("R,mode,"+modenum);
	}
}

function showMode(mode) {

	let button = document.getElementById(mode+"_button");
	let list = ["l_swipe","r_swipe", "l_static", "r_static", "l_groups", "r_groups"];

	for (let i = 0; i < list.length; i++) {

		if(mode[0] != list[i][0])
			continue;

		let page = document.getElementById(list[i]);
		let pagebutton = document.getElementById(list[i]+"_button");

		page.style.display = "none";
		pagebutton.style.backgroundColor = "buttonface";
		pagebutton.style.color = "rgb(54, 54, 54)";
	}

	if(mode[0] == "l"){
		if(mode == "l_static"){
			document.getElementById("l_arm_pos").style.display = "block";
			document.getElementById("l_arm_limits").style.display = "none";
		}else{
			document.getElementById("l_arm_pos").style.display = "none";
			document.getElementById("l_arm_limits").style.display = "block";
		}

		if(mode == "l_groups"){
			let container = document.getElementById("groups_left");
			container.style.display = "block";
			let html = "";

			const groups = groupSplits(settings.left.groups, settings.left.angle_min, settings.left.angle_max);
			for(let i = 0; i < settings.left.groups; i++){
				let style = "transform: rotateZ("+(groups.next().value*1.6 - 80)+"deg)";
				html +="<img style='"+style+"'class='groupline noselect' src='group.svg' alt=''/>\n";
			}

			container.innerHTML = html;
		}
		else{
			document.getElementById("groups_left").style.display = "none";
		}
	}
	else if(mode[0] == "r"){
		if(mode == "r_static"){
			document.getElementById("r_arm_pos").style.display = "block";
			document.getElementById("r_arm_limits").style.display = "none";
		}else{
			document.getElementById("r_arm_pos").style.display = "none";
			document.getElementById("r_arm_limits").style.display = "block";
		}

		if(mode == "r_groups"){
			let container = document.getElementById("groups_right");
			container.style.display = "block";
			let html = "";

			const groups = groupSplits(settings.right.groups, settings.right.angle_min, settings.right.angle_max);
			for(let i = 0; i < settings.right.groups; i++){
				let style = "transform: rotateZ("+(groups.next().value*1.6 - 80)+"deg)";
				html +="<img style='"+style+"'class='groupline noselect' src='group.svg' alt=''/>\n";
			}

			container.innerHTML = html;
		}
		else{
			document.getElementById("groups_right").style.display = "none";
		}
	}

	document.getElementById(mode).style.display = "block";
	button.style.backgroundColor = "#4b4b4b";
	button.style.color = "white";

}

function* groupSplits(groups, start, end){
	let total = end-start;
	let delta = total/(groups-1);

	for(let i = 0; i < groups; i++){
		yield (start+i*delta);
	}
}