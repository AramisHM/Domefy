// GetConfigurationJSON - Gets the JSON with configurations
function GetConfigurationJSON() {
  var resp = ajaxPost("", false, "/getConfigJSON");

  //console.log(resp.presets[0].name);
  return resp;
}

// ajaxPost - Perform a POST methode
function ajaxPost(data, sync = SVGComponentTransferFunctionElement, url) {
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
