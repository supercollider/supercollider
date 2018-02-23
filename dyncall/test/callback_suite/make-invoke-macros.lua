require "config"
if mode == "ordered" then
  minargs = 0
end
local i
for i = minargs, maxargs do
  local line = "#define F" .. i .. "(ID,R"
  local argdef = ""
  local argset = ""
  if i > 0 then
    line = line .. ","
    local j
    for j = 0, i-1 do
      if j > 0 then 
        argdef = argdef .. ","
        argset = argset .. ","
      end
      argdef = argdef .. "A" .. j
      argset = argset .. "A[" .. j .. "].A" .. j
    end
  end
  line = line .. argdef .. ") void ID(void* addr) { Result.R = ((CONFIG_API R(*)("  .. argdef .. "))addr)(" .. argset .. ");}\n"
  io.write(line)
end
io.flush()


