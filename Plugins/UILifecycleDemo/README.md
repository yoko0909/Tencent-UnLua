# UI Lifecycle Demo

This is a pure C++ UE4.27 demo for observing the lifetime of a parent
`UUserWidget` and a child `UUserWidget` embedded in the parent Widget Blueprint.
It does not use Lua, UnLua, a widget pool, or project-specific classes.

## Copy

Copy the `UILifecycleDemo` folder into the target project's `Plugins` folder.
Regenerate project files and compile the target project.

## Blueprint setup

1. Create a Widget Blueprint named `WBP_LifecycleDemoChild`.
2. Set its parent class to `LifecycleDemoChildWidget`.
3. Create a Widget Blueprint named `WBP_LifecycleDemoParent`.
4. Set its parent class to `LifecycleDemoParentWidget`.
5. Open the parent Widget Blueprint Designer.
6. Add `WBP_LifecycleDemoChild` as a child UserWidget.
7. Rename that child widget to exactly `ChildWidget`.
8. Save and compile both Widget Blueprints.

The `ChildWidget` name is required by the `BindWidget` property in the C++
parent class.

## Console commands

Use the in-game console:

```text
UI.LifecycleDemo.Create /Game/UI/LifecycleDemo/WBP_LifecycleDemoParent.WBP_LifecycleDemoParent_C
UI.LifecycleDemo.Dump
UI.LifecycleDemo.Close
UI.LifecycleDemo.Create
UI.LifecycleDemo.Dump
```

The first `Create` loads and remembers the Blueprint class. Later `Create`
commands can omit the class path.

`Close` only calls `RemoveFromParent`. The demo keeps a `TStrongObjectPtr` to
the parent, so the parent and its WidgetTree remain reachable for this
experiment.

`Release` removes the parent from the viewport and releases the demo strong
reference:

```text
UI.LifecycleDemo.Release
UI.LifecycleDemo.GC
```

## What to compare

The important fields are:

- `this`: the UObject address.
- `path`: the full UObject path, including Outer and Name.
- `outer`: the direct UObject owner.
- `Parent.ChildWidget`: the embedded child instance.
- `BeginDestroy` and `FinishDestroy`: actual UObject destruction stages.
- `PostDuplicate`: UObject duplication/instancing callback, especially useful
  for the child UserWidget created from the parent WidgetTree template.
- `NativeDestruct`: removal from the live Slate/widget hierarchy, not UObject destruction.

For the same-name experiment, compare the first and second `Create` logs.
`BeforeCreate.ExistingSameOuterAndName` also shows whether an object with the
same Outer and Name already exists before `CreateWidget` runs.

Do not infer destruction from `NativeDestruct` alone. Run `Release` followed by
`GC` and check whether both parent and child reach `BeginDestroy` and
`FinishDestroy`.

## Expected experiments

### Same-name create while the previous object is still reachable

```text
UI.LifecycleDemo.Create /Game/UI/LifecycleDemo/WBP_LifecycleDemoParent.WBP_LifecycleDemoParent_C
UI.LifecycleDemo.Close
UI.LifecycleDemo.Create
UI.LifecycleDemo.Dump
```

This is the key test for the `Outer + Name` path. The logs show whether the
existing object is replaced/reconstructed, whether its address is preserved,
and whether the child instance is destroyed or duplicated into a new WidgetTree.

### Normal release and garbage collection

```text
UI.LifecycleDemo.Close
UI.LifecycleDemo.Release
UI.LifecycleDemo.GC
```

This isolates normal UObject destruction. The old parent and child should only
become collectible after the demo releases its strong reference and the
viewport reference has been removed.
