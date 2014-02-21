// Load the TCP Library
net = require('net');
 
// Keep track of the clients
var clients = [];
 
// Start a TCP Server
net.createServer(function (socket) {
 
  // Identify this client
  socket.name = socket.remoteAddress + ":" + socket.remotePort;

  // Put this new client in the list
  clients.push(socket);

  console.log('[CONN] New connection: ' + socket.name + ', total clients: ' + clients.length);
 
  // Send a nice welcome message and announce
  //socket.write("Welcome " + socket.name + "\n");
 
  // Handle incoming messages from clients.
  socket.on('data', function (data) {
    data = data.toString('utf8').trim();
    console.log('[RECV] ' + socket.name + ': ' + data);
  });

  socket.on('error', function(error) {
    if (error.code == "ECONNRESET") {
      clients.splice(clients.indexOf(socket), 1);
      console.log('[EROR] Disconnection: ' + socket.name + ', total clients: ' + clients.length)
    }
  });
 
  // Remove the client from the list when it leaves
  socket.on('end', function () {
    clients.splice(clients.indexOf(socket), 1);
    console.log('[END] Disconnection: ' + socket.name + ', total clients: ' + clients.length)
  });

 
}).listen(9001);
 
console.log("[INIT] Server running");

function writeToClients(message) {
  if (!clients.length) return;

  console.log('[SEND] ' + message);

  for (var i = 0; i < clients.length; i++) {
    console.log("[SEND] Checking index "+ i +" of length "+ clients.length);
    var socket = clients[i];

    socket.write(message + "\n");
  }
}

// var state = true;

var writeInterval = function() {

  var r = parseInt(Math.random()*99);
  var g = parseInt(Math.random()*99);
  var b = parseInt(Math.random()*99);

  writeToClients(r+","+g+","+b);

}

setInterval(writeInterval, 1*1000);