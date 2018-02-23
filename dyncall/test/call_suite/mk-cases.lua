require"math"
local max = math.max
local maxargs = 0

function trim(l) return l:gsub("^%s+",""):gsub("%s+$","") end
function mkcase(id,sig)
  local sig = trim(sig)
  local h = { "/* ",id,":",sig," */ ",sig:sub(1,1), " f", id,"(",""}
  local t = { "fid=",id,";" }
  local pos = 0
  maxargs = max(maxargs, #sig-1)
  for i = 2, #sig do 
    pos = tostring(i-1)
    local name = "a"..pos
    local ch   = sig:sub(i,i)
    
    h[#h+1] = ch
    h[#h+1] = " "
    h[#h+1] = name
    h[#h+1] = ","

    t[#t+1] = "V_"
    t[#t+1] = ch
    t[#t+1] = "["
    t[#t+1] = pos
    t[#t+1] = "]"
    t[#t+1] = "="
    t[#t+1] = name
    t[#t+1] = ";"
  end
  h[#h] = "){"
  t[#t+1] = "ret_"
  t[#t+1] = sig:sub(1,1)
  t[#t+1] = "("
  t[#t+1] = pos
  t[#t+1] = ")"
  t[#t+1] = "}\n"
  return table.concat(h,"")..table.concat(t,"")
end

function mkfuntab(n)
  local s = { "funptr G_funtab[] = {\n"}
  for i = 1, n do
    s[#s+1] = "\t(funptr)&f"..i..",\n"
  end
  s[#s+1] = "};\n"
  return table.concat(s,"")
end

function mksigtab(sigs)
  local s = { "char const * G_sigtab[] = {\n"}
  for k,v in pairs(sigs) do
    s[#s+1] = '\t"'
    s[#s+1] = v
    s[#s+1] = '",\n'
  end
  s[#s+1] = "};\n"
  return table.concat(s,"")
end

function mkall()
  local lineno = 1
  local sigtab = { }
  for line in io.lines() do
    local sig = trim(line)
    io.write(mkcase(lineno,sig))
    sigtab[#sigtab+1] = sig
    lineno = lineno + 1
  end
  io.write(mkfuntab(lineno-1))
  io.write(mksigtab(sigtab))
  io.write("int G_maxargs = "..maxargs..";\n")
end

mkall()
-- print(mkcase(1,"vififififi"))

