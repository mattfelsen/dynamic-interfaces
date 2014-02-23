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
// var stationID = "L08"; // Bedford Av
// var stationID = "L03"; // Union Square

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


//time handlers
var time = {
	
	now: function (){	return new Date();						},
	
	hour: function(){	return this.now().getHours()*60;		},
	
	min: function(){	return this.now().getMinutes();			},
	
	full: function(){	return this.min() + this.hour(); 		},
	
	diff: [],
	
	interval: function(time){ //accepts time as minutes. ie: 4:02AM = 4*60 + 2.

		switch(true) {
			case time <= 362: //if time is between midnight at 6:02 AM
				return 11; //return 11, which is the average for this block of time.
				break;
			
			case time > 362 && time <= 847: //if time is between 6:03 AM and 2:07 PM
				return 5; //return 5
				break;

			case time > 847 && time <= 1201: //if time is between 2:07 PM and 8:01 PM
				return 7;
				break;
			
			case time > 1201 && time <= 1439: //if time is between 8:01 PM and 11:59 PM:
				return 5; 
				break;
			
			default: 
				return 12;
				break;
		}
	},
		
	numSpokesLit: function(){
		var percent = 100 - (time.diff[0] / time.interval(time.full()) * 100); //how close the train is to arriving (ie: if it's 1 minute away and the train is coming at intervals of 5 minutes,  the train is 80% here).
		
		var spokes = percent * 0.12; //the percentage converted to X/12
		return Math.round(spokes); //the fraction rounded
		return spokes; //the number of spokes that should be lit.
		
	},
		
};







http.get(url, function(res) {
    var data = [];

    res.on("data", function(chunk) {
        data.push(chunk);
    });

    res.on("end", function() {
    	// Combine all the data chunks into a buffer and decode it
        data = Buffer.concat(data);
        var message = FeedMessage.decode(data);
        // console.log(message);

        if (!message.hasOwnProperty('entity')) return;

        for (var i = 0; i < message.entity.length; i++) {
            // var obj = message.entity[i].alert;
            var obj = message.entity[i].trip_update;

            if (obj == null) continue;
            if (!obj.hasOwnProperty('stop_time_update')) continue;

            // console.log(JSON.stringify(obj));
            // console.log(obj);
            
            for (var j = 0; j < obj.stop_time_update.length; j++) {
                // search for desired stop only
                var stop_id = obj.stop_time_update[j].stop_id;
                if (stop_id.substring(0, 4) == stationID) {
                    var arrival = obj.stop_time_update[j].arrival.time.low;
                    var d = new Date(arrival * 1000);

                    // console.log("i:", i, "id:", message.entity[i].id, "stop_id:", obj.stop_time_update[j].stop_id, "date:", d.toLocaleTimeString());

				//	console.log("stop_id:", obj.stop_time_update[j].stop_id, "date:", d.toLocaleTimeString());
					
			
			
					//get time in minutes until next train and push difference between now + arrival to time.diff array
					var arrivalMins = d.getHours()*60 + d.getMinutes();
					var diff = arrivalMins - time.full();
					time.diff.push(diff);
					

                }
            }
            //console.log(JSON.stringify(myMessage.entity[i]) + ',');
            //if (i > 5) break;
            
            
        }

        console.log('The train is now coming at intervals of ' + time.interval(time.full()) + ' minutes.');
        console.log('The next train is ' + time.diff[0] + ' minutes away.');
        console.log('Spokes 0 through ' + time.numSpokesLit() + ' should be lit.');
    });
    
});


