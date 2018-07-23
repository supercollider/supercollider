const init = () => {
    CodeMirror.defineSimpleMode('scd', {
        start: [
            {regex: /"(?:[^\\]|\\.)*?(?:"|$)/, token: 'string'},
            {regex: /^[\t\n\r \xA0]+/, token: 'whitespace'},
            {regex: /^\\\w+/, token: 'attribute-name'},
            {regex: /^'[^']+'/, token: 'attribute-name'},
            {regex: /^\w+\:/, token: 'attribute-name'},
            {regex: /^\$(\\)?./, token: 'attribute-name'},
            {regex: /^~\w+/, token: 'env-var'},
            {regex: /^(?:super|thisFunctionDef|thisFunction|thisMethod|thisProcess|thisThread|this)\b/, token: 'keyword'},
            {regex: /^(?:true|false|nil|inf)\b/, token: 'atom'},
            {regex: /^(?:var|classvar|const|arg)\b/, token: 'declaration'},
            {regex: /^\b([A-Z][A-Za-z_0-9]+)\b/, token: 'type'},
            {regex: /^\/(?:\/.*|\*(?:\/|\**[^*/])*(?:\*+\/?)?)/, token: 'comment'},
            {regex: /^-?\d+r[\da-zA-Z]+(\.[\da-zA-Z]+)?/, token: 'literal'},
            {regex: /^-?(?:(?:\d+(\.\d+)?)(?:e[+\-]?\d+)?(pi)?)|(?:pi\b)/, token: 'literal'},
            {regex: /^[a-z_]\w*/i, token: 'plain'},
            {regex: /^[-.,;#()\[\]{}]/, token: 'punctuation'}
        ]
    });

    let textareas = Array.from(document.querySelectorAll('textarea'))
    textareas.forEach(textarea => {
        let code = textarea.value
        textarea.editor = CodeMirror.fromTextArea(textarea, {
            mode: 'scd',
            value: code,
            lineWrapping: true,
            viewportMargin: Infinity,
            extraKeys: { 
                'Cmd-Enter': selectRegion,
                'Shift-Enter': selectLine
            }
        })

        textarea.editor.on('dblclick', editor => {
            let cursor = editor.getCursor()
            let parenMatch = editor.getLine(cursor.line)
                .slice(cursor.ch-1,cursor.ch).match(/[()]/)
            if (parenMatch) {
                editor.replaceSelection(parenMatch[0])
                selectRegion(false)
            }
        })

        textarea.editor.on('blur', editor => {
            editor.setSelection(editor.getCursor(), null, { scroll: false })
        })
    })

}


/* returns the code selection, line or block */
const selectRegion = (reset = true ) => {
    let range = window.getSelection().getRangeAt(0)
    let textarea = range.startContainer.parentNode.previousSibling
    if (!textarea) return
    let editor = textarea.editor

    if (editor.somethingSelected())
        return selectLine(reset)

    const findLeftParen = cursor => {
        let cursorLeft = editor.findPosH(cursor, -1, 'char')
        if (cursorLeft.hitSide)
            return null
        let ch = editor.getLine(cursorLeft.line)
            .slice(cursorLeft.ch, cursorLeft.ch+1)
        if (ch === ')') {
            let cursorInner = findLeftParen(cursorLeft)
            return cursorInner ? findLeftParen(cursorInner) : cursorInner
        }
        if (ch === '(')
            return cursorLeft
        return findLeftParen(cursorLeft)
    }

    const findRightParen = cursor => {
        let cursorRight = editor.findPosH(cursor, 1, 'char')
        if (cursorRight.hitSide)
            return null
        let ch = editor.getLine(cursorRight.line)
            .slice(cursorRight.ch-1, cursorRight.ch)
        if (ch === '(') {
            let cursorInner = findRightParen(cursorRight)
            return cursorInner ? findRightParen(cursorInner) : cursorInner
        }
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

    do {
        if (!leftCursor || !rightCursor)
            return selectLine(reset)
        parenPairs.push([leftCursor, rightCursor])
        leftCursor = findLeftParen(leftCursor)
        rightCursor = findRightParen(rightCursor)
    } while (leftCursor && rightCursor)

    let parenPair = parenPairs.pop()
    cursor = editor.getCursor()
    editor.addSelection(parenPair[0], parenPair[1])
    
    if (reset)
        setTimeout(() => {
            editor.setCursor(cursor, null, { scroll: false })
        }, 400)

    return editor.getSelection()
}

/* returns the code selection or line */
const selectLine = (reset = true) => {
    let range = window.getSelection().getRangeAt(0)
    let textarea = range.startContainer.parentNode.previousSibling
    if (!textarea) return
    let editor = textarea.editor
    let cursor = editor.getCursor()

    if (reset)
        setTimeout(() => {
            editor.setCursor(cursor, null, { scroll: false })
        }, 400)

    if (editor.somethingSelected())
        return editor.getSelection()
    else {
        editor.addSelection({ line: cursor.line, ch: 0 },
            { line: cursor.line, ch: editor.getLine(cursor.line).length })
        return editor.getSelection()
    }
}

init()
