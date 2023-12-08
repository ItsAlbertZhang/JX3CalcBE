function InitAttr(player)
    -- 注: 1 tick = 1/1024 秒. 数字不大的情况下, 可以近似将其视作 1ms.
    
    player.delayBase = 45;                              -- 网络延迟, 单位为 tick
    player.delayRand = 20;                              -- 按键延迟, 单位为 tick

    player.chAttr.atHasteBase = 95;                     -- 急速
    player.chAttr.atSpunkBase = 6380;                   -- 元气

    player.chAttr.atPhysicsAttackPowerBase = 6;         -- 外功基础攻击
    player.chAttr.atSolarAttackPowerBase = 19308;       -- 阳性内功基础攻击
    player.chAttr.atLunarAttackPowerBase = 19308;       -- 阴性内功基础攻击
    player.chAttr.atNeutralAttackPowerBase = 12721;     -- 混元内功基础攻击
    player.chAttr.atPoisonAttackPowerBase = 12721;      -- 毒性内功基础攻击

    player.chAttr.atPhysicsCriticalStrike = 8325;       -- 外功会心等级
    player.chAttr.atSolarCriticalStrike = 15048;        -- 阳性内功会心等级
    player.chAttr.atLunarCriticalStrike = 15048;        -- 阴性内功会心等级
    player.chAttr.atNeutralCriticalStrike = 8715;       -- 混元内功会心等级
    player.chAttr.atPoisonCriticalStrike = 8715;        -- 毒性内功会心等级

    player.chAttr.atPhysicsCriticalDamagePowerBase = 0; -- 外功会心效果等级
    player.chAttr.atSolarCriticalDamagePowerBase = 188; -- 阳性内功会心效果等级
    player.chAttr.atLunarCriticalDamagePowerBase = 188; -- 阴性内功会心效果等级
    player.chAttr.atNeutralCriticalDamagePowerBase = 0; -- 混元内功会心效果等级
    player.chAttr.atPoisonCriticalDamagePowerBase = 0;  -- 毒性内功会心效果等级

    player.chAttr.atPhysicsOvercomeBase = 12;           -- 外功破防等级
    player.chAttr.atMagicOvercome = 20360;              -- 内功破防等级

    player.chAttr.atStrainBase = 21855;                 -- 无双等级
    player.chAttr.atSurplusValueBase = 9536;            -- 破招
    player.chAttr.atMeleeWeaponDamageBase = 240;        -- 武器伤害(基础, 数值上等于最小)
    player.chAttr.atMeleeWeaponDamageRand = 160;        -- 武器伤害(浮动, 数值上等于最大减去最小)

    player.macroNum = 3;                                -- 宏数量. 除非你确切地知道自己在干什么, 否则不要修改此项.
end

