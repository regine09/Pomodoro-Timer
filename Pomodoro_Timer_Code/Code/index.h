#ifndef INDEX_H
#define INDEX_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>POMODORO</title>

<style>
body { 
  font-family: Arial, sans-serif; 
  background: #f4f6f9; 
  display: flex; 
  justify-content: center; 
  align-items: center; 
  height: 100vh; 
  margin: 0; 
  transition: 0.4s ease;
}

/* ================= CARD ================= */
.card { 
  background: white; 
  width: 350px; 
  padding: 25px; 
  border-radius: 15px; 
  box-shadow: 0 10px 25px rgba(0,0,0,0.1); 
  text-align: center; 
  transition: 0.4s ease;
}

/* ================= BUTTONS ================= */
h2 { margin-top: 0; margin-bottom: 20px; }
.section { margin-bottom: 20px; font-weight: normal; }
.name-input { width: 280px; padding: 10px; font-size: 16px; text-align: center; border-radius: 6px; border: 1px solid #ccc; }
.time-input { width: 60px; padding: 6px; font-size: 16px; text-align: center; border-radius: 6px; border: 1px solid #ccc; }
.button-row { display: flex; flex-direction: column; gap: 10px; align-items: center; }
button { padding: 10px; font-size: 16px; border-radius: 8px; cursor: pointer; min-width: 300px; text-align: center; border: 2px solid transparent; transition: 0.3s ease; }
.next-btn { background-color: #1e2952; color: white; border-color: #1e2952; }
.next-btn:hover { background-color: #1e2952; }
.feeling-btn { background-color: white; color: inherit; border-width: 2px; border-style: solid; }
.feeling-btn.green { border-color: #28a745; color: #28a745; }
.feeling-btn.blue { border-color: #007bff; color: #007bff; }
.feeling-btn.orange { border-color: #fd7e14; color: #fd7e14; }
.feeling-btn.red { border-color: #c82333; color: #c82333; }
.feeling-btn:hover { background-color: #e0e0e0; }
.continue-btn { background-color: #1e2952; color: white; border-color: #1e2952; }
.continue-btn:hover { background-color: #1e2952; }
.save-btn { background-color: #28a745; color: white; border-color: #28a745; }
.save-btn:hover { background-color: #28a745; }
.reset-btn { background-color: #dc3545; color: white; border-color: #dc3545; }
.reset-btn:hover { background-color: #c82333; }
.back-btn { background-color: #1e2952; color: white; border-color: #1e2952;}
.back-btn:hover { background-color: #1e2952; }
#feelingCard, #pomodoroCard, #part3bad { display: none; }
.suggestion-box { background: #f8f9fa; padding: 12px; border-radius: 8px; margin-bottom: 10px; font-size: 14px; transition: 0.4s ease; }

/* ================= DARK MODE ================= */
body.dark-mode {
  background: #121212;
  color: #f1f1f1;
}

body.dark-mode .card {
  background: #1e1e1e;
  box-shadow: 0 10px 25px rgba(0,0,0,0.5);
}

body.dark-mode h2,
body.dark-mode h3,
body.dark-mode h4,
body.dark-mode p,
body.dark-mode label {
  color: #f1f1f1;
}

body.dark-mode #pomodoroForm .section {
  background: #2c2c2c !important;
  border: 1px solid #444 !important;
  border-radius: 10px;
}

body.dark-mode #pomodoroForm input,
body.dark-mode #pomodoroForm .time-input,
body.dark-mode #pomodoroForm .name-input {
  background: #2c2c2c;
  color: #f1f1f1;
  border: 1px solid #555;
}

body.dark-mode input {
  background: #2c2c2c;
  color: white;
  border: 1px solid #555;
}

body.dark-mode .feeling-btn {
  background-color: #2c2c2c;
}

body.dark-mode .feeling-btn:hover,
body.dark-mode .back-btn:hover {
  background-color: #1e2952;
}

body.dark-mode .back-btn {
  background-color: #1e2952;
  color: white;
  border-color: #1e2952;
}
body.dark-mode .feeling-btn.white { color: white; border-color: white; }
body.dark-mode .suggestion-box { background: #2c2c2c; }

/* ================= TOGGLE ================= */
.mode-toggle {
  position: fixed;
  top: 20px;
  right: 20px;
  width: 70px;
  height: 30px;
  background: #ddd;
  border-radius: 50px;
  display: flex;
  align-items: center;
  cursor: pointer;
  padding: 5px;
  transition: 0.4s ease;
  font-weight: bold;
  box-shadow: 0 5px 15px rgba(0,0,0,0.2);
}
.toggle-circle {
  width: 30px;
  height: 30px;
  background: white;
  border-radius: 50%;
  position: absolute;
  left: 5px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 15px;
  transition: 0.4s ease;
  box-shadow: 0 3px 8px rgba(0,0,0,0.2);
}

body.dark-mode .mode-toggle { background: black; color: white; }
body.dark-mode .toggle-circle { left: 45px; background: white; }
</style>
</head>
<body>

<!-- MODE TOGGLE -->
<div class="mode-toggle" onclick="toggleMode()">
  <div class="toggle-circle" id="circle"></div>
</div>

<!-- Name Input Card -->
<div class="card" id="nameCard">
  <h2 style="margin-bottom: 15px;">POMODORO TIMER</h2>
  <h3 style="font-size: 15px; font-weight: normal; margin-bottom: 10px; margin-top: 7px;">Welcome to Your Pomodoro Focus Session!</h3>
  <div class="section" style="text-align: center;">
    <label for="userName" style="display: block; margin-bottom: 8px;"></label>
    <input type="text" id="userName" name="userName" class="name-input" placeholder="Enter your name">
</div>
  <div class="button-row">
    <button type="button" class="next-btn" onclick="goToFeeling()">Next</button>
  </div>
</div>

<!-- Feeling Selection Card -->
<div class="card" id="feelingCard">
  <h2 id="greeting">Hello!</h2>
  <p>How are you feeling today?</p>
  <div class="button-row">
    <button class="feeling-btn green" onclick="startPomodoro('Motivated')">Motivated</button>
    <button class="feeling-btn blue" onclick="startPomodoro('Productive')">Productive</button>
    <button class="feeling-btn orange" onclick="startPomodoro('Tired')">Tired</button>
    <button class="feeling-btn red" onclick="startPomodoro('Stressed')">Stressed</button>
    <button class="back-btn" onclick="goBack()">Back</button>
  </div>
</div>

<div class="card" id="part3bad">
    <h3>Take a Moment....</h3>
    <div class="suggestion-box" style="text-align: left;">
  <h4 style="margin-top: 5px;">I hope youll consider this suggestion.</h4>
  <ul style="padding-left: 20px; margin: 0;">
    <li>Take 3 deep breaths</li>
    <li>Drink water</li>
    <li>Stretch for 2 minutes</li>
    <li>Clear your desk</li>
  </ul>
</div>
    <div class="suggestion-box" style="text-align: left;">
  <ul style="padding-left: 20px; margin: 0;">
    <li>Its okay to feel tired sometimes.</li>
    <li>You are capable of more than you think!</li>
  </ul>
</div>
    <div class="button-row" style="flex-direction: row; gap: 10px; justify-content: center;">
    <button class="back-btn" style="flex: 1; min-width: 120px; max-width: 150px;" onclick="goBackFromSuggestion()">Back</button>
    <button class="continue-btn" style="flex: 1; min-width: 120px; max-width: 150px;" onclick="goToStudy()">Continue</button>
</div>
</div>

<!-- Pomodoro Timer Card -->
<div class="card" id="pomodoroCard">
<h2>POMODORO TIMER</h2>

<form id="pomodoroForm">
<div class="section" style="border: 1px solid #ccc; border-radius: 10px; padding: 5px; background: #f8f9fa; width: 340px; margin: 0 auto; margin-bottom: 10px;">
  <h3 style="margin-top: 0; margin-bottom: 10px;">Total Study Time</h3>
  <div style="display: flex; gap: 5px; align-items: center; justify-content: center;">
    <input type="number" name="studyHour" class="time-input" min="0" placeholder="hr" style="text-align: center;">
    :
    <input type="number" name="studyMinute" class="time-input" min="0" max="59" placeholder="min" style="text-align: center;">
  </div>
</div>

<div style="display: flex; gap: 10px; justify-content: center; width: 100%;">
  
  <div class="section" style="border: 1px solid #ccc; border-radius: 10px; padding: 5px; background: #f8f9fa; flex: 0 0 45%;">
    <h3 style="margin-top: 0; margin-bottom: 10px; text-align: center;">Study Duration</h3>
    <div style="display: flex; gap: 1px; align-items: center; justify-content: center;">
      <input type="number" name="focusHour" class="time-input" min="0" max="59" placeholder="hr" style="text-align: center;"> 
      :
      <input type="number" name="focusMinute" class="time-input" min="0" max="59" placeholder="min" style="text-align: center;">
    </div>
  </div>

  <div class="section" style="border: 1px solid #ccc; border-radius: 10px; padding: 5px; background: #f8f9fa; flex: 0 0 45%;">
    <h3 style="margin-top: 0; margin-bottom: 10px; text-align: center;">Study Break</h3>
    <div style="display: flex; gap: 1px; align-items: center; justify-content: center;">
      <input type="number" name="breakHour" class="time-input" min="0" max="59" placeholder="hr" style="text-align: center;"> 
      :
      <input type="number" name="breakMinute" class="time-input" min="0" max="59" placeholder="min" style="text-align: center;">
    </div>
  </div>
</div>

<div class="button-row">
  <div style="display:flex; gap:10px; width:300px;">
    <button type="button" class="save-btn" onclick="saveSettings()" style="flex:1; min-width:0;">Save</button>
    <button type="button" class="reset-btn" onclick="resetAll()" style="flex:1; min-width:0;">Reset</button>
  </div>
  <button type="button" class="back-btn" onclick="goBackToFeeling()">Back</button>
</div>
</form>
</div>

<script>
// ================= PAGE NAVIGATION =================
function goToFeeling() {
  const name = document.getElementById('userName').value.trim();
  if(name === "") { alert("Please enter your name."); return; }

  let hiddenName = document.getElementById('hiddenUserName');
  if(!hiddenName) {
    hiddenName = document.createElement("input");
    hiddenName.type = "hidden";
    hiddenName.name = "userName";
    hiddenName.id = "hiddenUserName";
    document.getElementById("pomodoroForm").appendChild(hiddenName);
  }
  hiddenName.value = name;

  document.getElementById('nameCard').style.display = 'none';
  document.getElementById('feelingCard').style.display = 'block';
  document.getElementById('greeting').innerText = `Hello ${name}!`;
}

function goBack() {
  document.getElementById('feelingCard').style.display = 'none';
  document.getElementById('nameCard').style.display = 'block';
}

function startPomodoro(feeling) {
  document.getElementById('feelingCard').style.display = 'none';
  if (feeling === "Tired" || feeling === "Stressed") {
    document.getElementById('part3bad').style.display = 'block';
  } else {
    document.getElementById('pomodoroCard').style.display = 'block';
  }
}

function goToStudy() {
  document.getElementById('part3bad').style.display = 'none';
  document.getElementById('pomodoroCard').style.display = 'block';
}

function goBackFromSuggestion() {
  document.getElementById('part3bad').style.display = 'none';
  document.getElementById('feelingCard').style.display = 'block';
}

function goBackToFeeling() {
  document.getElementById('pomodoroCard').style.display = 'none';
  document.getElementById('feelingCard').style.display = 'block';
}

// ================= SETTINGS =================
function saveSettings(event) {

  if(event) event.preventDefault();

  const form = document.getElementById("pomodoroForm");
  const params = new URLSearchParams(new FormData(form)).toString();

  fetch("/save?" + params)
    .then(response => response.text())
    .then(data => {

      if (data === "ERROR") {
        alert("Error: Study Duration + Break Time exceeds Total Study Time!");
        return;
      }

      alert("Settings Saved!");

    })
    .catch(err => {
      alert("Error saving settings.");
    });
}

function resetAll() {
  const inputs = document.querySelectorAll("#pomodoroCard input");
  inputs.forEach(input => input.value = "");
  fetch('/reset')
    .then(() => alert("Reset Done!"))
    .catch(err => alert("Error during reset."));
}

// ================= DARK/LIGHT MODE =================
function toggleMode() {
  document.body.classList.toggle("dark-mode");
  const text = document.getElementById("modeText");
  const circle = document.getElementById("circle");
  if(document.body.classList.contains("dark-mode")) {
    text.innerHTML = "NIGHT MODE";
    circle.innerHTML = "";
  } else {
    text.innerHTML = "DAY MODE";
    circle.innerHTML = "";
  }
}
</script>

</body>
</html>
)rawliteral";


#endif