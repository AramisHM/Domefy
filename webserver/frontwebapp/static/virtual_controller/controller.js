/*
 * File: controller.js
 * Project: FpMED - Framework for Distributed Multiprojection Systems
 * File Created: Friday, 9th August 2019 8:28:19 pm
 * Author: Aramis Hornung Moraes (aramishm@gmail.com)
 * -----
 * Last Modified: Saturday, 10th August 2019 7:52:38 pm
 * Modified By: Aramis Hornung Moraes (aramishm@gmail.com>)
 * -----
 * Copyright 2014 - 2019 Aramis Hornung Moraes
 */
let last = +new Date();

var thumbstick1 = {
  position: {
    last: {
      x: 0.0,
      y: 0.0
    },
    current: {
      x: 0.0,
      y: 0.0
    },
    delta: {
      x: 0.0,
      y: 0.0
    }
  },
  baudInterval: 25
};

function UpdateStickDisplay(element) {
  element.style.position = "absolute";
  var p = thumbstick1.position.current;
  element.style.left = p.x + "px";
  element.style.top = p.y + "px";
}

window.addEventListener("touchstart", function(event) {
  var lp = thumbstick1.position.last;
  lp.x = event.touches[0].clientX;
  lp.y = event.touches[0].clientY;
});

function ViewDrag(event) {
  var cp = thumbstick1.position.current;
  var lp = thumbstick1.position.last;
  var mv = thumbstick1.position.delta;
  cp.x = Math.round(event.touches[0].clientX);
  cp.y = Math.round(event.touches[0].clientY);
  UpdateStickDisplay(document.getElementById("stick2"));
  mv.x = Math.floor(cp.x - lp.x);
  mv.y = Math.floor(cp.y - lp.y);
  var cmd = "VIEW;" + mv.x + ";" + mv.y + ";";
  console.log(cmd);
  baudCommand(cmd, thumbstick1.baudInterval, commandsEndpoint);
  lp.x = cp.x;
  lp.y = cp.y;
}
