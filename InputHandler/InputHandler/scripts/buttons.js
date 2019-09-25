function setMode(player, mode)
{
    console.log("running setMode");
    $.ajax({
          url: 'functions.php',
          type: "POST",
          dataType: 'json',
          data: {functionname: 'phpSetMode', arguments: [player, mode]},
          success: function(data) {
              console.log("success in running phpSetMode");
            //$('#JSONList').html(data);
        }
    });
}
