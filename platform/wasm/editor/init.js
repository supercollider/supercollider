import ScLang from './sclang.js';
import ScSynth from './scsynth.js';

// scsynth stuff
let scsynth = await ScSynth();
window.scsynth = scsynth;

// sclang stuff
let sclang = await ScLang();
window.sclang = sclang;

sclang.bootInterpreter();

const htmlConsole = document.getElementById("console");

// if line count is 1000, clear the first 500 lines, so there are never more than 1000 lines kept to display
let lineCount = 0;
function printToConsole(text) {
    htmlConsole.value += `${text}\n`;
    lineCount += 1;

    if(lineCount >= 1000) {
        console.log(`Clear print console`);
        let cutPos = 0;
        for (let i=0; i < 500; i++) {
            cutPos = htmlConsole.value.indexOf('\n', cutPos) + 1;
        }
        htmlConsole.value = htmlConsole.value.slice(cutPos);
        lineCount = 500;
    }

    htmlConsole.scrollTop = htmlConsole.scrollHeight;
}

sclang.printCallback = printToConsole;

scsynth.onStdout = printToConsole;

sclang.onOsc = (osc) => {
    // osc message will be freed - so create a copy for scsynth!
    let copy = new Uint8Array(osc);
    scsynth.sendOsc(copy);
};

scsynth.onOscReply = (oscReply) => {
    sclang.sendOsc(oscReply);
};

async function connectMic() {
    console.log("Connecting Mic...");
    const stream = await navigator.mediaDevices.getUserMedia({
        audio: true,
        video: false,
    });

    const audioContext = scsynth.getAudioContext();
    const worklet = scsynth.getWorkletNode();

    const micSource = audioContext.createMediaStreamSource(stream);
    micSource.connect(worklet);
}

window.bootServer = (options) => {
    // console.log("now the server should boot via window", options);
    if(options.numOutputBusChannels)
    setTimeout(() => {
        scsynth.boot(options)
        if(options.numInputBusChannels > 0) {
            connectMic();
        }
    }, 100);
}

// cmd period
document.addEventListener("keydown", (event) => {
    const keyName = event.key;

    if (keyName === ".") {
        if (event.metaKey || event.altKey || event.ctrlKey) {
            if (typeof sclang !== 'undefined') {
                sclang.runCode(`CmdPeriod.run;`);
            }
        }
    }
});

// editor stuff

