<?php
$nrOfPlayers = 4;
$nrOfModes = 4;

for ($i=0; $i < $nrOfPlayers; $i++) {

    echo "<div class='buttonField'> <h2>Player ";
    echo $i+1;
    echo "</h2></br>";
    for ($j=0; $j < $nrOfModes; $j++) {
        echo "<button id='playerButton' onclick='setMode(";
        echo $i;
        echo ",";
        echo $j;
        echo ")'>--";
        echo $j;
        echo "--</button>";
        //echo "<button id='playerButton' onclick='setMode(0,0)'>p mode </button>";
    };
    echo "</div></br> ";
};



 ?>
