const fs = require("fs");
const readline = require("readline");

// set up readline
readline.emitKeypressEvents(process.stdin);
process.stdin.setRawMode(true);

// pwm set up
const pwmPeriodNs = "20000000";

const PWMPATH = "/sys/class/pwm/pwm-0:0";
const low = 0.8;
const hi = 2.4;
var ms = 250;
var pos = 1.6;
var step = 0.1;

instruct();

fs.writeFileSync(PWMPATH + "/period", pwmPeriodNs);
fs.writeFileSync(PWMPATH + "/enable", "1");

/** @type {NodeJS.Timeout | null} */
var timer = setInterval(sweep, ms);

function instruct() {
  console.log("MS: q=inc, e=dec, r=reset");
  console.log("STEP: w=inc, s=dec");
  console.log("ROTATE: a=ccw, d=cw");
  console.log("1=0deg, 2=90deg, 3=180deg");
  console.log("RETURN=toggle manual");
  console.log("?=instructions");
  console.log("^C to stop\n");
}

/** Calculates current angle */
function calcAngleDeg() {
  return Math.round(((pos - low) / (hi - low)) * 180);
}

/** Writes position to duty cycle */
function writePos() {
  if (pos >= hi) {
    pos = hi;
  }
  if (pos <= low) {
    pos = low;
  }

  const dutyCycle = Math.round(pos * 1000000);
  fs.writeFileSync(PWMPATH + "/duty_cycle", dutyCycle);
  console.log(`Angle: ${calcAngleDeg()}deg`);
}

function sweep() {
  pos += step;
  if (pos > hi || pos < low) {
    step *= -1;
  }

  writePos();
}

/**
 * @param {'inc' | 'dec'} dir
 */
function adjustStep(dir) {
  if (dir === "inc" && step >= 1.6) {
    console.log(`\n--- MAX STEP REACHED ---`);
    step = 1.6;
    return;
  }

  if (dir === "dec" && step <= 0.1) {
    console.log(`\n--- MIN STEP REACHED ---`);
    step = 0.1;
    return;
  }

  const roundStep = step.toFixed(1);
  if (dir === "inc") {
    const roundStepInc = (step + 0.1).toFixed(1);
    console.log(`\n--- STEP INCREASED: ${roundStep} -> ${roundStepInc} ---`);
    step += 0.1;
  }

  if (dir === "dec") {
    const roundStepDec = (step - 0.1).toFixed(1);
    console.log(`\n--- STEP DECREASED: ${roundStep} -> ${roundStepDec} ---`);
    step -= 0.1;
  }
}

/**
 * @param {'inc' | 'dec' | "res"} dir
 */
function adjustMs(dir) {
  if (dir === "inc") {
    console.log(`\n--- MS INCREASED: ${ms} -> ${ms + 10} ---`);
    ms += 10;
  }

  if (dir === "dec") {
    console.log(`\n--- MS DECREASED: ${ms} -> ${ms - 10} ---`);
    ms -= 10;
  }

  if (dir === "res") {
    console.log(`\n--- MS RESET: ${ms} -> 250 ---`);
    ms -= 10;
  }
}

function cleanup() {
  console.log("Got SIGINT, turning motor off");
  clearInterval(timer); // Stop the timer
}

function toggle() {
  if (timer == null) {
    console.log("\n--- AUTOMATIC ---");
    timer = setInterval(sweep, ms);
  } else {
    console.log("\n--- MANUAL ---");
    clearInterval(timer);
    timer = null;
    step = Math.abs(step);
  }
}

function rotateCcw() {
  if (pos > low) {
    pos -= Math.abs(step);
    writePos();
  } else {
    console.log("\n!!! MIN ROTATION REACHED !!!");
  }
}

function rotateCw() {
  if (pos < hi) {
    pos += Math.abs(step);
    writePos();
  } else {
    console.log("\n!!! MAX ROTATION REACHED !!!");
  }
}

process.stdin.on("keypress", function (str, key) {
  if (!key) return;

  if (key.ctrl && key.name === "c") {
    cleanup();
    process.exit();
  }

  if (str === "?") {
    instruct();
  }

  if (key.name === "return") {
    toggle();
  }

  if (timer !== null) {
    return;
  }

  switch (key.name) {
    case "q":
      adjustMs("inc");
      break;

    case "e":
      adjustMs("dec");
      break;

    case "r":
      adjustMs("res");
      break;

    case "w":
      adjustStep("inc");
      break;

    case "s":
      adjustStep("dec");
      break;

    case "a":
      rotateCcw();
      break;

    case "d":
      rotateCw();
      break;

    case "1":
      pos = low;
      writePos();
      break;

    case "2":
      pos = hi - low;
      writePos();
      break;

    case "3":
      pos = hi;
      writePos();
      break;
  }
});
