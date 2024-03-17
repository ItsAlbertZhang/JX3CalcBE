import json

time = -1
l = [19055, 26916]

with open("result.json", "r") as f:
    damage_list = json.load(f)["data"][0]
    # 遍历 list
    for i in range(len(damage_list)):
        # 打印第 i 个元素
        damage = damage_list[i]
        if damage["id"] in l and damage["time"] - time > 14 / 16:
            time = damage["time"]
            print(f"{damage['time']:.2f} {damage['name']}")
