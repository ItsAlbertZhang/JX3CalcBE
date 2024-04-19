local skillDict = {
    ["日斩"] = 3963,
    ["月斩"] = 3960,
    ["日破"] = 3967,
    ["月破"] = 3967,
    ["日劫"] = 3966,
    ["月劫"] = 3966,
    ["月悬"] = 34347,
    ["隐身"] = 3974,
    ["驱夜"] = 3979,
    ["诛邪"] = 22890,
    ["崇光"] = 37335,
}

local skillQueueQishou = {
    "日斩", "月斩",
}

local skillQueue6chong = {
    "月悬", "日劫", "隐身", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "日斩", "驱夜", "日破", "月斩", "月劫", "崇光", "崇光", "崇光", "诛邪", "日斩", "驱夜", "日破", "月斩", "月破", "诛邪",
    "日斩", "驱夜", "日破", "月斩", "月劫", "崇光", "崇光", "崇光", "诛邪", "日斩", "驱夜", "日破", "月斩", "月破", "诛邪",
    "月斩", "日斩", "驱夜",
}

local skillQueue9chong = {
    "月悬", "日劫", "隐身", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "日斩", "崇光", "崇光", "崇光", "驱夜", "日破", "月斩", "月劫", "崇光", "崇光", "崇光", "诛邪", "日斩", "驱夜", "日破", "月斩", "月破", "诛邪",
    "日斩", "驱夜", "日破", "月斩", "月劫", "诛邪", "日斩", "驱夜", "日破", "月斩", "月破", "崇光", "崇光", "崇光", "诛邪",
    "月斩", "日斩", "驱夜",
}

local skillQueueShouwei = {
    "月悬", "日劫", "隐身", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "日斩", "崇光", "崇光", "崇光",
}

local idx = 1
local midx = 0

function Init() end;

MacroNum = 4;

function Macro0(player)
    local id = skillDict[skillQueueQishou[idx]];
    if player:cast(id) then
        idx = idx + 1;
        if idx > #skillQueueQishou then
            idx = 1;
            player.macroIdx = 1;
        end
    end
end

function Macro1(player)
    local id = skillDict[skillQueue6chong[idx]];
    if player:cast(id) then
        idx = idx + 1;
        if idx == 3 then -- 隐身不占 GCD
            player.delayCustom = 400;
        end
        if idx > #skillQueue6chong then
            idx = 1;
            player.macroIdx = 2;
        end
    end
end

function Macro2(player)
    local id = skillDict[skillQueue9chong[idx]];
    if idx == 11 then -- 第 11 个技能前开腰坠
        player:itemUse(1, 38789);
    end
    if player:cast(id) then
        idx = idx + 1;
        if idx == 3 then -- 隐身不占 GCD
            player.delayCustom = 400;
        end
        if idx > #skillQueue9chong then
            idx = 1;
            midx = midx + 1;
            if midx == 3 then
                player.macroIdx = 3;
            else
                player.macroIdx = 1;
            end
        end
    end
end

function Macro3(player)
    local id = skillDict[skillQueueShouwei[idx]];
    if player:cast(id) then
        idx = idx + 1;
        if idx == 3 then -- 隐身不占 GCD
            player.delayCustom = 400;
        end
    end
end
