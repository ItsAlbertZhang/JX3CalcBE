local skillDict = {
    ["日轮"] = 3962,
    ["月轮"] = 3959,
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
    ["光明相"] = 3969,
}

local skillQueueQishou = {
    -- "日斩", "月破", "日破", "光明相", "驱夜", "月斩",
    "日斩", "月破", "日破", "驱夜", "月斩",
}

local skillChanggui = {
    "月悬", "日劫", "隐身", "诛邪", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "日斩", "驱夜", "日破", "月斩", "月劫", "日斩", "诛邪", "驱夜", "日破", "月斩", "月破", "日斩", "诛邪", "驱夜", "日破", "月斩", "月破",
    "日斩", "诛邪", "驱夜", "日破", "月轮", "月劫", "月斩", "诛邪", "月轮", "月斩", "月破", "日斩", "驱夜", "日破", "诛邪", "月轮", "月破",
    "月斩", "日斩", "驱夜",
}

local idx = 1

function Init()
    idx = 1;
end;

MacroNum = 2;

function Macro0(player)
    local id = skillDict[skillQueueQishou[idx]];
    if player:cast(id) then
        idx = idx + 1;
        if idx > #skillQueueQishou then
            idx = 1;
            player.macroIdx = 1;
            player.nCurrentSunEnergy = 10000; -- 手动给个光明相, 测试点崇光的情况下该循环的健壮性
        end
    end
end

function Macro1(player)
    local id = skillDict[skillChanggui[idx]];
    if idx == 3 then -- 隐身前开腰坠
        player:itemUse(1, 38789);
    end
    if player:cast(id) then
        idx = idx + 1;   -- idx 此时为下一个技能的索引
        if idx == 3 then -- 下一个技能是隐身
            player.delayCustom = 400;
        end
        if idx > #skillChanggui then
            idx = 1;
        end
    end
end
