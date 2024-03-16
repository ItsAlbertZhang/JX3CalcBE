import os, json

directory = ".vscode"
if not os.path.exists(directory):
    os.makedirs(directory)


if os.name == "nt":
    settings_add = {
        "cmake.generator": "Ninja",
    }
    launch_type = "cppvsdbg"
elif os.name == "posix":
    settings_add = {
        "clangd.arguments": ["--compile-commands-dir=build/obj/Debug"],
    }
    launch_type = "lldb"
else:
    raise Exception("Unsupported OS")

settings: dict = {
    "cmake.buildDirectory": "${workspaceFolder}/build/obj/${buildType}",
}
settings.update(settings_add)
with open(".vscode/settings.json", "w") as f:
    json.dump(settings, f, indent=4)

launch = {
    "configurations": [
        {
            "name": "Launch",
            "type": launch_type,
            "request": "launch",
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "cwd": "${command:cmake.launchTargetDirectory}",
            "console": "integratedTerminal",
        }
    ]
}
with open(".vscode/launch.json", "w") as f:
    json.dump(launch, f, indent=4)
