"Copyright 2007 Alex Norman
"
"This file is part of SCVIM.
"
"SCVIM is free software: you can redistribute it and/or modify
"it under the terms of the GNU General Public License as published by
"the Free Software Foundation, either version 3 of the License, or
"(at your option) any later version.
"
"SCVIM is distributed in the hope that it will be useful,
"but WITHOUT ANY WARRANTY; without even the implied warranty of
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
"GNU General Public License for more details.
"
"You should have received a copy of the GNU General Public License
"along with SCVIM.  If not, see <http://www.gnu.org/licenses/>.
"
" Vim syntax file
" Language:	supercollider	
" Maintainer:	alex norman <alex@x37v.info>
" Version:	0.1
" Last change:	2007-11-21

syn clear

syn match	scAoperator	"{"
syn match	scAoperator	"}"

"syn	match	scVariable	"\%(var.*\)\@<=\(\l\w*\)" "lowercase followed by wordchar
syn	match	scGlobvariable	"\~\l\w*" "~ followed by lowercase followed by wordchar
syn	match scVar "\s*var\s"
syn	match scVar "\s*classvar\s"
syn	match scArg "\s*arg\s"

" symbols, strings, etc
"syn match	scSymbol "\(\w\\\)\@<!\'.\{-}\'" "\{-} is a non greedy version of *
"syn match	scSymbol "\(\w\|\\\)\@<!\'.\{-}\(\\\)\@<!\'" "\{-} is a non greedy version of *

syn match	scSymbol "\v(\w|\\)@<!\'.{-}(\\)@<!\'" "\{-} is a non greedy version of *

"syn match	scSymbol "\$\@<!\\\w\w*"
syn match	scSymbol "\v\$@<!\\\w\w*"
syn match	scSymbol "\\\\"

syn match	scChar	"\$\w"
syn match	scChar	"\$\\\\"
syn match	scChar	"\$\\\w"

"syn match	scString "\".\{-}\""
"syn region	scString start=+\v\\@<!"+ end=+\v\\@<!"+ 
syn region  scString start=+"+ skip=+\\\\\|\\"+ end=+"+

"syn match	scClass "\u\w*"	"upper case followed by any wordchar

syn keyword	scSpecialval nil true false inf this

"syn keyword	repeat do for while

"syn region	literalarray start="\#\[" end="\]"
"need to figure out how to make it match the mattching braket

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
" rate

"syn match scFun "\.\@<=\a\w*"

syn match scArate "\v\.@<=ar(\w)@!"
syn match scKrate "\v\.@<=kr(\w)@!"

" load list of all objects
if filereadable($SCVIM_CACHE_DIR . "/supercollider_objects.vim")
	source	$SCVIM_CACHE_DIR/supercollider_objects.vim
elseif filereadable($HOME . ".scvim/supercollider_objects.vim")
	source	$HOME/.scvim/supercollider_objects.vim
end
" load all the language stuff
runtime! syntax/supercollider_lang.vim
runtime! syntax/supercollider_operators.vim

" comments
syn match	scComment	"//.*$"
syn region	scComment	start="/\*" end="\*/" contains=scComment keepend extend


" COLOR DEFINITIONS

"hi link	scFun	Function
"
"hi link	scUgen	Function
"hi link	scLang	Function
"
"hi link	scBinaryoperator Operator
"hi link	scUnaryoperator	Operator
"
"hi link	scAoperator	Delimiter
"hi link	scArate	Delimiter
"hi link	scKrate	Delimiter
"
"hi link	scSymbol String
"
"hi link	scString String
"
"hi link	scSpecialval Special
"
""hi link	scRepeat Repeat
"
""hi link	scVariable  	String
"hi link	scGlobVariable  	Statement
"
"hi link	scComment	Comment
"
"hi link	scVar 	Question
"hi link	scArg 	Question

"Here I have defined my own syntax colors
"you can customize these however you like

"hi scObject	term=bold ctermfg=cyan guifg=cyan
"hi scLang	term=bold ctermfg=lightcyan guifg=lightcyan
"
"hi scBinaryoperator	term=bold ctermfg=brown guifg=brown
"hi scUnaryoperator	term=bold ctermfg=brown guifg=brown
"
"hi scAoperator	term=bold ctermfg=darkyellow guifg=darkyellow
"hi scArate	term=bold ctermfg=darkyellow guifg=darkyellow
"hi scKrate	term=bold ctermfg=darkyellow guifg=darkyellow
"
"hi scSymbol	term=bold ctermfg=red guifg=red
"
"hi scString	term=bold ctermfg=darkred guifg=darkred
"
"hi scChar	term=bold ctermfg=lightred guifg=lightred
"
"hi scSpecialval	term=bold ctermfg=magenta guifg=magenta
"
"hi scGlobVariable	term=bold ctermfg=yellow guifg=yellow
"
"hi scComment	term=bold ctermfg=darkblue guifg=darkblue
"
"hi scVar	term=bold ctermfg=green guifg=green
"hi scArg	term=bold ctermfg=green guifg=green


hi link scObject Identifier
hi link scLang Identifier

hi link scBinaryoperator Special
hi link scUnaryoperator Special

hi link scAoperator Statement
hi link scArate Statement
hi link scKrate Statement

hi link scSymbol String

hi link scString String

hi link scChar String

hi link scSpecialval Constant

hi link scGlobVariable Define

hi link scComment Comment

hi link scVar Type
hi link scArg Type

let b:current_syntax = "supercollider"
