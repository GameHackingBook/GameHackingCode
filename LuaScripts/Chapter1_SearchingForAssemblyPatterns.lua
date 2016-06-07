BASEADDRESS = getAddress("Game.exe")
function LocatePacketCreation(packetType)
    for address = BASEADDRESS, (BASEADDRESS + 0x2ffffff) do
        local push = readBytes(address, 1, false)
        local type = readInteger(address + 1)
        local call = readInteger(address + 5)
        if (push == 0x68 and type == packetType and call == 0xE8) then
            return address
        end
    end
    return 0
end

FUNCTIONHEADER = { 0xCC, 0x55, 0x8B, 0xEC, 0x6A }
function LocateFunctionHead(checkAddress)
    if (checkAddress == 0) then return 0 end
    for address = checkAddress, (checkAddress - 0x1fff), -1 do
        local match = true
        local checkheader = readBytes(address, #FUNCTIONHEADER, true)
        for i, v in ipairs(FUNCTIONHEADER) do
            if (v ~= checkheader[i]) then
                match = false
                break
            end
        end
        if (match) then return address + 1 end
    end
    return 0
end

local funcAddress = LocateFunctionHead(LocatePacketCreation(0x64))
if (funcAddress ~= 0) then
    print(string.format("0x%x",funcAddress))
else
    print("Not found!")
end