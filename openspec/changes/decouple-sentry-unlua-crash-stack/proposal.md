## 为什么

当前 Sentry 插件中的崩溃诊断路径直接包含 UnLua 头文件、获取 UnLua 环境并调用 `UnLua::GetLuaCallStack` 写入 Lua 堆栈。这让 Sentry 插件依赖 UnLua 插件，依赖方向不清晰，也让通用崩溃采集插件承载了项目侧 Lua 诊断逻辑。

需要将依赖改为：

```text
Sentry <--- MiscInterface ---> UnLua
```

Sentry 只负责暴露崩溃诊断通知点；MiscInterface 作为项目侧桥接插件，同时依赖 Sentry 和 UnLua，监听通知并完成 Lua 状态机访问与 Lua 堆栈写入。

## 改动内容

- 将 `FSentryCrashLogAppender` 改为 Sentry 侧公开的崩溃诊断通知接口。
- 从 Sentry 模块移除对 UnLua 的直接依赖。
- 在 MiscInterface 模块中注册 Sentry 崩溃诊断监听器。
- MiscInterface 监听到崩溃诊断请求后，获取 UnLua 环境、Lua 状态机，并写入 `LuaCallStack.log`。
- 保持 Android/iOS 现有崩溃 hook 调用 `FSentryCrashLogAppender::InitExtLogFullPath()` 和 `FSentryCrashLogAppender::AppendCrashInfo(...)` 的入口形式。
- 明确记录：同步崩溃当场访问 UnLua 和写文件仍然不是 crash-time safe，只作为诊断验证路径。

## 能力

### 新增能力

- `sentry-unlua-crash-stack-bridge`：定义 Sentry 与 UnLua 的间接崩溃 Lua 堆栈桥接机制，由 MiscInterface 负责项目侧 Lua 堆栈采集。

### 修改能力

- Android/iOS 崩溃诊断 hook 的 Lua 堆栈采集归属从 Sentry 插件迁移到 MiscInterface 插件。

## 影响

- 影响 `Plugins/Sentry/Source/Sentry` 中的崩溃诊断 appender 与模块依赖。
- 影响 `Plugins/MiscInterface/Source/MiscInterface` 的模块依赖、启动注册和崩溃诊断监听。
- 不修改 Sentry 第三方库、AAR、Framework、Sentry envelope 或上传行为。
- 不新增 Blueprint API 或公开 gameplay 行为。
