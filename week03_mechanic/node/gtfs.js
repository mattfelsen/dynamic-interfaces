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

var stationID = "L02"; // 6th Ave
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
                if (stop_id.substring(0, 3) == stationID) {
                    var arrival = obj.stop_time_update[j].arrival.time.low;
                    var d = new Date(arrival * 1000);

                    // console.log("i:", i, "id:", message.entity[i].id, "stop_id:", obj.stop_time_update[j].stop_id, "date:", d.toLocaleTimeString());
                    console.log("stop_id:", obj.stop_time_update[j].stop_id, "date:", d.toLocaleTimeString());
                }
            }
            //console.log(JSON.stringify(myMessage.entity[i]) + ',');
            //if (i > 5) break;
            
        }
    });
    
});