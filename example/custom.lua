-- 自定义全局变量
FirstSwitch = true; -- 起手标记
HighPing = false;   -- 高延迟标记, 高延迟下, 有光明相起手无法使用 0 容错打法.

function Init()
    -- 每次战斗开始前, 重置全局变量
    FirstSwitch = true;
    HighPing = false;
end

MacroNum = 4; -- 自定义宏数量.

-- 宏0, 用于起手和进入大齐光的准备.
function Macro0(player)
    if player.nCurrentMoonEnergy >= 10000 or player.nCurrentMoonEnergy <= 2000 then
        player:cast(3967); -- /cast [moon>99&sun<21] 净世破魔击
    end
    player:cast(3979);     -- /cast 驱夜断愁
    player:cast(3963);     -- /cast 烈日斩
    if player.nCurrentSunEnergy >= 10000 and player.nCurrentMoonEnergy == 8000 then
        -- 切换至 1 号宏
        player.macroIdx = 1; -- /switch [sun>99&moon=80] 1
        if FirstSwitch then
            -- 起手时, 早一些释放暗尘弥散, 以避免第二轮卡隐身 CD
            player.delayCustom = 300; -- 将下一次释放宏的时机设置至 300 tick 后. 1024 tick 为 1 秒.
            -- 如果不设置 player.delayCustom, 那么下一次释放宏的时机会是 公共 CD 冷却完成后 + 基础延迟 + 随机延迟
            FirstSwitch = false;      -- 将起手标记置为 false
        end
    end
end

-- 宏1, 用于 齐光3 前半部分.
function Macro1(player)
    player:cast(3974); -- /cast 暗尘弥散
    -- 如果隐身没好, 等隐身
    if not player:buffExist(25731, 1) and not player:buffExist(25721, 3) then
        -- 在本宏的全部过程中, 要么有 降灵尊 buff (25731,1), 要么有 齐光3 buff (25721,3). 如果两者都没有, 说明隐身没有成功释放.
        return; -- 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
    end

    if player:buffExist(25721, 3) and not player:buffExist(25716, 0) and player.nCurrentMoonEnergy >= 10000 then
        player:cast(3969);        -- /cast [buff:齐光3 & nobuff:悬象(包括日和月) & moon>99] 光明相
    end
    player:cast(34347);           -- /cast 悬象著明
    if player:buffExist(25716, 0) then
        player:itemUse(1, 38789); -- /cast [buff:悬象(包括日和月)] 吹香雪(特效腰坠)
    end
    player:cast(3966);            -- /cast 生死劫
    -- 如果生死劫没好, 等生死劫
    if player:buffExist(25721, 1) then
        -- 生死劫会将齐光升至 2 阶. 如果此时仍有 齐光1 的 buff, 说明生死劫被卡 CD.
        return; -- 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
    end

    if player.nCurrentMoonEnergy <= 4000 then
        player:cast(22890); -- /cast [moon<41] 诛邪镇魔
    end
    player:cast(3967);      -- /cast 净世破魔击
    if player.nCurrentMoonEnergy <= 4000 then
        player:cast(3979);  -- /cast [moon<41] 驱夜断愁
    end
    player:cast(3960);      -- /cast 银月斩
    player:cast(3963);      -- /cast 烈日斩

    if player:buffExist(25721, 3) and not player:buffExist(25716, 0) and player.nCurrentMoonEnergy == 0 then
        -- 切换至2号宏
        player.macroIdx = 2; -- /switch [buff:齐光3 & nobuff:悬象(包括日和月) & moon=0] 2
    end
end