CodeMirror.defineSimpleMode('scd', {
    start: [
        { regex: /^\s+/, token: 'whitespace' },
        { regex: /^(?:arg|classvar|const|super|this|var)\b/, token: 'keyword' },
        { regex: /^(?:false|inf|nil|true|thisFunction|thisFunctionDef|thisMethod|thisProcess|thisThread|currentEnvironment|topEnvironment)\b/, token: 'built-in' },
        { regex: /^\b\d+r[0-9a-zA-Z]*(\.[0-9A-Z]*)?/, token: 'number radix-float' },
        { regex: /^\b\d+(s+|b+|[sb]\d+)\b/, token: 'number scale-degree' },
        { regex: /^\b((\d+(\.\d+)?([eE][-+]?\d+)?(pi)?)|pi)\b/, token: 'number float' },
        { regex: /^\b0x(\d|[a-f]|[A-F])+/, token: 'number hex-int' },
        { regex: /^\b[A-Za-z_]\w*\:/, token: 'symbol symbol-arg' },
        { regex: /^[a-z]\w*/, token: 'text name' },
        { regex: /^\b[A-Z]\w*/, token: 'class' },
        { regex: /^\b_\w+/, token: 'primitive' },
        { regex: /^\\\w*/, token: 'symbol' },
        { regex: /'(?:[^\\]|\\.)*?(?:'|$)/, token: 'symbol' },
        { regex: /^\$\\?./, token: 'char' },
        { regex: /^~\w+/, token: 'env-var' },
        { regex: /^\/\/[^\r\n]*/, token: 'comment single-line-comment' },
        { regex: /"(?:[^\\]|\\.)*?(?:"|$)/, token: 'string' },
        { regex: /^[-.,;#()\[\]{}]/, token: 'text punctuation' },
        { regex: /\/\*/, push: 'comment', token: 'comment multi-line-comment' },
        { regex: /^[+\-*/&\|\^%<>=!?]+/, token: 'text operator' },
    ],
    comment: [
        { regex: /\*\//, pop: true, token: 'comment multi-line-comment' },
        { regex: /./, token: 'comment multi-line-comment' }
    ]
});

var editor = CodeMirror.fromTextArea(document.getElementById('code'), {
    lineNumbers: true,
    matchBrackets: true,
    mode: 'scd',
    indentUnit: 4,
    extraKeys: {
        'Ctrl-Enter': selectLine,
        'Shift-Enter': selectRegion,
        'Cmd-Enter':   selectRegion,
    },
});

window.editor = editor;

editor.setSize('100%', '100%');

function evaluate(code) {
    if (typeof sclang !== 'undefined') {
        sclang.runCode(code);
    } else {
        alert('sclang not ready - plz wait or check console');
    }
}

/* returns the code selection, line or region */
function selectRegion() {
    if (window.editor.getSelection())
        return selectLine()

    const findLeftParen = cursor => {
        let cursorLeft = editor.findPosH(cursor, -1, 'char')
        let token = editor.getTokenTypeAt(cursor) || ''
        if (cursorLeft.hitSide)
            return cursorLeft
        let ch = editor.getLine(cursorLeft.line)
            .slice(cursorLeft.ch, cursorLeft.ch+1)
        if (token.match(/^(comment|string|symbol|char)/))
            return findLeftParen(cursorLeft)
        if (ch === ')')
            return findLeftParen(findLeftParen(cursorLeft))
        if (ch === '(')
            return cursorLeft
        return findLeftParen(cursorLeft)
    }

    const findRightParen = cursor => {
        let cursorRight = editor.findPosH(cursor, 1, 'char')
        let token = editor.getTokenTypeAt(cursor) || ''
        if (cursorRight.hitSide)
            return cursorRight
        let ch = editor.getLine(cursorRight.line)
            .slice(cursorRight.ch-1, cursorRight.ch)
        if (ch === '(')
            return findRightParen(findRightParen(cursorRight))
        if (ch === ')')
            return cursorRight
        if (token.match(/^(comment|string|symbol|char)/))
            return findRightParen(cursorRight)
        return findRightParen(cursorRight)
    }

    let cursor = editor.getCursor()
    if (editor.getLine(cursor.line).slice(cursor.ch,cursor.ch+1) === '(')
        editor.setCursor(Object.assign(cursor, { ch: cursor.ch+1 }))
    if (editor.getLine(cursor.line).slice(cursor.ch-1,cursor.ch) === ')')
        editor.setCursor(Object.assign(cursor, { ch: cursor.ch-1 }))

    let parenPairs = []
    let leftCursor = findLeftParen(cursor)
    let rightCursor = findRightParen(cursor)

    while (!leftCursor.hitSide || !rightCursor.hitSide) {
        parenPairs.push([leftCursor, rightCursor])
        leftCursor = findLeftParen(leftCursor)
        rightCursor = findRightParen(rightCursor)
    }

    /* no parens found */
    if (parenPairs.length === 0)
        return selectLine()

    let pair = parenPairs.pop()
    leftCursor = pair[0]
    rightCursor = pair[1]

    /* parens are inline */
    if (leftCursor.ch > 0)
        return selectLine()

    let marker = editor.markText(leftCursor, rightCursor, { className: 'text-flash' })
    setTimeout(() => marker.clear(), 300)
    evaluate(editor.getRange(leftCursor, rightCursor));
}

function selectLine() {
    let cursor = editor.getCursor()

    let from;
    let to;
    if (editor.somethingSelected()) {
        from = editor.getCursor('start')
        to = editor.getCursor('end')
    } else {
        from = { line: cursor.line, ch: 0 }
        to = { line: cursor.line, ch: editor.getLine(cursor.line).length }
    }

    let marker = editor.markText(from, to, { className: 'text-flash' })
    setTimeout(() => marker.clear(), 300)
    evaluate(editor.getRange(from, to));
}

// double tap w/ 2 fingers to execute line / selection
let lastDoubleTapReleaseTimeMs = 0;

document.addEventListener("touchstart", (e) => {
    let now = Date.now();
    if (e.touches.length === 3) {
        e.preventDefault();
        let deltaMs = now - lastDoubleTapReleaseTimeMs;
        if (deltaMs < 350) {
            selectRegion();
            console.log("double press");
            // fall back a bit to avoid "double clicks"
            lastDoubleTapReleaseTimeMs = now - 500;
        } else {
            lastDoubleTapReleaseTimeMs = now;
        }
    }
});
