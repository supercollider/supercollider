"SuperCollider/Vim interaction scripts
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

"au VimLeave

"if exists("$SCVIM_DIR") == 0
" echo "$SCVIM_DIR must be defined for SCVIM to work"
" finish
"endif


"source the syntax file as it can change
"so $SCVIM_DIR/syntax/supercollider.vim
runtime! syntax/supercollider.vim

if exists("loaded_scvim") || &cp
  finish
endif
let loaded_scvim = 1

"first if SCVIM_CACHE_DIR is defined, use that,
"otherwise use ~/.scvim
if exists("$SCVIM_CACHE_DIR") 
  let s:scvim_cache_dir = $SCVIM_CACHE_DIR
else
  let s:scvim_cache_dir = $HOME . "/.scvim"
  let $SCVIM_CACHE_DIR = s:scvim_cache_dir
endif

"source the scvimrc file if it exists
if filereadable($HOME . "/.scvimrc")
  source $HOME/.scvimrc
end

"add the cache dir to SCVIM_CACHE_DIR
set runtimepath+=$SCVIM_CACHE_DIR

if exists("g:sclangKillOnExit")
  let s:sclangKillOnExit = g:sclangKillOnExit
else
  let s:sclangKillOnExit = 1
endif

if exists("g:sclangPipeLoc")
  let s:sclangPipeLoc = g:sclangPipeLoc
else
  let s:sclangPipeLoc = "/tmp/sclang-pipe"
endif
let $SCVIM_PIPE_LOC = s:sclangPipeLoc

if exists("g:sclangPipeAppPidLoc")
  let s:sclangPipeAppPidLoc = g:sclangPipeAppPidLoc
else
  let s:sclangPipeAppPidLoc = "/tmp/sclangpipe_app-pid"
endif
let $SCVIM_PIPE_PID_LOC = s:sclangPipeAppPidLoc

if exists("g:sclangTerm")
  let s:sclangTerm = g:sclangTerm
else
  let s:sclangTerm = "xterm -e"
endif

if exists("g:sclangPipeApp")
  let s:sclangPipeApp = g:sclangPipeApp
else
  let s:sclangPipeApp = "sclangpipe_app"
endif

" Check if scvim in tmux and screen should be split vertically (default)
if exists("g:scvimSplitVertical")
  let s:scvimSplitVertical = g:scvimSplitVertical
else
  let s:scvimSplitVertical = 1
endif

" Check if size of tmux split or screen region is set (in lines),
" otherwise set default (30 lines/blocks)
if exists("g:scvimSplitSize")
  let s:scvimSplitSize = g:scvimSplitSize
else
  let s:scvimSplitSize = 30
endif

"function SClangRunning()
" if s:sclang_pid != 0 && `pidof "#{$sclangsclangPipeApp_no_quotes}"`.chomp != ""
"   return true
" else
"   $sclang_pid = 0
"   return false
" end
"end


