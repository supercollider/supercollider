/*
    SuperCollider Qt IDE
    Copyright (c) 2012 Jakob Leben & Tim Blechmann
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <cassert>
#include <algorithm>

#include "sc_syntax_highlighter.hpp"
#include "main.hpp"
#include "settings.hpp"

#include <QApplication>

namespace ScIDE {

SyntaxHighlighterGlobals * SyntaxHighlighterGlobals::mInstance = 0;

SyntaxHighlighterGlobals::SyntaxHighlighterGlobals( Main *main ):
    QObject(main)
{
    Q_ASSERT(mInstance == 0);
    mInstance = this;

    initHighlightFormats();
    initSyntaxRules();

    // initialize formats from defaults + settings:
    applySettings(main->settings());

    connect(main, SIGNAL(applySettingsRequest(QSettings*)),
            this, SLOT(applySettings(QSettings*)));
}

void SyntaxHighlighterGlobals::initHighlightFormats()
{
    mDefaultFormats[KeywordFormat].setFontWeight(QFont::Bold);
    mDefaultFormats[KeywordFormat].setForeground(QColor(0,0,230));

    mDefaultFormats[BuiltinFormat].setForeground(QColor(51,51,191));

    mDefaultFormats[PrimitiveFormat] = mDefaultFormats[BuiltinFormat];

    mDefaultFormats[ClassFormat].setForeground(QColor(0,0,210));

    mDefaultFormats[NumberFormat].setForeground(QColor(152,0,153));

    mDefaultFormats[CommentFormat].setForeground(QColor(191,0,0));

    mDefaultFormats[StringFormat].setForeground(QColor(95,95,95));

    mDefaultFormats[SymbolFormat].setForeground(QColor(0,115,0));

    mDefaultFormats[CharFormat] = mDefaultFormats[SymbolFormat];

    mDefaultFormats[EnvVarFormat].setForeground(QColor(255,102,0));

    // if system base color is dark, brighten the highlighting colors
    QPalette plt( QApplication::palette() );
    QColor base = plt.color(QPalette::Base);
    if (base.red() + base.green() + base.blue() < 380)
    {
        for (int i = 0; i < FormatCount; ++i)
        {
            mDefaultFormats[i].setForeground( mDefaultFormats[i].foreground().color().lighter(160) );
        }
    }
}

void SyntaxHighlighterGlobals::initSyntaxRules()
{
    initKeywords();
    initBuiltins();

    /* NOTE:

    The highlighting algorithm demands that all regexps
    start with a caret "^", to only match at beginning of string.

    Order is important:
    -- floatRegexp is subset of radixFloatRegex -> must come later
    -- classRegexp and primitiveRegexp are subsets of symbolArgRegexp -> must come later

    */

    mInCodeRules << SyntaxRule( SyntaxRule::SymbolArgRule, "^\\b[A-Za-z_]\\w*\\:" );

    mInCodeRules << SyntaxRule( SyntaxRule::ClassRule, "^\\b[A-Z]\\w*" );

    mInCodeRules << SyntaxRule( SyntaxRule::PrimitiveRule, "^\\b_\\w+" );

    mInCodeRules << SyntaxRule( SyntaxRule::SymbolRule, "^(\\\\\\w*|\\'([^\\'\\\\]*(\\\\.[^\\'\\\\]*)*)\\')" );

    mInCodeRules << SyntaxRule( SyntaxRule::CharRule, "^\\$\\\\?." );

    mInCodeRules << SyntaxRule( SyntaxRule::StringRule, "^\"([^\"\\\\]*(\\\\.[^\"\\\\]*)*)\"" );

    mInCodeRules << SyntaxRule( SyntaxRule::EnvVarRule, "^~\\w+" );

    mInCodeRules << SyntaxRule( SyntaxRule::RadixFloatRule, "^\\b(\\d)+r(\\d|[a-zA-Z])+(.(\\d|[a-zA-Z]))?" );

    // do not include leading "-" in float highlighting, as there's no clear
    // rule whether it is not rather a binary operator
    mInCodeRules << SyntaxRule( SyntaxRule::FloatRule, "^\\b((\\d*\\.?\\d+([eE][-+]?\\d+)?(pi)?)|pi)" );

    mInCodeRules << SyntaxRule( SyntaxRule::HexIntRule, "^\\b0(x|X)(\\d|[a-f]|[A-F])+" );

    mInCodeRules << SyntaxRule( SyntaxRule::SingleLineCommentRule, "^//[^\r\n]*" );

    mInCodeRules << SyntaxRule( SyntaxRule::MultiLineCommentStartRule, "^/\\*" );

    mInSymbolRegexp.setPattern("([^\'\\\\]*(\\\\.[^\'\\\\]*)*)");
    mInStringRegexp.setPattern("([^\"\\\\]*(\\\\.[^\"\\\\]*)*)");
}

