# 本文件为与后端进行 http 通信的 api 示例文件
# 注意: http 通信时 json 数据中的中文字符串要求使用 utf-8 编码

# 运行本文件前请确保已安装 requests 库, 可通过 pip install requests 安装
import requests  # 导入 requests 库, 用于进行 http 请求. 其他语言请自行解决 http 请求问题
import inspect, json, time, urllib3


# 调试环境为了方便, 未开启所有 HTTPS 的 SSL 验证, 并禁用了 SSL 警告.
# 生产环境请自行确保 HTTPS 的 SSL 安全.
urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)
BOOL_SSL_VERIFY = False  # 是否开启 SSL 验证

ip = "https://jx3calc.com"  # 对于本地部署的服务器, 可修改为 "http://localhost"
port = 12897  # 服务器的默认端口号


# 检查服务器状态
def status():
    url = f"{ip}:{port}/status"
    response = requests.get(url, verify=BOOL_SSL_VERIFY)
    return response.json()
    # 返回值示例:
    res = {
        "status": 0,  # 0 为正常, 其他值为异常
        "data": {
            "custom": True,  # 是否允许自定义战斗
            "isExp": False,  # 是否为测试服
            "userinput": {
                "maxDelayKeyboard": 1024,  # 允许的最大按键延迟, 最小为 0
                "maxDelayNetwork": 1024,  # 允许的最大网络延迟, 最小为 0
                "maxFightCount": 1073741824,  # 允许的最大战斗次数, 最小为 0
                "maxFightTime": 1073741824,  # 允许的最大战斗时间, 最小为 0
            },
            "version": "v1.2.1.080801",  # 服务器版本号
        },
    }


