 var socket = new WebSocket("ws://192.168.1.45:80");

// GLOBALS
var selectedTileMsg = "0001100100";
var selectedButtonMsg = "0002100";

var lastTileSelected = "0,0";
var lastTileColor = document.getElementById("0,0").style.backgroundColor;
var lastButtonSelected = "b0";
var lastButtonColor = document.getElementById("b0").style.backgroundColor;

// SOCKET FUNCTIONS
socket.onopen = function (event)
{
    socket.send("The connection was established");
};

socket.onmessage = function(e)
{
    var msg = e.data;

    if (msg == "name")
    {
        socket.send("My name is Ludvig");
    }
    else if(msg == "start")
    {
        console.log("We are wating for the client to start");
    }
    else if(msg[0] == 1)
    {
        document.getElementById("clientMsg1").innerHTML = msg;
    }
    else if(msg[0] == 2)
    {
        document.getElementById("clientMsg2").innerHTML = msg;
    }
    else if(msg[0] == 3)
    {
        document.getElementById("clientMsg3").innerHTML = msg;
    }
    else if(msg[0] == 4)
    {
        document.getElementById("clientMsg4").innerHTML = msg;
    }
    else
        document.getElementById("clientMsg0").innerHTML = msg;
}

// DEBUGGING FUNCTIONS
function ping()
{
    socket.send("ping");
}
function openPrep()
{

    document.getElementById('prepField').style.display = "block";
    document.getElementById('commandField').style.display = "none";
    document.getElementById('startfield').style.display = "none";
}

function openCommand()
{
    document.getElementById('prepField').style.display = "none";
    document.getElementById('commandField').style.display = "block";
    document.getElementById('startfield').style.display = "none";
}

// BUTTON FUNCTIONS
function start()
{
    socket.send("start");

    openCommand();
}

function setName(name)
{
    socket.send("0000" +  name.toString());
}

function setTile(x,y)
{
    var player = "0";
    var action = "001";
    x += 100;
    y += 100;
    var tileX = x.toString();
    var tileY = y.toString();


    var msg = player.concat(action.concat(tileX.concat(tileY)));

    console.log("Saved tileMsg: " + msg);
    selectedTileMsg = msg;

	x -= 100;
	y -= 100;
	var id = x + "," + y;
	//console.log(id);

	if(id != lastTileSelected)
		lastTileColor = document.getElementById(id).style.backgroundColor;

	document.getElementById(lastTileSelected).style.backgroundColor = lastTileColor;
	document.getElementById(id).style.backgroundColor = "#c00";
	lastTileSelected = id;
}

function setButton(x)
{
    var player = "0";
    var action = "002";
    x += 100;
    var Button = x.toString();


    var msg = player.concat(action.concat(Button));

    console.log("Saved buttonMsg: " + msg);
    selectedButtonMsg = msg;

	x -= 100;
	var bid = "b" + x;
	//console.log(id);

	if(bid != lastButtonSelected)
		lastButtonColor = document.getElementById(bid).style.backgroundColor;

	document.getElementById(lastButtonSelected).style.backgroundColor = lastButtonColor;
	document.getElementById(bid).style.backgroundColor = "green";
	lastButtonSelected = bid;
}

function setCommand(buttonCommand)
{
    var player = "0";
    var action = "003";
    var Button = buttonCommand.toString();


    var msg = player.concat(action.concat(Button));

    console.log(msg);
    socket.send(msg);
}

function sendButtonAndTile()
{
	console.log(selectedButtonMsg);
	socket.send(selectedButtonMsg);
	console.log(selectedTileMsg);
	socket.send(selectedTileMsg);
};