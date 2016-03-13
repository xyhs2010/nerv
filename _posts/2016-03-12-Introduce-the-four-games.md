---
title: game01-game04的说明
summary: 百度ife暑期训练营游戏班的四个游戏
excerpt: 百度ife暑期训练营游戏班的四个游戏
---

## 第一个游戏：剪刀石头布 ##
技术很简单，onclick事件上采用switch处理。

## 第二个游戏：2048 ##
比较复杂一点。在每一个动作之后，行和列的处理有相似之处，也有不同之处。我采取的方法是，将相似的数学处理部分写成一个函数。处理每一行时，先取出该行的4个表格单元，然后并不将这4个单元传入函数，而是将单元对应的值传入函数。换言之，传的不是引用，而是值。

## 第三个游戏：勇士斗怪兽 ##
照着[How to make a simple html5 canvas game](http://www.lostdecadegames.com/how-to-make-a-simple-html5-canvas-game/)写成的游戏

## 第四个游戏：接元宝 ##
移动端游戏。移动端的触摸事件名是touchstart、touchmove、touchend和touchcancel。