# Step 1: 创建任务
def create():
    url = f"{ip}:{port}/create"

    # 有两种方法构造属性数据.
    # 方法一: 通过 jx3box 配装器 api 获取属性数据
    pzid = 1144745  # jx3box 的配装 id
    jx3box_url = f"http://cms.jx3box.com/api/cms/app/pz/{pzid}"
    jx3box_res = requests.get(jx3box_url).json()  # 获取配装数据
    attribute_data = jx3box_res["data"]["data"]
    print(f"在第 {inspect.currentframe().f_lineno} 行, {"attribute_data"} 是:\n{json.dumps(attribute_data, ensure_ascii=False)}\n")  # type: ignore
    # 配装数据中 data 字段下的 data 字段, 即可直接用于本计算器的输入

    # 方法二: 手动构造数据
    # 1. 注意, 该方法并不要求填写每个字段. 对于未填写的字段, 计算器会以 0 作为默认值.
    # 2. 计算器会读取许多字段. 然而, 并非所有读取的字段都会在计算中被使用.
    #    因此, 只填写部分字段是可行的, 并且大部分情况下不会影响计算结果.
    # 3. 如果无法获取所有字段, 对于大部分情况, 建议将字段区分为"有效"和"无效"两类.
    #    随后, 正常填写有效字段即可. 无效字段可以不做填写, 由计算器以默认值 0 填充.
    #    例如, 对于焚影圣诀心法, 可以近似认为含 "元气", "阴性" 与 "阳性" 的进攻属性为有效字段, 其他字段为无效字段.
    #  * 注意, "外功" 对于焚影圣诀是无效字段. 如果近似认为其和内功相等, 会导致计算结果不准确 (武器伤害偏高).

    # 以下是计算器会读取的所有字段.
    # 注意! 所有的字段都为整型.
    attribute_data = {
        # 心法属性
        "Vitality": 32888,  # 体质
        "Strength": 41,  # 力道
        "Agility": 41,  # 身法
        "Spirit": 41,  # 根骨
        "Spunk": 6365,  # 元气
        # 基础攻击. 注意, 不是最终攻击.
        "PhysicsAttackPowerBase": 555,  # 外功基础攻击
        "SolarAttackPowerBase": 42905,  # 阳性内功基础攻击
        "LunarAttackPowerBase": 42905,  # 阴性内功基础攻击
        "NeutralAttackPowerBase": 27983,  # 混元内功基础攻击
        "PoisonAttackPowerBase": 27983,  # 毒性内功基础攻击
        "TherapyPowerBase": 0,  # 基础治疗
        # 会心等级. 注意, 不是会心率.
        # 对于 120 级角色而言, 会心等级等于 会心率 * 78622.5 (请自行取整)
        # 例如, 100% 会心率对应 78622.5 的会心等级, 取整为 78622 或 78623.
        "PhysicsCriticalStrike": 23932,  # 外功会心等级
        "SolarCriticalStrike": 31997,  # 阳性内功会心等级
        "LunarCriticalStrike": 31997,  # 阴性内功会心等级
        "NeutralCriticalStrike": 23932,  # 混元内功会心等级
        "PoisonCriticalStrike": 23932,  # 毒性内功会心等级
        # 会心效果等级. 注意, 不是会效率.
        # 对于 120 级角色而言, 会心效果等级等于 (会效率 - 175%) * 27513.75 (请自行取整)
        # 例如, 200% 会心效果对应 6878.4375 的会心效果等级, 取整为 6878.
        "PhysicsCriticalDamagePower": 5703,  # 外功会心效果等级
        "SolarCriticalDamagePower": 8461,  # 阳性内功会心效果等级
        "LunarCriticalDamagePower": 8461,  # 阴性内功会心效果等级
        "NeutralCriticalDamagePower": 8043,  # 混元内功会心效果等级
        "PoisonCriticalDamagePower": 8043,  # 毒性内功会心效果等级
        # 基础破防等级. 注意, 不是破防率.
        # 对于 120 级角色而言, 破防等级等于 破防率 * 78622.5 (请自行取整)
        # 例如, 100% 破防率对应 78622.5 的破防等级, 取整为 78622 或 78623.
        "PhysicsOvercomeBase": 1230,  # 外功基础破防等级
        "SolarOvercomeBase": 32876,  # 阳性内功基础破防等级
        "LunarOvercomeBase": 32876,  # 阴性内功基础破防等级
        "NeutralOvercomeBase": 21172,  # 混元内功基础破防等级
        "PoisonOvercomeBase": 21172,  # 毒性内功基础破防等级
        # 其他进攻属性
        "SurplusValue": 3838,  # 破招
        "Strain": 41977,  # 无双等级. 对于 120 级角色而言, 无双等级等于 无双率 * 75809.25 (请自行取整)
        "Haste": 0,  # 加速等级. 对于 120 级角色而言, 加速等级等于 加速率 * 96483.75 (请自行取整)
        # 武器
        "MeleeWeaponDamage": 2419,  # 武器伤害(最低)
        "MeleeWeaponDamageRand": 1613,  # 武器伤害(波动). 在数值上等于最高减去最低
        # 防御等级
        "PhysicsShieldBase": 3185,  # 外功基础防御等级
        "SolarShieldBase": 4825,  # 阳性内功基础防御等级
        "LunarShieldBase": 4825,  # 阴性内功基础防御等级
        "NeutralShieldBase": 4825,  # 混元内功基础防御等级
        "PoisonShieldBase": 4825,  # 毒性内功基础防御等级
        # 其他防御属性
        "Dodge": 0,  # 闪避等级
        "Parry": 0,  # 招架等级
        "ParryValue": 0,  # 拆招值
        "Toughness": 0,  # 御劲等级
        "DecriticalDamage": 0,  # 化劲等级
    }
    print(f"在第 {inspect.currentframe().f_lineno} 行, {"attribute_data"} 是:\n{json.dumps(attribute_data, ensure_ascii=False)}\n")  # type: ignore

    # 一个简化版本(仅包括"有效"字段)的属性数据示例:
    attribute_data = {
        "Spunk": 6365,  # 元气
        "SolarAttackPowerBase": 42905,  # 阳性内功基础攻击
        "LunarAttackPowerBase": 42905,  # 阴性内功基础攻击
        "SolarCriticalStrike": 31997,  # 阳性内功会心等级
        "LunarCriticalStrike": 31997,  # 阴性内功会心等级
        "SolarCriticalDamagePower": 8461,  # 阳性内功会心效果等级
        "LunarCriticalDamagePower": 8461,  # 阴性内功会心效果等级
        "SolarOvercomeBase": 32876,  # 阳性内功基础破防等级
        "LunarOvercomeBase": 32876,  # 阴性内功基础破防等级
        "SurplusValue": 3838,  # 破招
        "Strain": 41977,  # 无双等级
        "Haste": 0,  # 加速等级
        "MeleeWeaponDamage": 2419,  # 武器伤害(最低)
        "MeleeWeaponDamageRand": 1613,  # 武器伤害(波动)
    }
    print(f"在第 {inspect.currentframe().f_lineno} 行, {"attribute_data"} 是:\n{json.dumps(attribute_data, ensure_ascii=False)}\n")  # type: ignore

    body = {
        "player": "焚影圣诀",
        "delayNetwork": 45,  # 网络延迟, 建议默认为 45, 不得大于 status 返回值中的 maxDelayNetwork
        "delayKeyboard": 20,  # 按键延迟, 建议默认为 20 , 不得大于 status 返回值中的 maxDelayKeyboard
        "fightTime": 300,  # 战斗时间, 建议默认为 300, 不得大于 status 返回值中的 maxFightTime
        "fightCount": 100,  # 战斗次数, 不得大于 status 返回值中的 maxFightCount
        "attribute": {
            "method": "从数据导入",  # 不建议改动
            "data": attribute_data,  # 属性数据
        },
        # "effects" 为效果字段, 用于添加配装效果/增益效果.
        # 如果不需要某一字段, 从 effects 中删除该字段即可.
        # 更多效果可以查看源码, 包括小吃小药/阵眼/团队增益等效果在此处未列出.
        "effects": {
            "套装·技能": True,  # Any, 雾海寻龙赛季 PVE 门派套装 2 件套效果 / 万灵当歌 4 件套
            # "套装·特效": 114514,  # Any, 雾海寻龙赛季 PVE 门派套装 4 件套效果 / 万灵当歌 2 件套
            "大附魔·腰": "example string",  # Any
            # 注意, Any 类型的字段只要存在, 不管其值为何 (哪怕是 False 或 0), 都会被视为激活.
            # 因此, 如果不需要某一效果, 请从 effects 中删除该字段.
            "大附魔·腕": "雾海寻龙",  # str["雾海寻龙"|"万灵当歌"]
            "大附魔·鞋": "雾海寻龙",  # str["雾海寻龙"|"万灵当歌"]
            "腰坠·特效": "梧桐影",  # str["梧桐影"|"吹香雪"]
            # 梧桐影: 雾海寻龙赛季元气特效腰坠
            # 吹香雪: 万灵当歌赛季元气特效腰坠
            "武器·特效": "无尽沙海",  # str["血月"|"封霜曲刃·忆"|"无尽沙海"|"冰焰玉"]
            # 血月: 焚影圣诀心法 120 级大橙武
            # 封霜曲刃·忆: 焚影圣诀心法 120 级小橙武
            # 无尽沙海: 焚影圣诀心法雾海寻龙赛季水特效武器
            # 冰焰玉: 焚影圣诀心法万灵当歌赛季水特效武器
            "家园·酿造": "女儿红·旬又三",  # str["状元红"|"状元红·旬又三"|"女儿红"|"女儿红·旬又三"]
            # 注: 部分心法(如焚影圣诀)会采用加速酒上一段加速的方式, 因此建议默认为所有玩家开启 "女儿红·旬又三" 增益.
        },
        "fight": {"method": "使用内置循环", "data": 0},  # 不建议改动
        "talents": [  # 奇穴, 不建议改动
            5972,
            18279,
            22888,
            6717,
            34383,
            34395,
            34372,
            17567,
            25166,
            34378,
            34347,
            37337,
        ],
    }
    print(f"在第 {inspect.currentframe().f_lineno} 行, {"body"} 是:\n{json.dumps(body, ensure_ascii=False)}\n")  # type: ignore

    response = requests.post(url, json=body, verify=BOOL_SSL_VERIFY)
    return response.json()
    # 返回值示例:
    res = {
        "status": 0,  # 0 为正常, 其他值为异常
        "data": "UAXkrE",  # 任务 ID, 为随机六位字符串, 用于后续查询
    }


