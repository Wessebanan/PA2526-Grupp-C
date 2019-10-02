function getName()
{
  $.getJSON("json/data.json", function(json) {
      console.log(json); // this will show the info it in firebug console
      var stringed = JSON.stringify(json);
      var response = JSON.parse(stringed);
      console.log(response.name); // this will show the info it in firebug console

  });
};
function getData()
{
  $.getJSON("json/playerMode.json", function(json) {
      console.log(json); // this will show the info it in firebug console
      var stringed = JSON.stringify(json);
      var response = JSON.parse(stringed);
      console.log(response[2].Name); // this will show the info it in firebug console

  });
};


function myFunction()
{
    //getData();
    document.getElementById("test").innerHTML = "Pressed";
    document.getElementById("JSONList").innerHTML = "";

    var $JSONList = $('#JSONList');
    $.ajaxSetup({ cache:false });

    //?" + new Date().getTime()
    $.getJSON("json/playerMode.json", function(json) {
        console.log(json); // this will show the info it in firebug console
        var stringed = JSON.stringify(json);
        var response = JSON.parse(stringed);
        //console.log(response); // this will show the info it in firebug console

        $.each(response, function(i, itt){

            $JSONList.append( "</br>" + itt.Name + " has pressed: " + itt.Button + " ");
        })

    });
}


$(function (){
    var $JSONList = $('#JSONList');

    $.ajax({
        type:'GET',
        url:'json/playerMode.json',
        success: function(jsonInput){
            var stringed = JSON.stringify(jsonInput);
            var response = JSON.parse(stringed);
            $.each(response, function(i, response){

                $JSONList.append(response+ "</br>");
            })
        },
        error: function() {
            console.log("Error: Reading JSON data");
        }
    });



});


var clicks = 0;
function AddClick()
{

    $.ajax({
        url: 'addItem.php',
        type: "POST",
        dataType: 'json',
        data: {functionname: 'addItem', arguments: ["age", ++clicks,'json/data.json']},
        success: function(data) {
            $('#JSONList').html(data);
        }
    });
}