void SyntaxHighlighterGlobals::initKeywords()
{
    QStringList keywords;
    keywords << "arg"
             << "classvar"
             << "const"
             << "super"
             << "this"
             << "thisFunction"
             << "thisFunctionDef"
             << "thisMethod"
             << "thisProcess"
             << "thisThread"
             << "thisThread"
             << "var";

    QString keywordPattern = QString("^\\b(%1)\\b").arg(keywords.join("|"));
    mInCodeRules << SyntaxRule(SyntaxRule::KeywordRule, keywordPattern);
}

void SyntaxHighlighterGlobals::initBuiltins()
{
    QStringList builtins;
    builtins << "false"
             << "inf"
             << "nil"
             << "true";

    QString builtinsPattern = QString("^\\b(%1)\\b").arg(builtins.join("|"));
    mInCodeRules << SyntaxRule(SyntaxRule::BuiltinRule, builtinsPattern);
}

void SyntaxHighlighterGlobals::applySettings( QSettings *s )
{
    QString key("IDE/editor/highlighting");
    applySettings( s, key + "/normal", PlainFormat );
    applySettings( s, key + "/keyword", KeywordFormat );
    applySettings( s, key + "/builtin", BuiltinFormat );
    applySettings( s, key + "/primitive", PrimitiveFormat );
    applySettings( s, key + "/class", ClassFormat );
    applySettings( s, key + "/number", NumberFormat );
    applySettings( s, key + "/symbol", SymbolFormat );
    applySettings( s, key + "/envvar", EnvVarFormat );
    applySettings( s, key + "/string", StringFormat );
    applySettings( s, key + "/char", CharFormat );
    applySettings( s, key + "/comment", CommentFormat );

    Q_EMIT(syntaxFormatsChanged());
}

void SyntaxHighlighterGlobals::applySettings( QSettings *s, const QString &key, SyntaxFormat type )
{
    if (s->contains(key))
        mFormats[type] = s->value(key).value<QTextCharFormat>();
    else
        mFormats[type] = mDefaultFormats[type];
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent):
    QSyntaxHighlighter( parent )
{
    mGlobals = SyntaxHighlighterGlobals::instance();

    connect(mGlobals, SIGNAL(syntaxFormatsChanged()), this, SLOT(rehighlight()));
}

SyntaxRule::Type SyntaxHighlighter::findMatchingRule
(const QString& text, int& currentIndex, int& lengthOfMatch)
{
    int matchLength = -1;
    SyntaxRule::Type matchRule = SyntaxRule::NoRule;
    foreach(SyntaxRule rule, mGlobals->mInCodeRules)
    {
        int matchIndex = rule.expr.indexIn(text, currentIndex, QRegExp::CaretAtOffset);
        // a guard to ensure all regexps match only at beginning of string:
        assert(matchIndex <= currentIndex);
        if (matchIndex != -1) {
            matchRule = rule.type;
            matchLength = rule.expr.matchedLength();
            break;
        }
    }

    lengthOfMatch = matchRule == SyntaxRule::NoRule ? 0 : matchLength;
    return matchRule;
}

void SyntaxHighlighter::highlightBlockInCode(const QString& text, int & currentIndex, int & currentState)
{
    BlockData *blockData = static_cast<BlockData*>(currentBlockUserData());
    Q_ASSERT(blockData);

    const QTextCharFormat * formats = mGlobals->formats();

    do {
        static QString brackets("(){}[]");

        if (brackets.contains(text[currentIndex])) {
            blockData->brackets.append( new BracketInfo(text[currentIndex].toAscii(), currentIndex) );
            ++currentIndex;
            continue;
        }

        if (text[currentIndex] == '\"') {
            currentState = inString;
            setFormat(currentIndex, 1, formats[StringFormat]);
            currentIndex += 1;
            return;
        }

        if (text[currentIndex] == '\'') {
            currentState = inSymbol;
            setFormat(currentIndex, 1, formats[SymbolFormat]);
            currentIndex += 1;
            return;
        }

        int lenghtOfMatch;
        SyntaxRule::Type rule = findMatchingRule(text, currentIndex, lenghtOfMatch);

        switch (rule)
        {
        case SyntaxRule::ClassRule:
            setFormat(currentIndex, lenghtOfMatch, formats[ClassFormat]);
            break;

        case SyntaxRule::BuiltinRule:
            setFormat(currentIndex, lenghtOfMatch, formats[BuiltinFormat]);
            break;

        case SyntaxRule::PrimitiveRule:
            setFormat(currentIndex, lenghtOfMatch, formats[PrimitiveFormat]);
            break;

        case SyntaxRule::KeywordRule:
            setFormat(currentIndex, lenghtOfMatch, formats[KeywordFormat]);
            break;

        case SyntaxRule::SymbolRule:
            setFormat(currentIndex, lenghtOfMatch, formats[SymbolFormat]);
            break;

        case SyntaxRule::SymbolArgRule:
            // Omit the trailing ":" that was included in the regexp:
            setFormat(currentIndex, lenghtOfMatch-1, formats[SymbolFormat]);
            break;

        case SyntaxRule::EnvVarRule:
            setFormat(currentIndex, lenghtOfMatch, formats[EnvVarFormat]);
            break;

        case SyntaxRule::StringRule:
            setFormat(currentIndex, lenghtOfMatch, formats[StringFormat]);
            break;

        case SyntaxRule::CharRule:
            setFormat(currentIndex, lenghtOfMatch, formats[CharFormat]);
            break;

        case SyntaxRule::SingleLineCommentRule:
            setFormat(currentIndex, lenghtOfMatch, formats[CommentFormat]);
            break;

        case SyntaxRule::FloatRule:
        case SyntaxRule::HexIntRule:
        case SyntaxRule::RadixFloatRule:
            setFormat(currentIndex, lenghtOfMatch, formats[NumberFormat]);
            break;

        case SyntaxRule::MultiLineCommentStartRule:
            setFormat(currentIndex, lenghtOfMatch, formats[CommentFormat]);
            currentIndex += lenghtOfMatch;
            currentState = inComment;
            return;

        case SyntaxRule::NoRule:
            currentIndex += 1;

        default:
            ;
        }

        currentIndex += lenghtOfMatch;
    } while (currentIndex < text.size());
}