# Step 2: 查询任务
def query_dps(taskid: str):
    """查询 dps."""
    url = f"{ip}:{port}/query/{taskid}/dps"
    response = requests.get(url, verify=BOOL_SSL_VERIFY)
    return response.json()
    # 返回值示例:
    res = {
        "status": 0,  # 0 为正常, 其他值为异常
        "data": {
            "avg": 1095558,  # 平均 dps
            "complete": True,  # 是否已计算完成. 若还未完成, 则为 False
            "current": 100,  # 当前已计算任务数量. 当小于 create 时的 fightCount 时, complete 为 False
            "total": 100,  # 总任务数量, 其应当等于 create 时的 fightCount
            "speed": 0,  # 当前每秒计算速度
            "max": 1102920,  # 最高一次模拟的 DPS
            "min": 1085521,  # 最低一次模拟的 DPS
            "md": 7362,  # 最大误差
            "sd": 3618,  # 标准差
            "ci99": 931,  # 99% 置信区间, 即有 99% 的把握认为 DPS 位于 avg ± ci99 区间内
            "list": [  # 每次模拟的 DPS 列表
                1097098,
                1090826,
                1088123,
                1092886,
                1095950,
                1099204,
                1087010,
                1097684,
                1097438,
                1097978,
                1093554,
                1096900,
                1097207,
                1098250,
                1096549,
                1096847,
                1098099,
                1098339,
                1095276,
                1096594,
                1090587,
                1100079,
                1090619,
                1095253,
                1100961,
                1098568,
                1098898,
                1097671,
                1091388,
                1097065,
                1094401,
                1093172,
                1096289,
                1096626,
                1102890,
                1097509,
                1095512,
                1090137,
                1095568,
                1098566,
                1095641,
                1094166,
                1086939,
                1092727,
                1097028,
                1098943,
                1093964,
                1095573,
                1096558,
                1101522,
                1097860,
                1095503,
                1094094,
                1100034,
                1091660,
                1100100,
                1092351,
                1094861,
                1098324,
                1095018,
                1098516,
                1095191,
                1091989,
                1098474,
                1099140,
                1085521,
                1098952,
                1100052,
                1089672,
                1087976,
                1095752,
                1096993,
                1097129,
                1099348,
                1096266,
                1095355,
                1099678,
                1095760,
                1093856,
                1098047,
                1096084,
                1097396,
                1090949,
                1098803,
                1095687,
                1097333,
                1095950,
                1098573,
                1097920,
                1097281,
                1089461,
                1091222,
                1092503,
                1096723,
                1102920,
                1094926,
                1089968,
                1088973,
                1094897,
                1088145,
            ],
        },
    }


def query_damage_list(taskid: str):
    """查询伤害列表, 可用于生成 时间 - dps 图表."""
    url = f"{ip}:{port}/query/{taskid}/damage-list"
    response = requests.get(url, verify=BOOL_SSL_VERIFY)
    return response.json()


def query_damage_analysis(taskid: str):
    """查询伤害分析, 可用于生成伤害占比图表."""
    url = f"{ip}:{port}/query/{taskid}/damage-analysis"
    response = requests.get(url, verify=BOOL_SSL_VERIFY)
    return response.json()


if __name__ == "__main__":
    print(f"在第 {inspect.currentframe().f_lineno} 行, {"status"} 是:\n{status()}\n")  # type: ignore
    res = create()
    print(f"在第 {inspect.currentframe().f_lineno} 行, {"create result"} 是:\n{res}\n")  # type: ignore
    taskid: str = res["data"]
    # 等待服务器计算.
    time.sleep(10)
    # 注意, 在任务完成 60 秒后, 服务器会自动删除任务数据.
    print(f"在第 {inspect.currentframe().f_lineno} 行, {"query_dps"} 是:\n{query_dps(taskid)}\n")  # type: ignore
