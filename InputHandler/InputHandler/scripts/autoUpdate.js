async function autoUpdate(){

    while (true)
    {

        var $JSONList = $('#JSONList');

        $.getJSON("json/playerMode.json?" + new Date().getTime(), function(json) {
            console.log(json); // this will show the info it in firebug console
            var stringed = JSON.stringify(json);
            var response = JSON.parse(stringed);
            //console.log(response); // this will show the info it in firebug console

            $.each(response, function(i, itt){

                $JSONList.append( "</br>" + itt.Name + " has pressed: " + itt.Button + " :(())");
            })

        });

        setTimeout(function() {var a = 4++;}, (1000));

    }

}