function Init(player, target)
    player:skillLearn(10242, 13);  -- 焚影圣诀
    player.dwKungfuID = 10242;
    player.publicCooldownID = 503; -- GCD
    player:skillActive(10242);     -- 激活心法加成

    player:skillLearn(3962, 33);   -- 赤日轮
    player:skillLearn(3963, 32);   -- 烈日斩
    player:skillLearn(3966, 1);    -- 生死劫
    player:skillLearn(3967, 32);   -- 净世破魔击
    player:skillLearn(3959, 24);   -- 幽月轮
    player:skillLearn(3960, 18);   -- 银月斩
    player:skillLearn(3969, 1);    -- 光明相
    player:skillLearn(3974, 1);    -- 暗尘弥散
    player:skillLearn(3979, 29);   -- 驱夜断愁

    player:skillLearn(5972, 1);    -- 腾焰飞芒
    player:skillLearn(18279, 1);   -- 净身明礼
    player:skillLearn(22888, 1);   -- 诛邪镇魔
    player:skillLearn(22890, 1);   -- 诛邪镇魔, 主动
    player:skillLearn(6717, 1);    -- 无明业火
    player:skillLearn(34383, 1);   -- 明光恒照
    player:skillLearn(34395, 1);   -- 日月同辉
    player:skillLearn(34372, 1);   -- 靡业报劫
    player:skillLearn(17567, 1);   -- 用晦而明
    player:skillLearn(25166, 1);   -- 净体不畏
    player:skillLearn(34378, 1);   -- 降灵尊
    player:skillLearn(34347, 1);   -- 悬象著明, 主动
    player:skillLearn(34370, 1);   -- 日月齐光

    player:skillActive(5972);
    player:skillActive(18279);
    player:skillActive(22888);
    -- player:skillActive(22890); -- 主动技能不需要激活
    player:skillActive(6717);
    player:skillActive(34383);
    player:skillActive(34395);
    player:skillActive(34372);
    player:skillActive(17567);
    player:skillActive(25166);
    player:skillActive(34378);
    -- player:skillActive(34347); -- 主动技能不需要激活
    player:skillActive(34370);

    player:skillrecipeAdd(1005, 1); -- 赤日轮, 会心提高4%
    player:skillrecipeAdd(999, 1);  -- 赤日轮, 伤害提高3%
    player:skillrecipeAdd(1000, 1); -- 赤日轮, 伤害提高4%
    player:skillrecipeAdd(1001, 1); -- 赤日轮, 伤害提高5%

    player:skillrecipeAdd(1011, 1); -- 烈日斩, 会心提高4%
    player:skillrecipeAdd(1008, 1); -- 烈日斩, 伤害提高4%
    player:skillrecipeAdd(1009, 1); -- 烈日斩, 伤害提高5%
    player:skillrecipeAdd(1013, 1); -- 烈日斩, 对原地静止的目标伤害提升10%

    player:skillrecipeAdd(1621, 1); -- 生死劫, 伤害提高3%
    player:skillrecipeAdd(1622, 1); -- 生死劫, 伤害提高4%
    player:skillrecipeAdd(1623, 1); -- 生死劫, 伤害提高5%

    player:skillrecipeAdd(1019, 1); -- 净世破魔击, 会心提高5%
    player:skillrecipeAdd(1015, 1); -- 净世破魔击, 伤害提高4%
    player:skillrecipeAdd(1016, 1); -- 净世破魔击, 伤害提高5%
    player:skillrecipeAdd(5206, 1); -- 焚影圣诀心法下净世破魔击·月命中后回复20点月魂

    player:skillrecipeAdd(989, 1);  -- 幽月轮, 会心提高4%
    player:skillrecipeAdd(990, 1);  -- 幽月轮, 会心提高5%
    player:skillrecipeAdd(984, 1);  -- 幽月轮, 伤害提高3%
    player:skillrecipeAdd(985, 1);  -- 幽月轮, 伤害提高4%

    player:skillrecipeAdd(992, 1);  -- 银月斩, 会心提高3%
    player:skillrecipeAdd(993, 1);  -- 银月斩, 会心提高4%
    player:skillrecipeAdd(994, 1);  -- 银月斩, 会心提高5%

    player:skillrecipeAdd(1029, 1); -- 光明相, 调息时间减少10秒
    player:skillrecipeAdd(1030, 1); -- 光明相, 调息时间减少10秒
    player:skillrecipeAdd(1031, 1); -- 光明相, 调息时间减少10秒

    player:skillrecipeAdd(1055, 1); -- 驱夜断愁, 会心提高4%
    player:skillrecipeAdd(1056, 1); -- 驱夜断愁, 会心提高5%
    player:skillrecipeAdd(1052, 1); -- 驱夜断愁, 伤害提高4%
    player:skillrecipeAdd(1053, 1); -- 驱夜断愁, 伤害提高5%

    InitAttr(player);

    target.nLevel = 124;
    target.chAttr.atLevel = 124;
    target.chAttr.atPhysicsShieldBase = 27550;
    target.chAttr.atMagicShield = 27550;
    target.fMaxLife64 = 1e+10;     -- 用于靡业报劫
    target.fCurrentLife64 = 1e+10; -- 用于驱夜断愁
end

function Ready(player)
    player:cast(3974);
    player:vCheckSunMoonPower();
end

FirstSwitch = true;

function Macro0(player)
    if player.nCurrentMoonEnergy >= 10000 or player.nCurrentMoonEnergy <= 2000 then
        player:cast(3967);           -- 净世破魔击
    end
    player:cast(3979);               -- 驱夜断愁
    player:cast(3963);               -- 烈日斩
    if player.nCurrentSunEnergy >= 10000 and player.nCurrentMoonEnergy == 8000 then
        player.macroIdx = 1;         -- 切换至 1 号宏
        if FirstSwitch then
            -- 起手时, 早一些释放暗尘弥散, 以避免第二轮卡隐身CD
            SetNextMacroActive(300); -- 将下一次释放宏的时机设置至 300 tick 后
            -- 如果不进行设置, 那么下一次释放宏的时机会是 GCD 结束后 + 网络延迟 + 随机按键延迟.
            FirstSwitch = false; -- 将起手标记置为 false
        end
    end
end

function Macro1(player)
    if player.nCurrentSunEnergy >= 10000 and player.nCurrentMoonEnergy >= 10000 then
        player.macroIdx = 2; -- 切换至 2 号宏
        Macro2(player); -- 执行 2 号宏
        return; -- 直接返回, 不进行后续操作
        -- 在开头进行判断的原因是, 导致切换条件 (sun>=100&moon>=100) 的事件 (日月齐光·叁结束) 是发生在 GCD 中的, 而不是发生在宏内的. 因此, 无法在宏结束时进行判断.
    end

    player:cast(3974); -- 暗尘弥散
    if player:GetBuff(25721, 3) and player.nCurrentMoonEnergy >= 10000 then
        local buff = player:GetBuff(25716, 0);
        if not buff then
            player:cast(3969); -- 光明相
        end
    end
    player:cast(34347);     -- 悬象著明
    player:cast(3966);      -- 生死劫
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
    if player.nCurrentMoonEnergy >= 10000 and player.nCurrentSunEnergy < 10000 then
        local buff = player:GetBuff(25721, 0);
        if not buff then
            player.macroIdx = 0; -- 切换至 0 号宏
            -- 在结尾进行判断的原因时, 导致切换条件 (moon>=100 & sun<100 & nobuff:25721) 的事件 (银月斩的释放) 是发生在宏内的. 因此, 可以在宏结束时进行判断.
        end
    end
end
