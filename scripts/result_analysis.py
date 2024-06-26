import json

skill_list = [
    "赤日轮",
    "幽月轮",
    "烈日斩",
    "银月斩",
    "净世破魔击·日",
    "净世破魔击·月",
    "生死劫·日",
    "生死劫·月",
    "悬象著明·日",
    "悬象著明·月",
    "驱夜断愁",
    "诛邪镇魔",
    "崇光斩恶",
]

skill_color = {
    "净世破魔击·日": "\033[34m",
    "净世破魔击·月": "\033[34m",
    "生死劫·日": "\033[35m",
    "生死劫·月": "\033[35m",
    "悬象著明·日": "\033[32m",
    "悬象著明·月": "\033[32m",
    "驱夜断愁": "\033[36m",
    "崇光斩恶": "\033[33m",
}


def isvalid(item):
    return (
        (item["name"] in skill_list) and (item["id"] != 19055) and (item["id"] != 4202)
    )


curr = -1
idx = 1

with open("data.json", "r", encoding="utf-8") as f:
    data = json.load(f)
    data = data["data"][0]
    for item in data:
        c = item["time"] - curr
        if item["id"] == 4482:
            c -= 1 / 16
        if c > 14 / 16 and isvalid(item):
            color = skill_color.get(item["name"], "")
            print(
                f'{idx}\t{item["time"]:.2f}\t{c:.2f}\t{c>1.1}\t{item["damageExcept"]}\t{color}{item["name"]}\033[0m'
            )
            curr += c
            idx += 1
    print(f"Total: {idx - 1}")
