function generateGrid(x, y)
{
    var field = document.getElementById('test');
    //var field = document.getElementById('prepField');
    // creating button element
    var EvenRow = document.createElement('OL');
    var OddRow = document.createElement('OL');

    EvenRow.setAttribute("class", "even");
    OddRow.setAttribute("class", "odd");

    var tile  = document.createElement('LI');
    var tile1  = document.createElement('LI');

    tile.setAttribute("class", "hex");
    tile1.setAttribute("class", "hex");

    // creating text to be
    //displayed on button
    var text = document.createTextNode("0,0");

    // appending text to button
    tile.appendChild(text);
    tile1.appendChild(text);

    for (var i = 0; i < 4; i++)
    {
        EvenRow.appendChild(tile);
        OddRow.appendChild(tile1);
    }
    OddRow.appendChild(tile1);

    // appending button to div
    field.appendChild(EvenRow);
    field.appendChild(OddRow);



}
