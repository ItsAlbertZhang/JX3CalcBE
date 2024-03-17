---------- 使用低级接口的 lua ----------
function Init() end;

MacroNum = 2;

function Macro0(player)
    if (player.nCurrentMoonEnergy == 8000) then
        player:macroSkillCast("悬象著明");
    end
    if (player:macroBuff("诛邪镇魔") > 0) then
        player:macroSkillCast("光明相");
    end
    if (player.nCurrentSunEnergy == 6000) then
        player:macroSkillCast("暗尘弥散");
    end
    if (player.nCurrentSunEnergy == 6000) then
        player:macroSkillCast("诛邪镇魔");
    end
    player:macroSkillCast("净世破魔击");
    if (player.nCurrentMoonEnergy == 2000) then
        player:macroSkillCast("驱夜断愁");
    end
    player:macroSkillCast("烈日斩");
    if (player.nCurrentSunEnergy > 9900 and (player.nCurrentMoonEnergy == 8000)) then
        player.macroIdx = 1;
    end
end

function Macro1(player)
    player:macroSkillCast("诛邪镇魔");
    player:macroSkillCast("暗尘弥散");
    player:macroSkillCast("光明相");
    player:macroSkillCast("悬象著明");
    player:macroSkillCast("生死劫");
    player:macroSkillCast("净世破魔击");
    if (player.nCurrentSunEnergy < 4100 and (player.nCurrentMoonEnergy < 4100)) then
        player:macroSkillCast("驱夜断愁");
    end
    player:macroSkillCast("烈日斩");
    player:macroSkillCast("银月斩");
    player:macroSkillCast("赤日轮");
end
