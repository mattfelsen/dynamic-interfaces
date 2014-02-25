// Load up the TCP server
var server = require("server");

// Load up HTTP module
var http = require("http");
// var fs = require("fs");

// Load ProtoBuf.js, parse the schema, and build the main FeedMessage container object
var ProtoBuf = require("protobufjs");
var builder = ProtoBuf.loadProtoFile("nyct-subway.proto");

var root = builder.build("");
var FeedMessage = root.transit_realtime.FeedMessage;

// MTA Developer Key
var mtaKey = "****";
var url = "http://datamine.mta.info/mta_esi.php?key="+mtaKey+"&feed_id=2";

var stationID = "L02N"; // 6th Ave
// var stationID = "L08S"; // Bedford Av
// var stationID = "L03"; // Union Square

// time handlers
var time = {

	now: function () {
		return new Date();
	},
	hour: function() {
		return this.now().getHours() * 60;
	},
	min: function() {
		return this.now().getMinutes();
	},
	full: function() {
		return this.min() + this.hour();
	},
	
	diff: [],
	
	// accepts time as minutes. ie: 4:02AM = 60*4 + 02.
	// returns the average time between trains in minutes for the given block of time
	interval: function(time) {

		switch(true) {

			//if time is between midnight at 6:02 AM
			case time <= 60 * 6 + 02:
				return 11; 
				break;
			
			//if time is between 6:03 AM and 2:07 PM
			case time > (60 * 6 + 02) && time <= (60 * 14 + 07):
				return 5; //return 5
				break;

			//if time is between 2:07 PM and 8:01 PM
			case time > (60 * 14 + 07) && time <= (60 * 20 + 01):
				return 7;
				break;
			
			//if time is between 8:01 PM and 11:59 PM:
			case time > (60 * 20 + 01) && time <= (60 * 23 + 59):
				return 5; 
				break;
			
			default: 
				return 12;
				break;
		}
	},
		
	numSpokesLit: function() {
		// how close the train is to arriving
		// ie: if it's 1 minute away and the train is coming at intervals of 5 minutes,  the train is 80% here
		
		// first let's find the first entry in the list that is not negative
		// (negative entries refer to trains that have already left)
		// if there are no non-negative entries, return 0
		var index = 0;
		while (time.diff[index] < 0) {
			index++;
		}
		if (time.diff[index] == undefined) {
			return 0;
		}

		// check that the train hasn't already arrived
		if (time.diff[index] < 1) {
			var percent = 100;
		} else {
			var percent = 100 - (time.diff[0] / time.interval(time.full()) * 100); 
		}
				
		var spokes = percent * 0.12; //the percentage converted to X/12
		spokes = Math.round(spokes); //the fraction rounded
		return spokes; //the number of spokes that should be lit.
		
	}
		
};


var run = function(){

	// Deprecated code for reading from a file instead of the MTA stream
	/*
	var data = fs.readFileSync("gtfs.pb");
	var message = FeedMessage.decode(data);
	
	if (!message.hasOwnProperty('entity')) return;
	
	for (var i = 0; i < message.entity.length; i++) {
	    var obj = message.entity[i].alert;
	
	    if (obj == null) continue;
	    //if (!obj.hasOwnProperty('stop_time_update')) continue;
	
		console.log(JSON.stringify(obj));
	}
	*/


	http.get(url, function(res) {
	    var data = [];
	
	    res.on("data", function(chunk) {
	        data.push(chunk);
	    });
	
	    res.on("end", function() {
	    	// Combine all the data chunks into a buffer and decode it
	        data = Buffer.concat(data);
	        var message = FeedMessage.decode(data);
	
	        if (!message.hasOwnProperty('entity')) return;

	        // clear out previous results before starting
	        time.diff = [];
	
	        for (var i = 0; i < message.entity.length; i++) {
	            // var obj = message.entity[i].alert;
	            var obj = message.entity[i].trip_update;
	
	            if (obj == null) continue;
	            if (!obj.hasOwnProperty('stop_time_update')) continue;

	            for (var j = 0; j < obj.stop_time_update.length; j++) {
	                // search for desired stop only
	                var stop_id = obj.stop_time_update[j].stop_id;
	                if (stop_id.substring(0, 4) != stationID) continue;

                    var arrival = obj.stop_time_update[j].arrival.time.low;
                    var d = new Date(arrival * 1000);

	                // console.log("i:", i, "id:", message.entity[i].id, "stop_id:", obj.stop_time_update[j].stop_id, "date:", d.toLocaleTimeString());
					// console.log("stop_id:", obj.stop_time_update[j].stop_id, "date:", d.toLocaleTimeString());

					//get time in minutes until next train and push difference between now + arrival to time.diff array
					var arrivalMins = d.getHours()*60 + d.getMinutes();
					var diff = arrivalMins - time.full();
					time.diff.push(diff);
	            }
	            
	        }

			// console.log(time.diff);
			// console.log('The train is now coming at intervals of ' + time.interval(time.full()) + ' minutes.');
			// console.log('The next train is ' + time.diff[0] + ' minutes away.');		
			// console.log('Spokes 0 through ' + time.numSpokesLit() + ' should be lit.');
			server.send(time.numSpokesLit());
	    });
	    
	});
};
		

setInterval(run, 30*1000);
