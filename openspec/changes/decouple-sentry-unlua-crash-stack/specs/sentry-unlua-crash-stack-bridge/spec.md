## ADDED Requirements

### Requirement: Sentry shall expose crash diagnostic callbacks
Sentry SHALL expose a native C++ crash diagnostic callback interface that allows other modules to register initialization and crash-info append listeners.

#### Scenario: Listener registers for crash diagnostic initialization
- **WHEN** another module registers an initialization listener with `FSentryCrashLogAppender`
- **THEN** Sentry stores the listener and invokes it when crash diagnostic path initialization is requested

#### Scenario: Listener registers for crash diagnostic append
- **WHEN** another module registers an append listener with `FSentryCrashLogAppender`
- **THEN** Sentry stores the listener and invokes it when crash information is appended

### Requirement: Sentry shall not depend on UnLua
The Sentry module SHALL NOT directly depend on UnLua for Lua crash stack collection.

#### Scenario: Sentry module dependencies are checked
- **WHEN** `Plugins/Sentry/Source/Sentry/Sentry.Build.cs` is inspected
- **THEN** it does not list `UnLua` or `Unlua` as a dependency

#### Scenario: Sentry source references are checked
- **WHEN** Sentry source files are searched
- **THEN** they do not include UnLua headers or call `IUnLuaModule`, `UnLua::GetLuaCallStack`, or other UnLua APIs

### Requirement: MiscInterface shall bridge Sentry and UnLua
MiscInterface SHALL depend on Sentry and UnLua and SHALL own project-specific Lua crash stack collection.

#### Scenario: MiscInterface registers listeners
- **WHEN** the MiscInterface module starts
- **THEN** it registers listeners with `FSentryCrashLogAppender`

#### Scenario: MiscInterface unregisters listeners
- **WHEN** the MiscInterface module shuts down
- **THEN** it unregisters its Sentry crash diagnostic listeners

#### Scenario: Lua crash stack append is requested
- **WHEN** Sentry broadcasts a crash-info append request
- **THEN** MiscInterface attempts to get the UnLua environment, get the Lua state, generate the Lua call stack, and write it to `LuaCallStack.log`

### Requirement: Existing Android and iOS crash hook entry points shall remain stable
Existing Android and iOS Sentry crash hooks SHALL continue calling `FSentryCrashLogAppender::InitExtLogFullPath()` and `FSentryCrashLogAppender::AppendCrashInfo(...)`.

#### Scenario: Android crash hook is inspected
- **WHEN** the Android Sentry crash signal hook is inspected
- **THEN** it still calls the Sentry crash diagnostic appender entry points

#### Scenario: iOS crash hook is inspected
- **WHEN** the iOS Sentry crash diagnostic hook is inspected
- **THEN** it still calls the Sentry crash diagnostic appender entry points

### Requirement: Crash-time safety limitations shall be documented
The implementation SHALL document that synchronous Lua stack collection from crash diagnostic callbacks is diagnostic-only and not async-signal-safe.

#### Scenario: Sentry public callback API is inspected
- **WHEN** the public Sentry crash diagnostic callback API is inspected
- **THEN** it warns that listeners may execute from native crash handlers

#### Scenario: MiscInterface Lua stack listener is inspected
- **WHEN** the MiscInterface Lua stack append listener is inspected
- **THEN** it warns that UnLua access and file writing are not async-signal-safe and are only for diagnostic validation
