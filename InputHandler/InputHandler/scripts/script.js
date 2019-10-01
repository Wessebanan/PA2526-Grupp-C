 var socket = new WebSocket("ws://192.168.1.45:80");

    var selectedTileMsg = "0001100100";
    var selectedButtonMsg = "0002100";

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

    function ping()
    {
        socket.send("ping");
    }
    function start()
    {
        socket.send("start");

        openCommand();
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
		//socket.send(msg);
    }

    function setButton(x)
    {
        var player = "0";
        var action = "002";
        x += 100;
        var Button = x.toString();


        var msg = player.concat(action.concat(Button));

        console.log("Saved buttonMsg: " + msg);
        selectedTileMsg = msg;
        //socket.send(msg);
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