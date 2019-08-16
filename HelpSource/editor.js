function init () {
    // Based on editors/sc-ide/core/sc_lexer.cpp
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

    const textareas = Array.from(document.querySelectorAll('textarea'))
    for (let textarea of textareas) {
        textarea.editor = CodeMirror.fromTextArea(textarea, {
            mode: 'scd',
            value: textarea.value,
            lineWrapping: true,
            viewportMargin: Infinity,
            extraKeys: {
                'Shift-Enter': evalLine
            }
        })

        textarea.editor.on('dblclick', (editor) => {
            const cursor = editor.getCursor()
            const parenMatch = editor.getLine(cursor.line)
                .slice(cursor.ch-1,cursor.ch).match(/[()]/)
            if (parenMatch) {
                editor.undoSelection()
                selectRegion({ flash: false })
            }
        })

        textarea.editor.on('blur', (editor) => {
            editor.setSelection(editor.getCursor(), null, { scroll: false })
        })
    }
}

function evalLine () {
    if (window.IDE) {
      // Ask IDE to eval line. Calls back to `selectLine()`
        window.IDE.evaluateLine()
    }
}

// Returns the code selection, line or region
function selectRegion (options = { flash: true }) {
    const range = window.getSelection().getRangeAt(0)
    const textarea = range.startContainer.parentNode.previousSibling
    if (!textarea) return
    const editor = textarea.editor
    const cursor = editor.getCursor()

    if (editor.somethingSelected())
        return selectLine(options)

    function findLeftParen (cursor) {
        const left = editor.findPosH(cursor, -1, 'char')
        const char = editor.getLine(left.line).slice(left.ch, left.ch+1)
        const token = editor.getTokenTypeAt(cursor) || ''
        if (left.hitSide)
            return left
        if (token.startsWith('comment'))
            return findLeftParen(left)
        if (char === ')')
            return findLeftParen(findLeftParen(left))
        if (char === '(')
            return left
        return findLeftParen(left)
    }

    function findRightParen (cursor) {
        const right = editor.findPosH(cursor, 1, 'char')
        const char = editor.getLine(right.line).slice(right.ch-1, right.ch)
        const token = editor.getTokenTypeAt(cursor) || ''
        if (right.hitSide)
            return right
        if (token.startsWith('comment'))
            return findRightParen(right)
        if (char === '(')
            return findRightParen(findRightParen(right))
        if (char === ')')
            return right
        return findRightParen(right)
    }

    // Adjust cursor before finding parens
    if (editor.getLine(cursor.line).slice(cursor.ch,cursor.ch+1) === '(')
        editor.setCursor(Object.assign(cursor, { ch: cursor.ch+1 }))
    if (editor.getLine(cursor.line).slice(cursor.ch-1,cursor.ch) === ')')
        editor.setCursor(Object.assign(cursor, { ch: cursor.ch-1 }))

    const parenPairs = []
    let left = findLeftParen(cursor)
    let right = findRightParen(cursor)

    while (!left.hitSide || !right.hitSide) {
        parenPairs.push([left, right])
        left = findLeftParen(left)
        right = findRightParen(right)
    }

    // No parens found
    if (parenPairs.length === 0)
        return selectLine(options)

    const pair = parenPairs.pop()
    left = pair[0]
    right = pair[1]

    // Parens are inline
    if (left.ch > 0)
        return selectLine(options)

    // Parens are a region
    if (options.flash) {
        const marker = editor.markText(left, right, { className: 'text-flash' })
        setTimeout(() => marker.clear(), 300)
        return editor.getRange(left, right)
    } else {
        editor.addSelection(left, right)
        return editor.getSelection()
    }
}

// Returns the code selection or line
function selectLine (options = { flash: true }) {
    const range = window.getSelection().getRangeAt(0)
    const textarea = range.startContainer.parentNode.previousSibling
    if (!textarea) return
    const editor = textarea.editor
    const cursor = editor.getCursor()
    let from, to

    if (editor.somethingSelected()) {
        from = editor.getCursor('start')
        to = editor.getCursor('end')
    } else {
        from = { line: cursor.line, ch: 0 }
        to = { line: cursor.line, ch: editor.getLine(cursor.line).length }
    }

    if (options.flash) {
        let marker = editor.markText(from, to, { className: 'text-flash' })
        setTimeout(() => marker.clear(), 300)
    }
    return editor.getRange(from, to)
}

init()
