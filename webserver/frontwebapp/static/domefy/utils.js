// GetConfigurationJSON - Gets the JSON with configurations
function GetConfigurationJSON() {
  var resp = ajaxPost("", false, "/getConfigJSON");

  //console.log(resp.presets[0].name);
  return resp;
}

// ajaxPost - Perform a POST methode
function ajaxPost(data, sync, url) {
  var tmp = null;
  $.ajax({
    type: "POST",
    url: "http://" + serviceIPPort + url,
    async: sync,
    data: '{"command": "' + data + '"}',
    success: function(response) {
      if (response != "done") {
        var respJ = JSON.parse(response);
        tmp = respJ;
      }
    },
    dataType: "json"
  });
  return tmp;
}

// ajaxPost - Perform a POST methode, no template on data parameter
function ajaxPostRaw(data_raw, sync, url) {
  var tmp = null;
  $.ajax({
    type: "POST",
    url: "http://" + serviceIPPort + url,
    async: sync,
    data: data_raw,
    success: function(response) {
      if (response == "done") {
        console.log(data_raw);
      }
    },
    dataType: "json"
  });
  return tmp;
}

var last = +new Date();
// baudCommand - Similar to sendCommand, but has rate limit to send the data
function baudCommand(data, baudInterval, url) {
  const now = +new Date();
  if (now - last > baudInterval) {
    last = now;
    ajaxPost(data, true, url);
  }
}
