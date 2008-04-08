"this indent function isn't that smart yet, hopefully it'll improve in the future
if exists ("b:did_indent")
	finish
endif
let b:did_indent = 1

setlocal indentexpr=GetSCIndent()
setlocal indentkeys+=0),0],0}

if exists ("*GetSCIndent")
	finish
endif

function GetSCIndent()

	let curr_line = getline(v:lnum)
	let lnum = prevnonblank(v:lnum - 1)

	if lnum == 0
    		return 0
  	endif

	let prev_line = getline(lnum)

	let ind = indent(lnum)

	if prev_line =~ '\(\/\/.*\)\@\<![[({]\s*\([^])}]*\)\=$'
		let ind = ind + &sw
	endif

	if curr_line =~ '\v^\s*[)}\]]'
		"if synIDattr(synID(line("."), col("."), 0), "name") =~? "scComment" ||
		"	synIDattr(synID(line("."), col("."), 0), "name") =~? "scString" ||
		"	synIDattr(synID(line("."), col("."), 0), "name") =~? "scSymbol"
		"	"do nothing
		"else
			let ind = ind - &sw
		"end
	endif

	return ind
endfunction