function! FindOuterMostBlock()
  "search backwards for parens dont wrap
  let l:search_expression_up = "call searchpair('(', '', ')', 'bW'," .
    \"'synIDattr(synID(line(\".\"), col(\".\"), 0), \"name\") =~? \"scComment\" || " .
    \"synIDattr(synID(line(\".\"), col(\".\"), 0), \"name\") =~? \"scString\" || " .
    \"synIDattr(synID(line(\".\"), col(\".\"), 0), \"name\") =~? \"scSymbol\"')"
  "search forward for parens, don't wrap
  let l:search_expression_down = "call searchpair('(', '', ')', 'W'," .
    \"'synIDattr(synID(line(\".\"), col(\".\"), 0), \"name\") =~? \"scComment\" || " .
    \"synIDattr(synID(line(\".\"), col(\".\"), 0), \"name\") =~? \"scString\" || " .
    \"synIDattr(synID(line(\".\"), col(\".\"), 0), \"name\") =~? \"scSymbol\"')"

  "save our current cursor position
  let l:returnline = line(".")
  let l:returncol = col(".")

  "if we're on an opening paren then we should actually go to the closing one to start the search
  "if buf[l:returnline][l:returncol-1,1] == "("
  if strpart(getline(line(".")),col(".") - 1,1) == "("
    exe l:search_expression_down
  endif

  let l:origline = line(".")
  let l:origcol = col(".")

  "these numbers will define our range, first init them to illegal values
  let l:range_e = [-1, -1]
  let l:range_s = [-1, -1]

  "this is the last line in our search
  let l:lastline = line(".")
  let l:lastcol = col(".")

  exe l:search_expression_up

  while line(".") != l:lastline || (line(".") == l:lastline && col(".") != l:lastcol)
    "keep track of the last line/col we were on
    let l:lastline = line(".")
    let l:lastcol = col(".")
    "go to the matching paren
    exe l:search_expression_down

    "if there isn't a match print an error
    if l:lastline == line(".") && l:lastcol == col(".")
      call cursor(l:returnline,l:returncol)
      throw "UnmachedParen at line:" . l:lastline . ", col: " . l:lastcol
    endif

    "if this is equal to or later than our original cursor position
    if line(".") > l:origline || (line(".") == l:origline && col(".") >= l:origcol)
      let l:range_e = [line("."), col(".")]
      "go back to opening paren
      exe l:search_expression_up
      let l:range_s = [line("."), col(".")]
    else
      "go back to opening paren
      exe l:search_expression_up
    endif
    "find next paren (if there is one)
    exe l:search_expression_up
  endwhile

  "restore the settings
  call cursor(l:returnline,l:returncol)

  if l:range_s[0] == -1 || l:range_s[1] == -1
    throw "OutsideOfParens"
  endif

  "return the ranges
   return [l:range_s, l:range_e]
endfunction


"this causes the sclang pipe / terminal app to be killed when you exit vim, if you don't
"want that to happen then just comment this out
if !exists("loaded_kill_sclang")
  if s:sclangKillOnExit
    au VimLeave * call SClangKill()
  endif
  let loaded_kill_sclang = 1
endif

" Resize the current sclang_pipe tmux split or screen region (positive values
" enlargen, negative values lower the size of the pipe output)
function SCresize(lines)
  " If using tmux resize its pane
  if $TMUX != ""
    " If the tmux split is vertical, resize vertically
    if s:scvimSplitVertical == 1
      if a:lines > 0
        call system("tmux resizep -L ".a:lines)
      elseif a:lines < 0
        call system("tmux resizep -R ".(a:lines*(-1)))
      endif
    else
      if a:lines > 0
        call system("tmux resizep -U ".a:lines)
      elseif a:lines < 0
        call system("tmux resizep -D ".(a:lines*(-1)))
      endif
    endif
  " If using screen, resize its region
  elseif $STY != ""
    " If the screen region is vertical or horizontal, resize accordingly
    if a:lines > 0
      call system("screen -D -R -X resize -".a:lines)
    elseif a:lines < 0
      call system("screen -D -R -X resize +".(a:lines*(-1)))
    endif
  endif
endfunction

"the vim version of SendToSC
function SendToSC(text)
  let l:text = substitute(a:text, '\', '\\\\', 'g')
  let l:text = substitute(l:text, '"', '\\"', 'g')
  let l:cmd = system('echo "' . l:text . '" >> ' . s:sclangPipeLoc)
  "let l:cmd = system('echo "' . l:text . '" >> /tmp/test')
endfunction

function SendLineToSC(linenum)
  let cmd = a:linenum . "w! >> " . s:sclangPipeLoc
  silent exe cmd
  "let cmd = a:linenum . "w! >> /tmp/test" 
  "silent exe cmd
endfunction

let s:trackoflines = ""

function! SClang_send()
  let s:trackoflines .= getline(".") . "\n"
  if line(".") == a:lastline
    call SendToSC(s:trackoflines)
    let s:trackoflines = ""
    call SendToSC('')
    "redraw!
  endif
endfunction

function SClangStart()
  if !filewritable(s:sclangPipeAppPidLoc)
    " If tmux is running, spawn a seperate split for the sclang_pipe and
    " rename the window to scvim
    if $TMUX != ""
      if s:scvimSplitVertical == 1
        call system("tmux splitw -h -p ". s:scvimSplitSize ."; tmux send-keys " . s:sclangPipeApp . " C-m; tmux selectp -l; sleep 1; tmux renamew scvim")
      else
        call system("tmux splitw -v -p ". s:scvimSplitSize ."; tmux send-keys " . s:sclangPipeApp . " C-m; tmux selectp -l; sleep 1; tmux renamew scvim")
      endif
    " If screen is running, spawn a seperate region for the sclang_pipe and
    " change the window's caption to scvim
    elseif $STY != ""
      if s:scvimSplitVertical == 1
        " Open screen with vertical region
        call system("screen -D -R -X split -v; screen -D -R -X focus; screen -D -R -X resize ". s:scvimSplitSize ."; screen -D -R -X screen " . s:sclangPipeApp . "; screen -D -R -X focus")
      else
        " Open screen with horizontal region
        call system("screen -D -R -X split; screen -D -R -X focus; screen -D -R -X resize ". s:scvimSplitSize ."; screen -D -R -X screen " . s:sclangPipeApp . "; screen -D -R -X focus")
      endif
      " set screen caption to scvim
      call system("screen -D -R -X caption string scvim")
    else
      call system(s:sclangTerm . " " . s:sclangPipeApp . "&")
    endif
  else
    "TODO: If this happens, make sure to remove the pipe and restart scvim
    throw s:sclangPipeAppPidLoc . " exists, is " . s:sclangPipeApp . " running?  If not try deleting " . s:sclangPipeAppPidLoc
  endif
endfunction

function SClangKill()
  if filewritable(s:sclangPipeAppPidLoc)
    call SendToSC("Server.quitAll;")
    :sleep 10 m
    call system("kill $(pidof cat " . s:sclangPipeAppPidLoc . "); for i in $(pidof ruby $(which sclangpipe_app)); do kill $i; done; kill $(pidof cat " . s:sclangPipeLoc . "); rm " . s:sclangPipeAppPidLoc . "; rm " . s:sclangPipeLoc )
    " If using tmux, kill remaining panes and rename the window back to the
    " shell currently in use (not necessarily the one in $SHELL!)
    if $TMUX != ""
      call system("tmux killp -t 1; tmux renamew $(ps -p $$|grep pts|cut -c16-|cut -d ' ' -f2)")
    elseif $STY != ""
      "If screen is running, kill its region spawned to contain sclangpipe_app
      call system("screen -D -R -X only")
    endif
  end
endfunction

function SClangRestart()
  if filewritable(s:sclangPipeAppPidLoc)
    " Kill all processes, that were spawned
    call system("kill $(pidof cat " . s:sclangPipeAppPidLoc . "); for i in $(pidof ruby $(which sclangpipe_app)); do kill $i; done; kill $(pidof cat " . s:sclangPipeLoc . "); rm " . s:sclangPipeAppPidLoc . "; rm " . s:sclangPipeLoc )
    " If tmux is running, kill the pane for sclangpipe_app
    if $TMUX != ""
      call system("tmux killp -t 1")
    elseif $STY != ""
      "If screen is running, kill its region spawned to contain sclangpipe_app
      call system("screen -D -R -X only")
    endif
    call SClangStart()
  else
    call SClangStart()
  end
endfunction

function SClang_free(server)
  call SendToSC('s.freeAll;')
  redraw!
endfunction

function SClang_thisProcess_stop()
  call SendToSC('thisProcess.stop;')
  redraw!
endfunction

function SClang_TempoClock_clear()
  call SendToSC('TempoClock.default.clear;')
  redraw!
endfunction

function! SClang_block()
  let [blkstart,blkend] = FindOuterMostBlock()
  "blkstart[0],blkend[0] call SClang_send()
  "these next lines are just a hack, how can i do the above??
  let cmd = blkstart[0] . "," . blkend[0] . " call SClang_send()"
  let l:origline = line(".")
  let l:origcol = col(".")
  exe cmd
  call cursor(l:origline,l:origcol)
  
  ""if the range is just one line
  "if blkstart[0] == blkend[0]
  " "XXX call SendToSC(strpart(getline(blkstart[0]),blkstart[1] - 1, (blkend[1] - blkstart[1] + 1)))
  " call SendLineToSC(blkstart[0])
  "else
  " let linen = blkstart[0] - 1
  " "send the first line as it might not be a full line
  " "XXX let line = getline(linen)
  " "XXX call SendToSC(strpart(line, blkstart[1] - 1))
  " call SendLineToSC(linen)
  " let linen += 1
  " let endlinen = blkend[0]
  " while linen < endlinen
  "   "XXX call SendToSC(getline(linen))
  "   call SendLineToSC(linen)
  "   let linen += 1
  " endwhile
  " "send the last line as it might not be a full line
  " "XXX let line = getline(endlinen)
  " "XXX call SendToSC(strpart(line,0,blkend[1]))
  " call SendLineToSC(endlinen)
  "endif
  "call SendToSC('')
endfunction

function SCdef(subject)
  let l:tagfile = s:scvim_cache_dir . "/TAGS_SCDEF"
  let l:tagdest = s:scvim_cache_dir . "/doc/tags"

  if !filereadable(l:tagfile)
    echo "definition tag cache does not exist, you must run SCVim.updateCaches in supercollider"
    let l:dontcare = system("echo 'SC:SCVim SCVim.scd /^' > " . l:tagdest)
    exe "help SC:SCVim"
  else
    let l:dontcare = system("grep SCdef:" . a:subject . " " . l:tagfile . " > " . l:tagdest)
    exe "help SCdef:" . a:subject
  end
endfun

function SChelp(subject)
  let l:tagfile = s:scvim_cache_dir . "/doc/TAGS_HELP"
  let l:tagdest = s:scvim_cache_dir . "/doc/tags"
  if !filereadable(l:tagfile)
    echo "help tag cache does not exist, you must run SCVim.updateHelpCache in supercollider in order have help docs"
    let l:dontcare = system("echo 'SC:SCVim SCVim.scd /^' > $SCVIM_CACHE_DIR/doc/tags")
    exe "help SC:SCVim"
    return
  end

  "the keybindings won't find * but will find ** for some reason
  if a:subject == ""
    let l:dontcare = system("grep \"SC:Help\" " . l:tagfile . " > " . l:tagdest)
    exe "help SC:Help"
  elseif a:subject == "*"
    let l:dontcare = system("grep \"SC:\\*\" " . l:tagfile . " > " . l:tagdest)
    exe "help SC:\*" . a:subject
  elseif a:subject == "**"
    let l:dontcare = system("grep \"SC:\\*\\*\" " . l:tagfile . " > " . l:tagdest)
    exe "help SC:\*\*" . a:subject
  else
    let l:dontcare = system("grep SC:\"" . a:subject . "\" " . l:tagfile . " > " . l:tagdest)
    exe "help SC:" . a:subject
  endif
endfun

" search help files for word under the cursor
" or open the HelpBrowser front page
function! HelpBrowser(subject)
    if strlen(a:subject) > 0 && a:subject!~" " && a:subject!~"\t" 
        let string= "HelpBrowser.openHelpFor"
        let format= "(\"" . a:subject . "\");"
        let string= string . format
        call SendToSC(string)
    else
        call SendToSC('Help.gui;')
    endif
endfunction

function ListSCObjects(A,L,P)
  return system("cat $SCVIM_CACHE_DIR/sc_object_completion")
endfun

function ListSCHelpItems(A,L,P)
  return system("cat $SCVIM_CACHE_DIR/doc/sc_help_completion")
endfun


"custom commands (SChelp,SCdef,SClangfree)
com -complete=custom,ListSCHelpItems -nargs=? SChelp call SChelp("<args>")
com -complete=custom,ListSCObjects -nargs=1 SCdef call SCdef("<args>")
com -nargs=1 SClangfree call SClang_free("<args>")
com -nargs=0 SClangStart call SClangStart()
com -nargs=0 SClangKill call SClangKill()
com -nargs=0 SClangRestart call SClangRestart()
com -nargs=1 SCresize call SCresize("<args>")

" end supercollider.vim
