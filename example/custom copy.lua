MacroNum = 1; -- 自定义宏数量.

function MacroPrepare(player)
    -- 起手准备
    player:cast(3974);
    if player.nMoonPowerValue == 0 and player.nSunPowerValue == 0 then
        if player.nCurrentMoonEnergy >= 10000 then
            player.nMoonPowerValue = 1;
        elseif player.nCurrentSunEnergy >= 10000 then
            player.nSunPowerValue = 1;
        end
    end
end

function Macro0(player)
    player:cast(3963);      -- 烈日斩
    player:cast(3960);      -- 银月斩
end