void SyntaxHighlighter::highlightBlockInString(const QString& text, int& currentIndex, int& currentState)
{
    const QRegExp &expr = mGlobals->mInStringRegexp;
    int matchIndex = expr.indexIn(text, currentIndex);
    if (matchIndex == -1)
        assert(false);

    int matchLength = expr.matchedLength();
    setFormat(currentIndex, matchLength, mGlobals->format(StringFormat));
    currentIndex += matchLength;
    if (currentIndex == text.size()) {
        // end of block
        currentState = inString;
        return;
    }

    if (text[currentIndex] == QChar('\"'))
        currentState = inCode;

    setFormat(currentIndex, 1, mGlobals->format(StringFormat));
    ++currentIndex;
    return;
}

void SyntaxHighlighter::highlightBlockInSymbol(const QString& text, int& currentIndex, int& currentState)
{
    const QRegExp &expr = mGlobals->mInSymbolRegexp;
    int matchIndex = expr.indexIn(text, currentIndex);
    if (matchIndex == -1)
        assert(false);

    int matchLength = expr.matchedLength();
    setFormat(currentIndex, matchLength, mGlobals->format(SymbolFormat));
    currentIndex += matchLength;
    if (currentIndex == text.size()) {
        // end of block
        currentState = inSymbol;
        return;
    }

    if (text[currentIndex] == QChar('\''))
        currentState = inCode;

    setFormat(currentIndex, 1, mGlobals->format(SymbolFormat));
    ++currentIndex;
    return;
}

void SyntaxHighlighter::highlightBlockInComment(const QString& text, int& currentIndex, int& currentState)
{
    int index = currentIndex;
    int maxIndex = text.size() - 1;

    while(index < maxIndex)
    {
        if (text[index] == '/' && text[index+1] == '*') {
            ++currentState;
            index += 2;
        }
        else if (text[index] == '*' && text[index+1] == '/') {
            --currentState;
            index += 2;
        }
        else
            index += 1;

        if (currentState < inComment) {
            currentState = inCode;
            break;
        }
    }

    if(currentState == inCode) {
        setFormat(currentIndex, index - currentIndex, mGlobals->format(CommentFormat));
        currentIndex = index;
    }
    else {
        setFormat(currentIndex, text.size() - currentIndex, mGlobals->format(CommentFormat));
        currentIndex = text.size();
    }

    return;
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    int currentIndex = 0;

    int currentState = previousBlockState();
    if (currentState == -1)
        currentState = 0;

    BlockData *blockData = static_cast<BlockData*>(currentBlockUserData());
    if(!blockData) {
        blockData = new BlockData;
        setCurrentBlockUserData(blockData);
    }
    else {
        blockData->brackets.clear();
    }

    while (currentIndex < text.size()) {
        switch (currentState) {
        case inCode:
            highlightBlockInCode(text, currentIndex, currentState);
            break;

        case inString:
            highlightBlockInString(text, currentIndex, currentState);
            break;

        case inSymbol:
            highlightBlockInSymbol(text, currentIndex, currentState);
            break;

        default:
            if(currentState >= inComment)
                highlightBlockInComment(text, currentIndex, currentState);
        }
    }
    setCurrentBlockState(currentState);
}

}
