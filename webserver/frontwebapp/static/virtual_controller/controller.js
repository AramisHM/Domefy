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

var touchCound = 0;
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
  baudInterval: 25,
  elementName: "stick1",
  touchIndex: -1
};

function UpdateStickDisplay(thumbstick) {
  var element = document.getElementById(thumbstick.elementName);
  element.style.position = "absolute";
  var p = thumbstick.position.current;
  element.style.left = element.parentElement.style.left + p.x + "px";
  element.style.top = element.parentElement.style.top + p.y + "px";
}

function ViewDrag(event) {
  var cp = thumbstick1.position.current;
  var lp = thumbstick1.position.last;
  var mv = thumbstick1.position.delta;
  var tIndex = thumbstick1.touchIndex;
  cp.x = Math.round(event.targetTouches[tIndex].clientX);
  cp.y = Math.round(event.targetTouches[tIndex].clientY);
  UpdateStickDisplay(thumbstick1);
  mv.x = Math.floor(cp.x - lp.x);
  mv.y = Math.floor(cp.y - lp.y);
  var cmd = "VIEW;" + mv.x + ";" + mv.y + ";";
  console.log(cmd);
  baudCommand(cmd, thumbstick1.baudInterval, commandsEndpoint);
  lp.x = cp.x;
  lp.y = cp.y;
}
//  ----  ----  ----  ----  ----  ----  ----  ----  ----  ----  ---- second stick logic
var thumbstick2 = {
  position: {
    start: {
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
  baudInterval: 180,
  updateControls: false,
  elementName: "stick2",
  touchIndex: -1
};

function MoveDrag(event) {
  var cp = thumbstick2.position.current;
  var ps = thumbstick2.position.start;
  var mv = thumbstick2.position.delta;
  var tIndex = thumbstick2.touchIndex;
  cp.x = Math.round(event.targetTouches[tIndex].clientX);
  cp.y = Math.round(event.targetTouches[tIndex].clientY);
  UpdateStickDisplay(thumbstick2);
  mv.x = Math.floor(cp.x - ps.x);
  mv.y = Math.floor(cp.y - ps.y);
  var cmd = "MOVE;" + mv.x + ";" + mv.y + ";";
  console.log(cmd);
  baudCommand(cmd, thumbstick2.baudInterval, commandsEndpoint);
}

// loopStick2 - infinitly check stick
function loopStick2() {
  if (thumbstick2.updateControls) {
    // send last state
    var mv = thumbstick2.position.delta;
    var cmd = "MOVE;" + mv.x + ";" + mv.y + ";";
    baudCommand(cmd, thumbstick2.baudInterval, commandsEndpoint);
  }
  setTimeout(loopStick2, thumbstick2.baudInterval);
}
loopStick2(""); // start loop

function silenceLoopStick2() {
  ajaxPost("NMOVE;0;0;", true, commandsEndpoint);
  thumbstick2.updateControls = false;
  thumbstick2.position.delta.x = 0.0;
  thumbstick2.position.delta.y = 0.0;
}

function RegisterIndex(stick, event) {
  var tIndex = event.targetTouches.length - 1;
  if (stick == 1) {
    thumbstick1.touchIndex = tIndex;
    var lp = thumbstick1.position.last;
    lp.x = event.targetTouches[tIndex].clientX;
    lp.y = event.targetTouches[tIndex].clientY;
  } else if (stick == 2) {
    thumbstick2.touchIndex = tIndex;
    var ps = thumbstick2.position.start;
    ps.x = event.targetTouches[tIndex].clientX;
    ps.y = event.targetTouches[tIndex].clientY;
  }
}

function JumpCommand() {
  ajaxPost("JUMP;0;", true, commandsEndpoint);
}