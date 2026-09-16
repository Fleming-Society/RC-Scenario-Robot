// Object to track active keys for keyboard control
const activeKeys = {};

// Function to send a command to the ESP endpoint /cmd?value=...
function sendCommand(cmd) {
  fetch("/cmd?value=" + cmd)
    .then(response => response.text())
    .then(data => console.log("Sent command: " + data))
    .catch(err => console.error(err));
}

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
  // Clear all button states on release (or you can handle individual buttons if desired)
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