-- 宏2, 用于 齐光3 后半部分.
function Macro2(player)
    if player.nCurrentSunEnergy >= 10000 and player.nCurrentMoonEnergy >= 10000 then
        -- sun>=100&moon>=100, 说明 齐光3 结束返灵了. 走 齐光3 结束的流程.
        if HighPing then
            -- 处理高延迟情况.
            -- 高延迟下, 即使 齐光3 结束返灵, 该日斩也必须打出, 否则会在小齐光部分开头卡月斩 CD.
            player:cast(3963); -- /cast 烈日斩
            if player.nCurrentSunEnergy < 14000 then
                -- 日斩打出 + 齐光3 结束返还 100 能量, nCurrentSunEnergy < 14000 说明日斩未成功释放.
                return; -- 直接返回, 不进行后续操作. 由于公共 CD 没有被触发, 因此下一次释放宏的时机为 基础延迟 + 随机延迟 后.
            end
        end
        -- 如果没有返回, 说明日斩成功释放, 可以继续走 齐光3 结束的流程.
        HighPing = false;    -- 将高延迟标记重置为 false
        player.macroIdx = 3; -- 切换至 3 号宏
        Macro3();            -- 执行一次 3 号宏
        return;              -- 直接返回, 不进行后续操作
        -- 在宏开始时进行判断的原因是, 导致切换条件 (sun>=100&moon>=100) 的事件 (齐光3 结束返灵) 是发生在公共 CD 中的,
        -- 而不是发生在宏内由宏内的技能导致的. 因此, 必须在宏开始时进行判断, 而不能在宏结束时进行判断.
    end
    if player:buffTimeLeftTick(25721, 3) < 5 * (1024 * 15 / 16 + player.delayBase + player.delayRand) and player:buffExist(9909, 0) then
        -- 齐光剩余时间小于 5 个 GCD, 且手上的诛邪还没打出去, 换高延迟打法
        HighPing = true; -- 将高延迟标记置为 true
    end

    -- 高延迟打法
    if HighPing then
        -- 当 highPing 被标记时, 一定已经到了最后 5 个技能. 所以这里可以省去判定条件, 直接按顺序释放技能.
        player:cast(3963);  -- /cast 烈日斩
        player:cast(22890); -- /cast 诛邪镇魔
        player:cast(3960);  -- /cast 银月斩
        player:cast(3967);  -- /cast 净世破魔击
    end

    -- 正常打法
    if player.nCurrentMoonEnergy <= 4000 then
        player:cast(22890); -- /cast [moon<41] 诛邪镇魔
    end
    player:cast(3967);      -- /cast 净世破魔击
    if player.nCurrentMoonEnergy <= 4000 then
        player:cast(3979);  -- /cast [moon<41] 驱夜断愁
    end
    player:cast(3963);      -- /cast 烈日斩
    player:cast(3960);      -- /cast 银月斩
end

-- 宏3, 用于 齐光3 结束后直至 齐光2 的结束阶段.
function Macro3(player)
    if player.nCurrentMoonEnergy >= 6000 then
        player:cast(22890); -- /cast [moon>59] 诛邪镇魔
    end
    if player.nCurrentSunEnergy >= 10000 then
        player:cast(3966); -- /cast [sun>99] 生死劫
    end
    player:cast(3967);     -- /cast 净世破魔击
    player:cast(3960);     -- /cast 银月斩
    if player.nCurrentMoonEnergy >= 6000 or player.nCurrentSunEnergy >= 6000 then
        player:cast(3963); -- /cast [moon>59&sun>59] 烈日斩
    end
    if player.nCurrentMoonEnergy == 6000 and player.nCurrentSunEnergy == 4000 then
        player:cast(3962); -- /cast [moon=60&sun=40] 赤日轮
    end
    if player.nCurrentMoonEnergy >= 10000 and player.nCurrentSunEnergy < 10000 and not player:buffExist(25721, 0) then
        -- 切换至 0 号宏
        player.macroIdx = 0; -- /switch [moon>99&sun<100&nobuff:齐光] 0
        -- 在结尾进行判断的原因是, 导致切换条件 (moon>99&sun<100&nobuff:齐光) 的事件 (银月斩的释放) 是发生在宏内的. 因此, 可以在宏结束时进行判断.
    end
end
