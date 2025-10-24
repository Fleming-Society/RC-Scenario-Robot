// Open a WebSocket connection to the ESP32 (port 81)
var connection = new WebSocket("ws://" + window.location.hostname + ":81/");

function toPlanner() {
  stopPath()
  window.location = "/planner";
}

function toControl() {
  window.location = "/";
}

connection.onopen = function () {
  console.log("WebSocket is open now.");
};

connection.onclose = function () {
  console.log("WebSocket is closed now.");
};

connection.onerror = function (error) {
  console.error("WebSocket error:", error);
};

// Function to send a command via the WebSocket
function sendCommand(cmd) {
  if (connection.readyState === WebSocket.OPEN) {
      connection.send(cmd.toString());
      console.log("Sent command: " + cmd);
  } else {
      console.error("WebSocket is not open. Command not sent.");
  }
}

// Object to track active keys for keyboard control
const activeKeys = {};

// Determine which command to send based on active keys
function updateCommand() {
  const up = activeKeys['ArrowUp'];
  const down = activeKeys['ArrowDown'];
  const left = activeKeys['ArrowLeft'];
  const right = activeKeys['ArrowRight'];

  if (up && left) {
    sendCommand(5); // Diagonal Forward Left
  } else if (up && right) {
    sendCommand(6); // Diagonal Forward Right
  } else if (down && left) {
    sendCommand(7); // Diagonal Backward Left
  } else if (down && right) {
    sendCommand(8); // Diagonal Backward Right
  } else if (up) {
    sendCommand(1); // Forward
  } else if (down) {
    sendCommand(2); // Backward
  } else if (left) {
    sendCommand(3); // Left
  } else if (right) {
    sendCommand(4); // Right
  } else {
    sendCommand(0); // Stop
  }
}

// Keyboard event listeners for arrow keys
document.addEventListener('keydown', function(event) {
  if (event.repeat) return;
  if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight'].includes(event.key)) {
    activeKeys[event.key] = true;
    updateCommand();
  }
});

document.addEventListener('keyup', function(event) {
  if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight'].includes(event.key)) {
    delete activeKeys[event.key];
    updateCommand();
  }
});

// For on-screen button controls, maintain a similar state
let buttonStates = { up: false, down: false, left: false, right: false };

function buttonPress(direction) {
  buttonStates[direction] = true;
  updateButtonCommand();
}

function buttonRelease() {
  // Clear all button states on release (or handle individually if needed)
  buttonStates = { up: false, down: false, left: false, right: false };
  updateButtonCommand();
}

function updateButtonCommand() {
  const up = buttonStates['up'];
  const down = buttonStates['down'];
  const left = buttonStates['left'];
  const right = buttonStates['right'];

  if (up && left) {
    sendCommand(5); // Diagonal Forward Left
  } else if (up && right) {
    sendCommand(6); // Diagonal Forward Right
  } else if (down && left) {
    sendCommand(7); // Diagonal Backward Left
  } else if (down && right) {
    sendCommand(8); // Diagonal Backward Right
  } else if (up) {
    sendCommand(1); // Forward
  } else if (down) {
    sendCommand(2); // Backward
  } else if (left) {
    sendCommand(3); // Left
  } else if (right) {
    sendCommand(4); // Right
  } else {
    sendCommand(0); // Stop
  }
}

// 0 - up, 1 - down, 2 - left, 3 - right
var commandQueue = []
var timeQueue = []
var timeOuts = []

function addCommand(direction) {
  console.log("Adding command with direction: " + direction)
  var arrow
  var symbols = ['\u21E7', '\u21E9', '\u21E6', '\u21E8']
  try {
    arrow = symbols[direction]
  } catch (error) {
    console.log("Invalid Direction")
    alert("Invalid Direction")
    return;
  }
  flag = 1
  while(flag){
    var time = prompt("Please enter time in seconds", 0)
    if (time != null && time > 0){
      flag = 0
      commandQueue.push(direction)
      timeQueue.push(time)

      const path = document.createElement("button");
      const arrowSym = document.createTextNode(arrow + "\n" + time);
      path.append(arrowSym)
      path.className = "pathBox"
      const pathList = document.getElementById("pathDiv");
      pathList.append(path)

      updateBoxIndex()
    } else {
      alert("Invalid input, command time must be greater than 0. Input was: " + time)
      console.log("Invalid input, command time must be greater than 0. Input was: " + time)
    }
  }
}

function updateBoxIndex(){
  var pathBoxes = document.getElementsByClassName("pathBox");
  for (let i = 0; i < pathBoxes.length; i++) { 
    pathBoxes[i].addEventListener('mousedown', managePath);
    pathBoxes[i].addEventListener('touchstart', managePath);
    pathBoxes[i].dataset.index = i;
  }
}

function runPath(){
  stopPath()
  timeOuts = []
  var currTime = 0;
  for(i = 0; i < commandQueue.length; i++){
    console.log(currTime + 500)
    timeOuts[2*i] = setTimeout(sendCommand, currTime + 500, commandQueue[i]+1)
    currTime += 500
    console.log(currTime + (timeQueue[i]*1000.0))
    timeOuts[2*i+1] = setTimeout(sendCommand, currTime + (timeQueue[i]*1000.0), 0)
    currTime += (timeQueue[i]*1000.0)
  }
}

function stopPath(){
  for(i=0; i < timeOuts.length; i++){
    clearTimeout(timeOuts[i])
  }
  sendCommand(0)
}

function clearPath(){
  stopPath()
  toPlanner()
}

function managePath(event) {
  var index = event.target.dataset.index
  console.log("Element Index:", index);  // Logs the assigned ID
  
  var input = prompt("Edit command time. Enter 0 to delete the command")
  console.log("Input is: " + input)
  if(input == null || input < 0){
    console.log("Invalid input")
    return
  }
  if(input == 0){
    commandQueue.splice(index, 1)
    timeQueue.splice(index, 1)
    event.target.remove()
    updateBoxIndex()
    return
  }
  timeQueue[index] = input
  event.target.innerText = event.target.innerText.substring(0, event.target.innerText.length - 1) + input;
}