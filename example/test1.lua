function Init()
end

MacroNum = 2; -- 自定义宏数量.

function Macro0(player)
    player:cast(3967);
    if player.nCurrentSunEnergy < 10000 and player.nCurrentMoonEnergy < 10000 then
        player.macroIdx = 1;
    end
end

function Macro1(player)
    player:cast(3963); -- 烈日斩
    player:cast(3960); -- 银月斩
end
