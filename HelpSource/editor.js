const init = () => {
    /* based on editors/sc-ide/core/sc_lexer.cpp */
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
    })

    let textareas = Array.from(document.querySelectorAll('textarea'))
    textareas.forEach(textarea => {
        let code = textarea.value
        textarea.editor = CodeMirror.fromTextArea(textarea, {
            mode: 'scd',
            value: code,
            lineWrapping: true,
            viewportMargin: Infinity,
            extraKeys: {
                'Shift-Enter': evalLine
            }
        })

        textarea.editor.on('dblclick', editor => {
            let cursor = editor.getCursor()
            let parenMatch = editor.getLine(cursor.line)
                .slice(cursor.ch-1,cursor.ch).match(/[()]/)
            if (parenMatch) {
                editor.undoSelection()
                selectRegion({ flash: false })
            }
        })

        textarea.editor.on('blur', editor => {
            editor.setSelection(editor.getCursor(), null, { scroll: false })
        })
    })

}

const evalLine = () => {
    // If we are not running in the SC IDE, do nothing.
    if (!window.IDE) {
        return;
    }
    // Ask IDE to eval line. Calls back to `selectLine()`
    window.IDE.evaluateLine();
}

/* returns the code selection, line or region */
const selectRegion = (options = { flash: true }) => {
    let range = window.getSelection().getRangeAt(0)
    let textarea = range.startContainer.parentNode.previousSibling
    if (!textarea) return
    let editor = textarea.editor

    if (editor.somethingSelected())
        return selectLine(options)

    const findLeftParen = cursor => {
        let cursorLeft = editor.findPosH(cursor, -1, 'char')
        if (cursorLeft.hitSide)
            return cursorLeft
        let ch = editor.getLine(cursorLeft.line)
            .slice(cursorLeft.ch, cursorLeft.ch+1)
        if (ch === ')')
            return findLeftParen(findLeftParen(cursorLeft))
        if (ch === '(')
            return cursorLeft
        return findLeftParen(cursorLeft)
    }

    const findRightParen = cursor => {
        let cursorRight = editor.findPosH(cursor, 1, 'char')
        if (cursorRight.hitSide)
            return cursorRight
        let ch = editor.getLine(cursorRight.line)
            .slice(cursorRight.ch-1, cursorRight.ch)
        if (ch === '(')
            return findRightParen(findRightParen(cursorRight))
        if (ch === ')')
            return cursorRight
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
        return selectLine(options)

    let pair = parenPairs.pop()
    leftCursor = pair[0]
    rightCursor = pair[1]

    /* parens are inline */
    if (leftCursor.ch > 0)
        return selectLine(options)

    /* parens are a region */
    if (options.flash === false) {
        editor.addSelection(leftCursor, rightCursor)
        return editor.getSelection()
    } else {
        let marker = editor.markText(leftCursor, rightCursor, { className: 'text-flash' })
        setTimeout(() => marker.clear(), 300)
        return editor.getRange(leftCursor, rightCursor)
    }
}

// Returns the code selection or line
const selectLine = (options = { flash: true }) => {
    let range = window.getSelection().getRangeAt(0)
    let textarea = range.startContainer.parentNode.previousSibling
    if (!textarea) return
    let editor = textarea.editor
    let cursor = editor.getCursor()

    if (editor.somethingSelected()) {
        from = editor.getCursor('start')
        to = editor.getCursor('end')
    } else {
        from = { line: cursor.line, ch: 0 }
        to = { line: cursor.line, ch: editor.getLine(cursor.line).length }
    }

    if (!options.flash)
        return editor.getRange(from, to)
    let marker = editor.markText(from, to, { className: 'text-flash' })
    setTimeout(() => marker.clear(), 300)
    return editor.getRange(from, to)
}

init()
