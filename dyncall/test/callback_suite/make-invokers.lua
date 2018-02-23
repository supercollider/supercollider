require "config"
local id = 1
for signature in io.lines() do
  local nargs = string.len(signature) - 2
  local rtype = string.sub(signature, nargs + 2, nargs + 2)
  local outline   = "F" .. nargs .. "(f" .. id .. "," .. rtype
  for i = 1, nargs do
    local type  = string.sub(signature, i, i)
    outline = outline .. "," .. type 
  end
  outline = outline .. ")\n"
  io.write( outline )
  id = id + 1
end
io.flush()

