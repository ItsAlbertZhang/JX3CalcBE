-- 这是一个使用技能队列的示例.

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
};

local skillQueueQishou = {
    "日斩", "月斩",
    "月悬", "日劫", "隐身", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "月斩", "日斩", "驱夜", "月破", "日劫", "诛邪", "月斩", "崇光", "崇光", "崇光", "月斩", "月破", "日斩", "驱夜", "日破", "诛邪", "月斩", "月破",
    "日斩", "驱夜", "日劫", "诛邪", "崇光", "崇光", "崇光", "月斩", "月破", "日斩", "驱夜", "日破", "诛邪", "月轮", "月破", "月斩", "日斩", "驱夜",
};

local skillQueue2 = {
    "月悬", "日劫", "隐身", "诛邪", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "月斩", "日斩", "驱夜", "崇光", "崇光", "崇光", "月破", "日劫", "诛邪", "月斩", "崇光", "崇光", "崇光", "月斩", "月破", "日斩", "驱夜", "日破", "诛邪", "月斩", "月破",
    "日斩", "驱夜", "日劫", "诛邪", "月斩", "月破", "日斩", "驱夜", "日破", "诛邪", "月轮", "月破", "崇光", "崇光", "崇光", "月斩", "日斩", "驱夜",
};

local skillQueue3 = {
    "月悬", "日劫", "隐身", "诛邪", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "月斩", "日斩", "驱夜", "崇光", "崇光", "崇光", "月破", "日劫", "诛邪", "月斩", "月斩", "月破", "崇光", "崇光", "崇光", "日斩", "驱夜", "日破", "诛邪", "月斩", "月破",
    "日斩", "驱夜", "日劫", "诛邪", "月斩", "月破", "日斩", "驱夜", "日破", "诛邪", "月轮", "月破", "崇光", "崇光", "崇光", "月斩", "日斩", "驱夜",
};

local skillQueue1 = {
    "月悬", "日劫", "隐身", "诛邪", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "月斩", "日斩", "驱夜", "月破", "日劫", "诛邪", "月斩", "崇光", "崇光", "崇光", "月斩", "月破", "日斩", "驱夜", "日破", "诛邪", "月斩", "月破",
    "日斩", "驱夜", "日劫", "诛邪", "崇光", "崇光", "崇光", "月斩", "月破", "日斩", "驱夜", "日破", "诛邪", "月轮", "月破", "月斩", "日斩", "驱夜",
};

local skillQueueShouwei = {
    "月悬", "日劫", "隐身", "诛邪", "月破", "诛邪", "日破", "驱夜", "诛邪", "月破", "诛邪", "驱夜", "日破", "诛邪", "月破", "诛邪",
    "日斩", "崇光", "崇光", "崇光",
};

local idx = 1;
local midx = 0;

function Init()
    idx = 1;
    midx = 0;
end;

MacroNum = 5;

function Macro0(player)
    local id = skillDict[skillQueueQishou[idx]];
    if player:cast(id) then
        idx = idx + 1;   -- idx 此时为下一个技能的索引
        if idx == 5 then -- 下一个技能是隐身
            player.delayCustom = 400;
        end;
        if idx > #skillQueueQishou then
            idx = 1;
            player.macroIdx = 1;
        end;
    end;
end;

function Macro1(player)
    local id = skillDict[skillQueue2[idx]];
    if player:cast(id) then
        idx = idx + 1;   -- idx 此时为下一个技能的索引
        if idx == 3 then -- 下一个技能是隐身
            player.delayCustom = 400;
        end;
        if idx > #skillQueue2 then
            idx = 1;
            player.macroIdx = 2;
        end;
    end;
end;

function Macro2(player)
    local id = skillDict[skillQueue3[idx]];
    if player:cast(id) then
        idx = idx + 1;   -- idx 此时为下一个技能的索引
        if idx == 3 then -- 下一个技能是隐身
            player.delayCustom = 400;
        end;
        if idx > #skillQueue3 then
            idx = 1;
            if midx == 0 then
                midx = 1;
                player.macroIdx = 3;
            else
                player.macroIdx = 4;
            end;
        end;
    end;
end;

function Macro3(player)
    local id = skillDict[skillQueue1[idx]];
    if player:cast(id) then
        idx = idx + 1;   -- idx 此时为下一个技能的索引
        if idx == 3 then -- 下一个技能是隐身
            player.delayCustom = 400;
        end;
        if idx > #skillQueue1 then
            idx = 1;
            player.macroIdx = 1;
        end;
    end;
end;

function Macro4(player)
    local id = skillDict[skillQueueShouwei[idx]];
    if player:cast(id) then
        idx = idx + 1;   -- idx 此时为下一个技能的索引
        if idx == 3 then -- 下一个技能是隐身
            player.delayCustom = 400;
        end;
        if idx > #skillQueueShouwei then
            idx = 1;
        end;
    end;
end;
