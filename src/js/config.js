function getUrlParameter(name) {
	return decodeURIComponent((RegExp(name + '=' + '([^&]*)(&|$)').exec(
			location.search) || [ , null ])[1]);
}

function escapeHtml(unsafe) {
    return unsafe
         .replace(/&/g, "&amp;")
         .replace(/</g, "&lt;")
         .replace(/>/g, "&gt;")
         .replace(/"/g, "&quot;")
         .replace(/'/g, "&#039;");
 }

$(function() {
  var text = $('#messages');
  var ws = new WebSocket("ws://espressif/ws");
  ws.onopen = function(event) {
    
  };
  ws.onmessage = function(event) {
    setTimeout(function() {
        text.append(escapeHtml(event.data + "\n"));
      },0);
  }
  ws.onclose = function(event) {
    
  }
});
