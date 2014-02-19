# Reinterpreting a Mechanic
## A new, physical life for progress indicators

Write a project description here!!


### Arduino
#### wiFlyHello
Simple program to connect to a server. Says "Hello from WiFly" after 5 seconds. Upon receiving messages it will try and parse an integer. If it receives a 1 it will turn on the pin 13 LED. Otherwise it turns it off.

### Node
#### server.js
Simple server which listens on port 9001. It will log opening & closing of connections. Every 5 seconds it writes a 1 or 0 to all connected clients.

#### gtfs.js
Script which pulls realtime GTFS data from the MTA and looks for a specific station ID (set at the top). Requires protobufjs which is included in the repo but can be installed with `npm install protobusjs`.
