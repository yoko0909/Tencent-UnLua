## 1. Sentry 崩溃诊断接口公开化

- [x] 1.1 确认当前只有一个 `SentryCrashLogAppender.h`，路径为 `Plugins/Sentry/Source/Sentry/Private/SentryCrashLogAppender.h`。
- [x] 1.2 将 `SentryCrashLogAppender.h` 从 Sentry `Private` 目录迁移到 `Plugins/Sentry/Source/Sentry/Public/SentryCrashLogAppender.h`。
- [x] 1.3 在 public header 中声明 `FSentryCrashInfoInitDelegate` 和 `FSentryCrashInfoAppendDelegate`。
- [x] 1.4 在 `FSentryCrashLogAppender` 中声明初始化监听注册/反注册接口。
- [x] 1.5 在 `FSentryCrashLogAppender` 中声明崩溃信息追加监听注册/反注册接口。
- [x] 1.6 保留 `InitExtLogFullPath()` 和 `AppendCrashInfo(const TCHAR* PlatformName, int32 Signal)` 作为现有 hook 的稳定入口。
- [x] 1.7 将 `SentryCrashLogAppender.cpp` 改为只保存 delegate、执行注册/反注册、广播初始化通知和广播追加通知。
- [x] 1.8 确认 `SentryCrashLogAppender.cpp` 不再包含 UnLua、路径处理、文件写入或 Lua 栈生成逻辑。
- [x] 1.9 提交 Sentry public callback 接口改动。

## 2. 移除 Sentry 对 UnLua 的直接依赖

- [x] 2.1 确认 `Plugins/Sentry/Source/Sentry/Sentry.Build.cs` 当前存在 `"UnLua"` 或 `"Unlua"` 依赖。
- [x] 2.2 从 Sentry 的 `PrivateDependencyModuleNames` 中移除 UnLua 依赖。
- [x] 2.3 搜索 `Plugins/Sentry/Source/Sentry`，确认不再出现 `UnLua`、`IUnLuaModule`、`UnLuaDebugBase` 或 `GetLuaCallStack`。
- [x] 2.4 确认 Android hook 仍然调用 `FSentryCrashLogAppender::InitExtLogFullPath()` 和 `FSentryCrashLogAppender::AppendCrashInfo(...)`。
- [x] 2.5 确认 iOS hook 仍然调用 `FSentryCrashLogAppender::InitExtLogFullPath()` 和 `FSentryCrashLogAppender::AppendCrashInfo(...)`。
- [x] 2.6 提交 Sentry 依赖清理。

## 3. 准备 MiscInterface 桥接模块状态

- [x] 3.1 确认 `Plugins/MiscInterface/Source/MiscInterface/MiscInterface.Build.cs` 当前依赖 UnLua 但不依赖 Sentry。
- [x] 3.2 在 MiscInterface 的 `PrivateDependencyModuleNames` 中加入 `"Sentry"`。
- [x] 3.3 将 MiscInterface 中 UnLua 模块名统一为实际模块名 `"UnLua"`。
- [x] 3.4 在 `FMiscInterfaceModule` 中添加 Sentry 初始化监听的 `FDelegateHandle` 成员。
- [x] 3.5 在 `FMiscInterfaceModule` 中添加 Sentry 崩溃追加监听的 `FDelegateHandle` 成员。
- [x] 3.6 在 `FMiscInterfaceModule` 中添加 `LuaCallStackLogPath` 成员。
- [x] 3.7 在 `FMiscInterfaceModule` 中声明注册、反注册、初始化 Lua 栈日志路径、追加 Lua 崩溃信息的私有函数。
- [x] 3.8 确认 MiscInterface public header 不 include Sentry 或 UnLua 头文件。
- [x] 3.9 提交 MiscInterface 依赖和模块状态准备。

## 4. 实现 MiscInterface 到 Sentry/UnLua 的桥接

