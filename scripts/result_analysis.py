import json

# 打开并加载data.json文件中的内容
with open("data.json", "r") as file:
    data = json.load(file)

# 按照id和level进行排序
sorted_data = sorted(data["data"], key=lambda x: (x["id"], x["level"]))

# 将排序后的结果保存回原json文件
with open("data.json", "w") as file:
    data["data"] = sorted_data  # 更新data字段为排序后的数据
    json.dump(data, file, indent=4, ensure_ascii=False)  # 以美化的形式写回文件
