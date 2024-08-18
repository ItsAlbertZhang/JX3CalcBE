-- 这是一个使用 lua 高级接口的示例.

function Init() end;

MacroNum = 2;

function Macro0(player)
    if player.nCurrentMoonEnergy == 8000 then -- moon=80
        player:cast(34347);                   -- 悬象著明
    end;
    if player:buffExist(9909, 0) then         -- buff: 诛邪镇魔
        player:cast(3969);                    -- 光明相
    end;
    if player.nCurrentSunEnergy == 6000 then  -- sun=60
        player:cast(3974);                    -- 暗尘弥散
    end;
    if player.nCurrentSunEnergy == 6000 then  -- sun=60
        player:cast(22890);                   -- 诛邪镇魔
    end;
    player:cast(3967);                        -- 净世破魔击
    if player.nCurrentMoonEnergy == 2000 then -- moon=20
        player:cast(3979);                    -- 驱夜断愁
    end;
    player:cast(3963);                        -- 烈日斩
    if player.nCurrentSunEnergy > 9900 and player.nCurrentMoonEnergy == 8000 then
        -- sun>99&moon=80
        player.macroIdx = 1; -- 切换至 1 号宏
    end;
end;

function Macro1(player)
    player:cast(22890); -- 诛邪镇魔
    player:cast(3974);  -- 暗尘弥散
    player:cast(3969);  -- 光明相
    player:cast(34347); -- 悬象著明
    player:cast(3966);  -- 生死劫
    player:cast(3967);  -- 净世破魔击
    if player.nCurrentSunEnergy < 4100 and player.nCurrentMoonEnergy < 4100 then
        -- sun<41&moon<41
        player:cast(3979); -- 驱夜断愁
    end;
    player:cast(3963);     -- 烈日斩
    player:cast(3960);     -- 银月斩
    player:cast(3962);     -- 赤日轮
end;
