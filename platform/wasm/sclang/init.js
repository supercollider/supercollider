import ScLang from './sclang.js';

let sclang = await ScLang();
window.sclang = sclang;

sclang.bootInterpreter();

const htmlConsole = document.getElementById("console");
const scsynthConsole = document.getElementById("scsynth");
const userInput = document.getElementById("user-input");

sclang.printCallback = (text) => {
    htmlConsole.value += `${text}\n`;
    htmlConsole.scrollTop = htmlConsole.scrollHeight;
};

userInput.addEventListener("keydown", (event) => {
    if (event.key === "Enter") {
        sclang.runCode(userInput.value);
        if(event.shiftKey) {
            userInput.value = "";
        };
    }
});

sclang.onOsc = (osc) => {
    console.log("OSC message from sclang", osc);
};


sclang.bootServer = () => {
    alert("Got signal to boot server - but scsynth is not available in this demo.");
}
//
// window.bootServer = () => {
//     console.log("now the server should boot via window");
//     setTimeout(() => {scsynth.boot()}, 100);
// }
