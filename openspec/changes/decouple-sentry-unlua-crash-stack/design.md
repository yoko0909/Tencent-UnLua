## 背景

已有 Android 和 iOS Sentry 崩溃 hook 可以在 native crash 路径中进入项目代码，并通过 `FSentryCrashLogAppender::AppendCrashInfo(...)` 尝试写入 Lua 堆栈。当前实现的问题是 Sentry 插件直接引用 UnLua 插件，使 Sentry 的通用崩溃诊断职责与项目 Lua 诊断职责耦合。

本变更只处理模块边界和职责归属，不把同步 Lua 堆栈采集声明为生产安全方案。

## 目标 / 非目标

**目标：**

- Sentry 模块不再 include UnLua 头文件。
- Sentry 模块不再在 `Sentry.Build.cs` 中依赖 UnLua。
- Sentry 通过 `FSentryCrashLogAppender` 暴露崩溃诊断初始化和追加通知。
- MiscInterface 同时依赖 Sentry 与 UnLua，并在模块启动时注册监听。
- Lua 状态机获取、Lua 调用栈生成、`LuaCallStack.log` 路径管理和写入逻辑全部迁移到 MiscInterface。
- Android/iOS 现有 crash hook 的调用入口保持稳定，减少对已验证 hook 链路的扰动。

**非目标：**

- 不在本变更中实现生产级 crash-time safe Lua 堆栈缓存。
- 不修改 Sentry 事件 payload、envelope、附件上传策略或第三方二进制。
- 不修改 SentryCrash、libsentry 或 Sentry Android/iOS SDK。
- 不新增 Lua 运行时调度、跨线程 Lua VM 访问策略或多 Lua Env 选择策略。

## 设计决策

1. **Sentry 暴露通知，而不是执行 Lua 逻辑。**

   `FSentryCrashLogAppender` 迁移到 Sentry Public 接口，提供注册/反注册崩溃诊断初始化和追加通知的 native multicast delegate。Sentry hook 继续调用 `InitExtLogFullPath()` 和 `AppendCrashInfo(...)`，但这些函数只广播通知，不包含 UnLua 或文件写入逻辑。

2. **MiscInterface 作为间接桥接插件。**

   MiscInterface 在 `StartupModule()` 中注册 Sentry 通知，在 `ShutdownModule()` 中反注册。由于 MiscInterface 是项目侧插件，它可以合理地同时依赖 Sentry 与 UnLua，并承担项目定制诊断逻辑。

3. **Lua 堆栈文件由 MiscInterface 管理。**

   `LuaCallStack.log` 的路径初始化、文件触碰、Lua 栈内容写入从 Sentry 移到 MiscInterface。这样 Sentry 插件不需要知道 Lua 堆栈文件的位置、格式或生成方式。

4. **保留现有崩溃 hook 的调用表面。**

   Android/iOS hook 仍然调用 `FSentryCrashLogAppender::InitExtLogFullPath()` 和 `FSentryCrashLogAppender::AppendCrashInfo(...)`，避免同时重写 hook 安装、转发和 Lua 桥接。

5. **显式标注 crash-time 风险。**

   由于监听器可能从 native signal handler 或 SentryCrash callback 同步执行，MiscInterface 中访问 UnLua、构造字符串、打开文件和写文件都不是 async-signal-safe。本阶段只能作为诊断验证路径；若要生产化，应改为正常运行期间缓存 Lua 栈快照，崩溃当场只做安全的固定写入或标记。

## 风险 / 权衡

- **风险：广播监听器仍然在 crash-time 执行。**  
  缓解：在 Sentry public 接口和 MiscInterface 监听函数旁写明该路径仅用于诊断验证，不作为生产安全承诺。

- **风险：MiscInterface 加载晚于 Sentry hook 初始化。**  
  缓解：验证模块加载顺序和启动日志；如果监听器注册太晚，需要调整 MiscInterface 的 LoadingPhase 或在 Sentry 初始化前显式加载。

- **风险：多 Lua Env 场景下只采集默认 Env。**  
  缓解：本变更保持当前行为，只使用 `IUnLuaModule::Get().GetEnv()`；多 Env 策略作为后续独立设计。

- **风险：移除 Sentry 对 UnLua 的依赖后平台编译失败。**  
  缓解：先做静态搜索，再分别编译 Win64 Editor、Android、iOS 目标，确认 Sentry 不再引用 UnLua 符号。

- **风险：Sentry public header 暴露新 C++ API 后命名不稳定。**  
  缓解：使用中性的崩溃诊断命名，不出现 Lua 专用语义，避免把项目定制能力固化进 Sentry API。
