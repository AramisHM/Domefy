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

    var channelSel = document.getElementById("channelSelector").value;
    if (channelSel == "CPP") {
        cmd = "CPP;" + cmd;
    }

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
function RunURLFromSelector() {
    var urlselected = document.getElementById("urlSelectorbb").value;

    var finalcmd;
    if (urlselected == "custom_url") {
        finalcmd = "CPP;RUNURL;" + document.getElementById("customUrlInput").value + ";";
    } else {
        finalcmd = "CPP;RUNURL;" + urlselected + ";";
    }

    ajaxPost(finalcmd, true, commandsEndpoint);
}
function StopURL() {
    ajaxPost("CPP;STOPCEFMEDIA;0;", true, commandsEndpoint);
}
function SlideZoom(zoom) {
    console.log("SLIDEZOOM;" + zoom + ";")
    var cmd = "SLIDEZOOM;" + zoom + ";";
    var channelSel = document.getElementById("channelSelector").value;
    if (channelSel == "CPP") {
        cmd = "CPP;" + cmd;
    }

    ajaxPost(cmd, true, commandsEndpoint);
}

function ChangeBrowserMesh(cb) {
    var cmdset360 = "";
    var checked = document.getElementById("user360browsercb").checked
    if (checked) { // activate the 36- browwser
        cmdset360 = "CPP;USER360BROWSER;1;";
    } else {
        cmdset360 = "CPP;USER360BROWSER;0;";
    }
    ajaxPost(cmdset360, true, commandsEndpoint);
}

function ChangeBrowserMeshCubic(cb) {
    var cmdset360 = "";
    var checked = document.getElementById("user360cubebrowsercb").checked
    if (checked) { // activate the 36- browwser
        cmdset360 = "CPP;USER360CUBEBROWSER;1;";
    } else {
        cmdset360 = "CPP;USER360CUBEBROWSER;0;";
    }
    ajaxPost(cmdset360, true, commandsEndpoint);
}