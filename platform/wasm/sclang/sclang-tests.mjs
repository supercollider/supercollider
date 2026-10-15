import { fileURLToPath } from "node:url";
import assert from "node:assert/strict";
import ScLang from "./sclang.js";

// how we resolve .data is a bit iffy - we have to give nodejs a hint here ;)
const distDir = fileURLToPath(new URL("../sclang/", import.meta.url));
let sclang = await ScLang({
    locateFile: (path) => distDir + path,
});

let outputLines = [];

sclang.printCallback = (text) => {
    console.log(text);
    outputLines.push(text);
};

sclang.bootInterpreter();
console.log("Waiting 5s for sclang to finish booting")

setTimeout(async function testEval() {
    sclang.runCode("2+2");
    // have to wait a sec b/c interpreter does not run blocking
    await new Promise((resolve) => setTimeout(resolve, 1000));

    assert.equal(outputLines.at(-1), "-> 4");
    console.log("sclang evaluation works :)")
    process.exit(0);
}, 5000);
