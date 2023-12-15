function AttrInit(player)
    -- 注: 1 tick = 1/1024 秒. 数字不大的情况下, 可以近似将其视作 1ms.
    player.delayBase = 45; -- 网络延迟, 单位为 tick
    player.delayRand = 20; -- 按键延迟, 单位为 tick

    -- 以下两种方法仅应当使用其中一种.

    -- 方法 1: 从 JX3BOX 导入属性
    player:attrImportFromJX3BOX(787136);

    -- 方法 2: 手动输入属性 (默认被注释)

    -- player.chAttr.atHasteBase = 95;                     -- 急速
    -- player.chAttr.atSpunkBase = 6380;                   -- 元气

    -- player.chAttr.atPhysicsAttackPowerBase = 6;         -- 外功基础攻击
    -- player.chAttr.atSolarAttackPowerBase = 19308;       -- 阳性内功基础攻击
    -- player.chAttr.atLunarAttackPowerBase = 19308;       -- 阴性内功基础攻击
    -- player.chAttr.atNeutralAttackPowerBase = 12721;     -- 混元内功基础攻击
    -- player.chAttr.atPoisonAttackPowerBase = 12721;      -- 毒性内功基础攻击

    -- player.chAttr.atPhysicsCriticalStrike = 8325;       -- 外功会心等级
    -- player.chAttr.atSolarCriticalStrike = 15048;        -- 阳性内功会心等级
    -- player.chAttr.atLunarCriticalStrike = 15048;        -- 阴性内功会心等级
    -- player.chAttr.atNeutralCriticalStrike = 8715;       -- 混元内功会心等级
    -- player.chAttr.atPoisonCriticalStrike = 8715;        -- 毒性内功会心等级

    -- player.chAttr.atPhysicsCriticalDamagePowerBase = 0; -- 外功会心效果等级
    -- player.chAttr.atSolarCriticalDamagePowerBase = 188; -- 阳性内功会心效果等级
    -- player.chAttr.atLunarCriticalDamagePowerBase = 188; -- 阴性内功会心效果等级
    -- player.chAttr.atNeutralCriticalDamagePowerBase = 0; -- 混元内功会心效果等级
    -- player.chAttr.atPoisonCriticalDamagePowerBase = 0;  -- 毒性内功会心效果等级

    -- player.chAttr.atPhysicsOvercomeBase = 12;           -- 外功破防等级
    -- player.chAttr.atSolarOvercomeBase = 20360;          -- 阳性内功破防等级
    -- player.chAttr.atLunarOvercomeBase = 20360;          -- 阴性内功破防等级
    -- player.chAttr.atNeutralOvercomeBase = 13390;        -- 混元内功破防等级
    -- player.chAttr.atPoisonOvercomeBase = 13390;         -- 毒性内功破防等级

    -- player.chAttr.atStrainBase = 21855;                 -- 无双等级
    -- player.chAttr.atSurplusValueBase = 9536;            -- 破招
    -- player.chAttr.atMeleeWeaponDamageBase = 240;        -- 武器伤害(基础, 数值上等于最小)
    -- player.chAttr.atMeleeWeaponDamageRand = 160;        -- 武器伤害(浮动, 数值上等于最大减去最小)
end

FightTime = 300;
FightCount = 10000;
UseCustomMacro = false; -- 是否使用自定义宏. 如果为 false, 则以下的所有设置均不会生效, 且会使用内置宏以提高计算速度.
-- 除非你确切地知道自己在干什么, 否则不要修改此项及以下的内容.

MacroNum = 3; -- 自定义宏数量.

FirstSwitch = true;
function MacroPrepare(player)
    -- print("here")
    player:cast(3974);
    if player.nMoonPowerValue == 0 and player.nSunPowerValue == 0 then
        if player.nCurrentMoonEnergy >= 10000 then
            player.nMoonPowerValue = 1;
        elseif player.nCurrentSunEnergy >= 10000 then
            player.nSunPowerValue = 1;
        end
    end
    FirstSwitch = true;
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
    if player.nCurrentSunEnergy >= 10000 and player.nCurrentMoonEnergy >= 10000 then
        player.macroIdx = 2; -- 切换至 2 号宏
        Macro2(player);      -- 执行 2 号宏
        return;              -- 直接返回, 不进行后续操作
        -- 在开头进行判断的原因是, 导致切换条件 (sun>=100&moon>=100) 的事件 (日月齐光·叁结束) 是发生在 GCD 中的, 而不是发生在宏内的. 因此, 无法在宏结束时进行判断.
    end

    player:cast(3974);     -- 暗尘弥散
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
end

function Macro2(player)
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
