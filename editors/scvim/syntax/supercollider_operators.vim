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

syn keyword  scUnaryoperator  neg reciprocal abs floor ceil frac sign squared cubed sqrt exp midicps cpsmidi midiratio ratiomidi dbamp ampdb octcps cpsoct log log2 log10 sin cos tan asin acos atan sinh cosh tanh distort softclip isPositive isNegative isStrictlyPositive

syn keyword  scBinaryoperator  min max round trunc atan2 hypot hypotApx ring1 ring2 ring3 ring4 sumsqr difsqr sqrsum sqrdif absdif thresh amclip scaleneg clip2 wrap2 fold2 excess + - *

syn match scBinaryoperator "+"
syn match scBinaryoperator "-"
syn match scBinaryoperator "*"
syn match scBinaryoperator "/"
syn match scBinaryoperator "%"
syn match scBinaryoperator "\*\*"
syn match scBinaryoperator "<"
syn match scBinaryoperator "<="
syn match scBinaryoperator ">"
syn match scBinaryoperator "<>"
syn match scBinaryoperator ">="
syn match scBinaryoperator "="
syn match scBinaryoperator "=="
syn match scBinaryoperator "==="
syn match scBinaryoperator "!="
syn match scBinaryoperator "!=="
syn match scBinaryoperator "&"
syn match scBinaryoperator "|"
syn match scBinaryoperator "<!"
syn match scBinaryoperator "?"
syn match scBinaryoperator "??"
syn match scBinaryoperator "!?"
syn match scBinaryoperator "!"
syn match scBinaryoperator "#"
syn match scBinaryoperator "_"
syn match scBinaryoperator "\.\."
syn match scBinaryoperator "\.\.\."
syn match scBinaryoperator "`"
syn match scBinaryoperator ":"
