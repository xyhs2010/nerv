var input = document.getElementById("input");
var confirming = document.getElementById("confirm");
var output = document.getElementById("output");
var text = document.getElementById("text");
var inputNum = 1;
input.onclick = function () {
    output.style.visibility = "hidden";
    text.innerText = "";
    inputNum += 1;
    switch (input.getAttribute("src")) {
        case "./scissor.png":
            input.setAttribute("src", "./rock.png");
            break;
        case "./rock.png":
            input.setAttribute("src", "./cloth.png");
            break;
        case "./cloth.png":
            input.setAttribute("src", "./scissor.png");
            break;
    }
}
confirming.onclick = function () {
    var outputNum = Math.random();
    outputNum = Math.floor(outputNum * 3);
    var judgeNum = (inputNum - outputNum) % 3;
    switch (outputNum) {
        case 0:
            output.setAttribute("src", "./cloth.png");
            break;
        case 1:
            output.setAttribute("src", "./scissor.png");
            break;
        case 2:
            output.setAttribute("src", "./rock.png");
            break;
    }
    output.style.visibility = "visible";
    
    switch (judgeNum) {
        case 0:
            text.innerText = "Tie";
            break;
        case 1:
            text.innerText = "You Win!";
            break;
        case 2:
            text.innerText = "You Lose...";
            break;
    }
}
