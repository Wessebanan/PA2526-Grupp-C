<?php
    header('Content-Type: application/json');

function phpSetMode($player, $mode)
{
    $oldList = file_get_contents('json/playerMode.json');

    $tempArray = json_decode($oldList, true);
    $tempArray[$player]["Button"] = $mode;
    $resArray = json_encode($tempArray);

    $fp = fopen('json/playerMode.json', 'w');
    fwrite($fp, $resArray);
    fclose($fp);


    echo $resArray;
}

$aResult = array();

if( !isset($_POST['functionname']) ) { $aResult['error'] = 'No function name!'; }

if( !isset($_POST['arguments']) ) { $aResult['error'] = 'No function arguments!'; }

if( !isset($aResult['error']) ) {
    switch($_POST['functionname']) {
        case 'phpSetMode':
            //check if the arguments are correct
           if( !is_array($_POST['arguments']) || (count($_POST['arguments']) < 2) ) {
           }
           else {
               phpSetMode(($_POST['arguments'][0]), ($_POST['arguments'][1]));
           }
           break;

        default:
          echo 'alert("Error with rnning functions")';
           break;
    }
}

echo json_encode($aResult);


?>
