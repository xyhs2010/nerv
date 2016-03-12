var table = document.getElementById("table");
var text = document.getElementById("text");
var flagLose;
var flagWin = false;
const KEY_TABLE_HEIGHT = 4;
const KEY_TABLE_WIDTH = 4;
var colorTable = {
    0 : "#FFCCFF",
    2 : "#FFFFCC",
    4 : "#FFFF99",
    8 : "#FFFF66",
    16 : "#FFFF33",
    32 : "#FFFF00",
    64 : "#FFCC00",
    128 : "#FF9900",
    256 : "#FF6600",
    512 : "#FF3300",
    1024 : "#FF0000",
    2048 : "#FFFFFF"
};

var clientWidth = window.screen.width;
var clientHeight = window.screen.height;
var clientScale = clientWidth < clientHeight ? clientWidth : clientHeight;
document.documentElement.style.fontSize = Math.floor(clientScale / 32) + "px";

table.playUp = function () {
    for (var i = 0; i < this.childElementCount; i++) {
        var tr = this.childNodes[i];
        for (var j = 0; j < tr.childElementCount; j++) {
            var td = tr.childNodes[j];
            if (td.value) {
                td.innerHTML = td.value + "<br \>";
            } else {
                td.innerHTML = "<br \>";
            }
            td.style.backgroundColor = colorTable[td.value];
        }
    }
}

table.addNew = function (array) {
    // 可优化
    if (array == null) {
        array = new Array();
        for (var i = 0; i < KEY_TABLE_HEIGHT * KEY_TABLE_WIDTH; i++) {
            array.push(i);
        }
    }
    var num = Math.floor(Math.random() * array.length);
    i = array[num];
    var td = this.childNodes[Math.floor(i / KEY_TABLE_WIDTH)].childNodes[i % KEY_TABLE_WIDTH];
    if (td.value == 0) {
        td.value = 2;
    } else {
        alert("not zero in (" + Math.floor(i / KEY_TABLE_WIDTH) + "," + (i % KEY_TABLE_WIDTH) + ")");
    }
}

table.updateArray = function (nodeArray) {
    var array = new Array();
    for (var i = 0; i < nodeArray.length; i++) {
        array[i] = nodeArray[i].value;
    }
    var flagMedZero = false;
    var flagPos = false;
    var flagZero = false;
    for (var i = array.length - 1; i >= 0 ; i--) {
        if (array[i] == 0) {
            flagZero = true;
            array.splice(i, 1);
            if (flagPos) {
                flagMedZero = true;
            }
        } else {
            flagPos = true;
        }
    }
    flagLose = flagLose || flagZero;
    var result = this.calculate(array) || flagMedZero;
    var diff = nodeArray.length - array.length;
    for (var i = 0; i < diff; i++) {
        array.push(0);
    }
    for (var i = 0; i < nodeArray.length; i++) {
        nodeArray[i].value = array[i];
    }
    return result;
}

table.calculate = function (array) {
    var flag = false;
    for (var i = 0; i < array.length - 1; i++) {
        var elem1 = array[i];
        var elem2 = array[i + 1];
        if (elem1 != 0 && elem1 == elem2) {
            array.splice(i, 1);
            array[i] = elem1 * 2;
            if (array[i] == 2048) {
                flagWin = true
            }
            array.push(0);
            flag = true;
        }
    }
    return flag;
}

table.handleKeyDown = function(keyNum) {
    flagLose = false;
    var ifVert = false;
    var ifInv = false;
    switch (keyNum) {
        case 37:
            break;
    
        case 38:
            ifVert = true;
            break;
            
        case 39:
            ifInv = true;
            break;
        
        case 40:
            ifInv = true;
            ifVert = true;
            break;
            
        default:
            return;
            break;
    }
    var nodeArray = new Array();
    var result = false;
    if (ifVert) {
        for (var i = 0; i < KEY_TABLE_WIDTH; i++) {
            for (var j = 0; j < KEY_TABLE_HEIGHT; j++) {
                if (ifInv) {
                    nodeArray[j] = this.childNodes[KEY_TABLE_HEIGHT - 1 - j].childNodes[i];
                } else {
                    nodeArray[j] = this.childNodes[j].childNodes[i];
                }
            }
            result = this.updateArray(nodeArray) || result;
        }
    } else {
        for (var i = 0; i < KEY_TABLE_HEIGHT; i++) {
            for (var j = 0; j < KEY_TABLE_WIDTH; j++) {
                if (ifInv) {
                    nodeArray[j] = this.childNodes[i].childNodes[KEY_TABLE_WIDTH - 1 - j];
                } else {
                    nodeArray[j] = this.childNodes[i].childNodes[j];
                }
            }
            result = this.updateArray(nodeArray) || result;
        }
    }
    if (result) {
        var tmpArray = new Array();
        for (var i = 0; i < KEY_TABLE_HEIGHT; i++) {
            for (var j = 0; j < KEY_TABLE_WIDTH; j++) {
                if (this.childNodes[i].childNodes[j].value == 0) {
                    tmpArray.push(i * KEY_TABLE_WIDTH + j);
                }
            }
        }
        table.addNew(tmpArray);
        table.playUp();
        if (flagWin) {
            text.innerHTML = "You Win! Keep on challeng!"
        }
    } else {
        if (!flagLose) {
            if (!flagWin) {
                text.innerHTML = "You lose...";
            } else {
                text.innerHTML = "You Win! Game over!";
            }
        }
    }
}

document.onkeydown = function(e) {
    var keyNum = window.event ? e.keyCode : e.which;
    table.handleKeyDown(keyNum);
}

for (var i = 0; i < KEY_TABLE_HEIGHT; i++) {
    var tr = document.createElement("tr");
    for (var j = 0; j < KEY_TABLE_WIDTH; j++) {
        var td = document.createElement("td");
        td.value = 0;
        tr.appendChild(td);
    }
    table.appendChild(tr);
}

table.addNew();
table.playUp();