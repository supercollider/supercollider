" Vim color file
" This is a modified version of dark blue by 
" Bohdan Vlasyuk <bohdan@vstu.edu.ua>

" it has a makes symbols, chars and strings color slightly differently

set bg=dark
hi clear
if exists("syntax_on")
	syntax reset
endif

let colors_name = "alex-darkblue"

hi Normal		guifg=#c0c0c0 guibg=#000040						ctermfg=gray
hi ErrorMsg		guifg=#ffffff guibg=#287eff						ctermfg=white
hi Visual		guifg=#8080ff guibg=fg		gui=reverse				ctermfg=lightblue cterm=reverse
hi VisualNOS	guifg=#8080ff guibg=fg		gui=reverse,underline	ctermfg=lightblue cterm=reverse,underline
hi Todo			guifg=#d14a14 guibg=#1248d1						ctermfg=red
hi Search		guifg=#90fff0 guibg=#2050d0						ctermfg=white cterm=underline term=underline
hi IncSearch	guifg=#b0ffff guibg=#2050d0							ctermfg=darkblue

hi SpecialKey		guifg=cyan			ctermfg=darkcyan
hi Directory		guifg=cyan			ctermfg=cyan
hi Title			guifg=magenta gui=none ctermfg=magenta cterm=bold
hi WarningMsg		guifg=red			ctermfg=red
hi WildMenu			guifg=yellow guibg=black ctermfg=yellow ctermbg=black cterm=none term=none
hi ModeMsg			guifg=#22cce2		ctermfg=lightblue
hi MoreMsg			ctermfg=darkgreen	ctermfg=darkgreen
hi Question			guifg=green gui=none ctermfg=green cterm=none
hi NonText			guifg=#0030ff		ctermfg=darkblue

hi StatusLine		guifg=blue guibg=darkgray gui=none		ctermfg=blue ctermbg=gray term=none cterm=none
hi StatusLineNC		guifg=black guibg=darkgray gui=none		ctermfg=black ctermbg=gray term=none cterm=none
hi VertSplit		guifg=black guibg=darkgray gui=none		ctermfg=black ctermbg=gray term=none cterm=none

hi Folded			guifg=#808080 guibg=#000040 ctermfg=white ctermbg=darkblue cterm=bold term=bold
hi FoldColumn		guifg=#808080 guibg=#000040			ctermfg=darkgrey ctermbg=black cterm=bold term=bold
hi LineNr			guifg=#90f020			ctermfg=green cterm=none

hi DiffAdd			guibg=darkblue	ctermbg=darkblue term=none cterm=none
hi DiffChange		guibg=darkmagenta ctermbg=magenta cterm=none
hi DiffDelete		ctermfg=blue ctermbg=cyan gui=bold guifg=Blue guibg=DarkCyan
hi DiffText			cterm=bold ctermbg=red gui=bold guibg=Red

hi Cursor			guifg=#000020 guibg=#ffaf38 ctermbg=brown
hi lCursor			guifg=#ffffff guibg=#000000 ctermbg=darkgreen


hi Comment			guifg=#80a0ff ctermfg=darkred
hi Constant			ctermfg=magenta guifg=magenta cterm=none
hi scString			ctermfg=magenta guifg=magenta cterm=bold
hi scChar			ctermfg=white guifg=white cterm=none guibg=magenta ctermbg=magenta
hi Special			ctermfg=brown guifg=orange cterm=none gui=none
hi Identifier		ctermfg=cyan guifg=cyan cterm=none
hi Statement		ctermfg=yellow cterm=none guifg=yellow gui=none
hi PreProc			ctermfg=magenta guifg=magenta gui=none cterm=none
hi Type				ctermfg=green guifg=green gui=none cterm=bold
hi Underlined		cterm=underline term=underline


