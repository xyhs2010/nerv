/**
 * Created by xyhs on 16/3/6.
 */
const CAT_SPEED = 256;
const MONEY_SPEED = 512;
var canvas = document.createElement("canvas");
ctx = canvas.getContext("2d");
canvas.height = document.documentElement.clientHeight;
canvas.width = document.documentElement.clientWidth;
document.body.appendChild(canvas);
document.body.style.margin = 0;
document.body.style.padding = 0;

ctx.fillStyle = "rgb(255, 128, 128)";
ctx.fillRect(0, 0, canvas.width, canvas.height);

function CreateImage(src, posFunc) {
    var object = new Image();
    object.ready = false;
    object.onload = function () {
        object.ready = true;
        object.posFunc = posFunc;
        object.posFunc(object.width, object.height);
        //object.locationX = (canvas.width - object.width) / 2;
        //object.locationY = (canvas.height - object.height);
    }
    object.src = src;
    return object;
}

// Create Cat
var cat = new CreateImage("images/cat.png", function (width, height) {
    this.locationX = (canvas.width - width) / 2;
    this.locationY = canvas.height - height;
});

// Create money
var createMoney = function () {
    var money = new CreateImage("images/money.png", function (width, height) {
        this.locationY = 0;
        this.locationX = Math.random() * (canvas.width - width);
    });
    return money;
};
var money = createMoney();
var moneyCaught = 0;

// Listener
var leftTouch = false;
var rightTouch = false;

addEventListener("touchstart", function (e) {
    e.preventDefault();
    if (e.touches != null) {
        var x = e.touches[0].clientX;
        var y = e.touches[0].clientY;
        if (y >= canvas.height - cat.height) {
            if (x < canvas.width / 2) {
                leftTouch = true;
            } else {
                rightTouch = true;
            }
        }
    }
}, false);

addEventListener("touchend", function (e) {
    leftTouch = false;
    rightTouch = false;
}, false);

var update = function (modifier) {
    if (leftTouch == true) {
        cat.locationX -= CAT_SPEED * modifier;
        if (cat.locationX < 0) {
            cat.locationX = 0;
        }
    }
    if (rightTouch == true) {
        cat.locationX += CAT_SPEED * modifier;
        if (cat.locationX > canvas.width - cat.width) {
            cat.locationX = canvas.width - cat.width;
        }
    }

    money.locationY += MONEY_SPEED * modifier;
    if (money.locationY >= canvas.height) {
        delete money;
        money = createMoney();
    } else if (cat.locationX - money.locationX < money.width &&
        money.locationX - cat.locationX < cat.width &&
        cat.locationY - money.locationY < money.height &&
        money.locationY - cat.locationY < cat.height) {
        delete money;
        moneyCaught += 1;
        money = createMoney();
    }
};

var render = function () {
    ctx.fillStyle = "rgb(255, 128, 128)";
    ctx.fillRect(0, 0, canvas.width, canvas.height);
    if (cat.ready) {
        ctx.drawImage(cat, cat.locationX, cat.locationY);
    }
    if (money.ready) {
        ctx.drawImage(money, money.locationX, money.locationY);
    }

    ctx.fillStyle = "rgb(250, 250, 250)";
    ctx.font = "24px Helvetica";
    ctx.textAlign = "left";
    ctx.textBaseLine = "top";
    ctx.fillText("Money caught: " + moneyCaught, 32, 32);
};

var main = function () {
    var now = Date.now();
    var delta = now - then;

    update(delta / 1000);
    render();

    then = now;

    requestAnimationFrame(main);
};

//requestAnimationFrame = window.requestAnimationFrame || window.webkitRequestAnimationFrame || window.msRequestAnimationFrame || window.mozRequestAnimationFrame;
requestAnimationFrame = (function(){
    return window.requestAnimationFrame ||
        window.webkitRequestAnimationFrame ||
        window.mozRequestAnimationFrame ||
        function( callback ){
            window.setTimeout(callback, 1000 / 60);
        };
})();

var then = Date.now();
main();