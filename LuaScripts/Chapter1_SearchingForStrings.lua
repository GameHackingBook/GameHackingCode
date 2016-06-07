BASEADDRESS = getAddress("Game.exe")
function findString(str)
    local len = string.len(str)
    local chunkSize = 4096
    local chunkStep = chunkSize - len
    print("Found '" .. str .. "' at:")
    for address = BASEADDRESS, (BASEADDRESS + 0x2ffffff), chunkStep  do
        local chunk = readBytes(address, chunkSize, true)
        if (not chunk) then break end
        for c = 0, chunkSize-len do
            checkForString(address , chunk, c, str, len)
        end
    end
end
function checkForString(address, chunk, start, str, len)
    for i = 1, len do
        if (chunk[start+i] ~= string.byte(str, i)) then
            return false
        end
    end
    print(string.format("\t0x%x", address + start))
end

findString("hello")
findString("world")