MacroNum = 4; -- 自定义宏数量.

-- 自定义全局变量
FirstSwitch = true;
HighPing = false;

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
    -- 初始化全局变量
    FirstSwitch = true;
    HighPing = false;
end

function Macro0(player)
    if player.nCurrentMoonEnergy >= 10000 or player.nCurrentMoonEnergy <= 2000 then
        player:cast(3967);   -- 净世破魔击
    end
    player:cast(3979);       -- 驱夜断愁
    player:cast(3963);       -- 烈日斩
    if player.nCurrentSunEnergy >= 10000 and player.nCurrentMoonEnergy == 8000 then
        player.macroIdx = 1; -- 切换至 1 号宏
        if FirstSwitch then
            -- 起手时, 早一些释放暗尘弥散, 以避免第二轮卡隐身CD
            player.delayCustom = 300; -- 将下一次释放宏的时机设置至 300 tick 后
            -- 如果不进行设置, 那么下一次释放宏的时机会是 GCD 结束后 + 网络延迟 + 随机按键延迟.
            FirstSwitch = false;      -- 将起手标记置为 false
        end
    end
end

function Macro1(player)
    player:cast(3974); -- 暗尘弥散
    -- 如果隐身没好, 等隐身
    if not player:buffExist(25731, 1) and not player:buffExist(25721, 3) then
        player.delayCustom = player.delayBase + player.delayRand / 2;
        return;
    end
    if player:buffExist(25721, 3) and not player:buffExist(25716, 0) and player.nCurrentMoonEnergy >= 10000 then
        player:cast(3969); -- 光明相
    end
    player:cast(34347);    -- 悬象著明
    player:cast(3966);     -- 生死劫
    -- 避免生死劫卡CD
    if not player:buffExist(25721, 1) then
        if player.nCurrentMoonEnergy <= 4000 then
            player:cast(22890); -- 诛邪镇魔
        end
        player:cast(3967);      -- 净世破魔击
        if player.nCurrentMoonEnergy <= 4000 then
            player:cast(3979);  -- 驱夜断愁
        end
        player:cast(3960);      -- 银月斩
        player:cast(3963);      -- 烈日斩
    end
    if player:buffExist(25721, 3) and not player:buffExist(25716, 0) and player.nCurrentMoonEnergy == 0 then
        player.macroIdx = 2; -- 切换至 2 号宏
    end
end

function Macro2(player)
    if player.nCurrentSunEnergy >= 10000 and player.nCurrentMoonEnergy >= 10000 then
        if HighPing then
            player:cast(3963); -- 烈日斩
            if player.nCurrentSunEnergy < 14000 then
                player.delayCustom = player.delayBase + player.delayRand / 2;
                return;
            end
        end
        HighPing = false;
        player.macroIdx = 3; -- 切换至 3 号宏
        Macro3();            -- 执行一次 3 号宏
        return;              -- 直接返回, 不进行后续操作
        -- 在开头进行判断的原因是, 导致切换条件 (sun>=100&moon>=100) 的事件 (日月齐光·叁结束) 是发生在 GCD 中的, 而不是发生在宏内的. 因此, 无法在宏结束时进行判断.
    end
    if player:buffTimeLeftTick(25721, 3) < 5 * (1024 * 15 / 16 + player.delayBase + player.delayRand) and player:buffExist(9909, 0) then
        -- 齐光剩余时间小于 5 个 GCD, 且手上的诛邪还没打出去, 换高延迟打法
        HighPing = true;
    end
    if HighPing then        -- 高延迟打法
        -- 当 highPing 被标记时, 一定已经到了最后 5 个技能. 所以这里可以省去判定条件.
        player:cast(3963);  -- 烈日斩
        player:cast(22890); -- 诛邪镇魔
        player:cast(3960);  -- 银月斩
        player:cast(3967);  -- 净世破魔击
    end
    -- 正常循环
    if player.nCurrentMoonEnergy <= 4000 then
        player:cast(22890); -- 诛邪镇魔
    end
    player:cast(3967);      -- 净世破魔击
    if player.nCurrentMoonEnergy <= 4000 then
        player:cast(3979);  -- 驱夜断愁
    end
    player:cast(3963);      -- 烈日斩
    player:cast(3960);      -- 银月斩
end

function Macro3(player)
    if player.nCurrentMoonEnergy >= 6000 then
        player:cast(22890); -- 诛邪镇魔
    end
    if player.nCurrentSunEnergy >= 10000 then
        player:cast(3966); -- 生死劫
    end
    player:cast(3967);     -- 净世破魔击
    player:cast(3960);     -- 银月斩
    if player.nCurrentMoonEnergy >= 6000 or player.nCurrentSunEnergy >= 6000 then
        player:cast(3963); -- 烈日斩
    end
    if player.nCurrentMoonEnergy == 6000 and player.nCurrentSunEnergy == 4000 then
        player:cast(3962); -- 赤日轮
    end
    if player.nCurrentMoonEnergy >= 10000 and player.nCurrentSunEnergy < 10000 and not player:buffExist(25721, 0) then
        player.macroIdx = 0; -- 切换至 0 号宏
        -- 在结尾进行判断的原因时, 导致切换条件 (moon>=100 & sun<100 & nobuff:25721) 的事件 (银月斩的释放) 是发生在宏内的. 因此, 可以在宏结束时进行判断.
    end
end
