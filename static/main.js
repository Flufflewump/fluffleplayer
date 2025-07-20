let socket = new WebSocket("ws://192.168.100.151:3000/ws");

socket.onopen = ()=>{
    console.log('open')
}
socket.onerror = (e)=>{
    console.log('error',e)
}
socket.onclose = (e)=>{
    console.log('close', e)
}
socket.onmessage = (e)=>{
    const msg = JSON.parse(e.data);

    if (msg.type == "file list") {
        console.log('list');

        let fileSelect = document.getElementById("file_select");

        fileSelect.innerHTML = "";

        msg.list.forEach(function(name) {
            let newOption = document.createElement("option");
            newOption.text = name;
            fileSelect.add(newOption);
      });
    }
}


function sendMessage(message) {
  socket.send(JSON.stringify({type : message}));
}


function loadFile() {
  let newFile = document.getElementById("file_select").value;

  socket.send(JSON.stringify( { type : 'load file', file : newFile } ));
}


function loadNext() {
  document.getElementById("file_select").selectedIndex += 1;
  loadFile();
}


function changeDir() {
  let newDir = window.prompt("Enter directory");
  socket.send('new directory', newDir);
}

/*
socket.on('file list', function(msg) {

  let fileSelect = document.getElementById("file_select");

  fileSelect.innerHTML = "";

  msg.forEach(function(file) {
    let newOption = document.createElement("option");
    newOption.text = file;
    fileSelect.add(newOption);
  });

});

socket.on('paused', function(paused) {
  document.getElementById('play_pause').src = (paused ? "/img/play.png" : "/img/pause.png");
});

socket.on('load file', function(file) {
  document.getElementById('file_select').value = file;
});

socket.on('subs', function(enabled) {
  document.getElementById('sub_button').src = (enabled ? "/img/subson.png" : "/img/subs.png");
});
*/