- [x] 4.1 在 `MiscInterface.cpp` 中 include `SentryCrashLogAppender.h`。
- [x] 4.2 在 `MiscInterface.cpp` 中 include `UnLuaModule.h` 和 `UnLuaDebugBase.h`。
- [x] 4.3 在 `StartupModule()` 中调用注册函数，注册 Sentry crash diagnostic listeners。
- [x] 4.4 在 `ShutdownModule()` 中调用反注册函数，移除 Sentry crash diagnostic listeners。
- [x] 4.5 实现初始化监听：计算 `LuaCallStack.log` 的项目持久下载目录路径，并转换为外部 App 可写绝对路径。
- [x] 4.6 初始化监听中触碰 `LuaCallStack.log` 文件，记录成功或失败日志。
- [x] 4.7 实现追加监听：如果日志路径为空，先执行路径初始化。
- [x] 4.8 追加监听中通过 `IUnLuaModule::Get().GetEnv()` 获取 UnLua 环境。
- [x] 4.9 追加监听中通过 `EnvPtr->GetMainState()` 获取 `lua_State*`。
- [x] 4.10 追加监听中调用 `UnLua::GetLuaCallStack(L)` 生成 Lua 调用栈。
- [x] 4.11 追加监听中打开 `LuaCallStack.log`，写入平台名、信号值和 Lua 调用栈。
- [x] 4.12 对无 UnLua Env、无 Lua State、空栈、文件打开失败分别写入低层诊断日志或 UE 日志。
- [x] 4.13 确认 Sentry 和 UnLua include 只出现在 MiscInterface private `.cpp` 中。
- [x] 4.14 提交 MiscInterface 桥接实现。

## 5. 标注 crash-time 安全边界

- [x] 5.1 在 Sentry public callback API 附近添加注释，说明监听器可能从 native crash handler 中同步执行。
- [x] 5.2 在 MiscInterface Lua 栈追加监听函数附近添加注释，说明访问 UnLua 和写文件不是 async-signal-safe。
- [x] 5.3 确认注释明确表达该路径仅作为诊断验证用途。
- [x] 5.4 提交 crash-time 风险说明。

## 6. 静态验证与编译验证

- [ ] 6.1 搜索 Sentry Build.cs，确认 Sentry 不再依赖 UnLua。
- [ ] 6.2 搜索 MiscInterface Build.cs，确认 MiscInterface 同时依赖 Sentry 和 UnLua。
- [ ] 6.3 搜索 Sentry 源码，确认不存在 UnLua 头文件、UnLua 模块 API 或 Lua 调用栈 API 引用。
- [ ] 6.4 搜索 MiscInterface 源码，确认 Lua 栈采集逻辑迁移到 MiscInterface。
- [ ] 6.5 编译 Win64 Editor 目标，确认新 public header 和模块依赖可通过。
- [ ] 6.6 编译 Android 目标或项目现有 Android 打包命令，确认 Android Sentry hook 可通过。
- [ ] 6.7 编译 iOS 目标或项目现有 iOS 打包命令，确认 iOS Sentry hook 可通过。
- [ ] 6.8 如果编译暴露模块名大小写或 include 路径问题，修正后重新运行对应编译。
- [ ] 6.9 仅在有实际编译修正时提交编译修复。

## 7. 运行时验证

- [ ] 7.1 启动 Editor 或打包应用，确认 MiscInterface 模块注册 Sentry 崩溃诊断监听。
- [ ] 7.2 确认启动日志中出现 `LuaCallStack.log` 路径初始化成功或明确失败原因。
- [ ] 7.3 在 Android 设备触发现有 native crash 验证路径。
- [ ] 7.4 确认 Android 日志中先进入 Sentry crash hook，再进入 MiscInterface Lua 栈桥接逻辑。
- [ ] 7.5 确认 Android 崩溃后 Sentry 仍然能生成或发送 crash event。
- [ ] 7.6 在 iOS 真机且不挂 Xcode debugger 的情况下触发现有 native crash 验证路径。
- [ ] 7.7 确认 iOS 日志中先进入 SentryCrash callback 或 signal fallback，再进入 MiscInterface Lua 栈桥接逻辑。
- [ ] 7.8 确认 iOS 崩溃后 Sentry 仍然能在下次启动识别或发送 crash event。
- [ ] 7.9 检查应用沙盒中的 `LuaCallStack.log`，确认包含平台名、信号值和 `Lua stack :` 内容。
- [ ] 7.10 记录 Android/iOS 运行时证据，并只勾选已有证据支持的任务。

## 8. OpenSpec 文档更新

- [ ] 8.1 在 Android crash signal hook 变更设计中记录：Lua 栈采集由 MiscInterface 作为间接插件承担。
- [ ] 8.2 在 iOS crash diagnostic hook 变更设计中记录：Lua 栈采集由 MiscInterface 作为间接插件承担。
- [ ] 8.3 在相关设计文档中补充同步 Lua 栈采集的 crash-time 风险。
- [ ] 8.4 运行 `openspec.cmd validate decouple-sentry-unlua-crash-stack --strict`。
- [ ] 8.5 运行已有 Android/iOS crash hook 变更的 OpenSpec strict validate。
- [ ] 8.6 提交 OpenSpec 设计与任务文档更新。
