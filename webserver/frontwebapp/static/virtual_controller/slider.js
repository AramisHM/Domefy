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


var thumbstick3 = {
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
    baudInterval: 20,
    updateControls: false,
    elementName: "stick3"
};


function RegisterTouchStartSliderStick3(event) {
    var lp = thumbstick3.position.last;
    lp.x = event.targetTouches[0].clientX;
    lp.y = event.targetTouches[0].clientY;
}
function MoveSlide(event) {
    var cp = thumbstick3.position.current;
    var lp = thumbstick3.position.last;
    var mv = thumbstick3.position.delta;
    cp.x = Math.round(event.targetTouches[0].clientX);
    cp.y = Math.round(event.targetTouches[0].clientY);
    UpdateStickDisplay(thumbstick3);
    mv.x = Math.floor(cp.x - lp.x);
    mv.y = Math.floor(cp.y - lp.y);
    var cmd = "SLIDEMOVE;" + mv.x + ";" + mv.y + ";";
    console.log(cmd);
    baudCommand(cmd, thumbstick3.baudInterval, commandsEndpoint);
    lp.x = cp.x;
    lp.y = cp.y;
}

function PreviousSlide() {
    ajaxPost("PREVIOUSSLIDE;0;", true, commandsEndpoint);
}
function NextSlide() {
    ajaxPost("NEXTSLIDE;0;", true, commandsEndpoint);
}

function SlideZoom(zoom) {
    console.log("SLIDEZOOM;" + zoom + ";")
    ajaxPost("SLIDEZOOM;" + zoom + ";", true, commandsEndpoint);
}