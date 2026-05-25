# Ky3-Plugin

Genshin Impact PC 客户端功能插件，基于 AOB + MinHook 实现，版本更新后无需手动适配。

如果觉得对你有帮助，请点一个 ⭐ Star！

If you find this useful, please give it a ⭐ Star!

## 功能列表

| 功能 | 配置项 | 说明 |
|------|--------|------|
| FPS 解锁 | `enableFps` / `targetFps` | 突破 60 帧限制 |
| VSync 关闭 | `disableVSync` | 关闭垂直同步 |
| FOV 修改 | `enableFov` / `targetFov` | 自定义视野角度 |
| 环境去雾 | `disableFog` | 移除距离/天气雾效 |
| 隐藏 UID | `hideUID` / `hideMenuUID` | 隐藏游戏内/菜单 UID 显示 |
| 隐藏任务横幅 | `hideQuestBanner` | 隐藏任务弹出横幅 |
| 隐藏伤害数字 | `disableDamageText` | 关闭伤害飘字 |
| 跳过大招动画 | `disableCameraAnim` | 跳过元素爆发镜头动画 |
| 手机UI模式 | `touchScreen` | 启用触屏/手机 UI 布局 |
| 便携合成 | `enableCraft` / `craftKey` | 快捷键打开合成台 |
| 便携烹饪 | `enableCooking` / `cookingKey` | 快捷键打开烹饪 |
| 便携派遣 | `enableDispatch` / `dispatchKey` | 快捷键打开探索派遣 |
| 便携锻造 | `enableForge` / `forgeKey` | 快捷键打开锻造 |
| 开放组队 | `removeTeamAnim` | 移除组队切换动画 |

## 技术架构

```
UnlockerDll/
├── dllmain.cpp          # 入口
├── init.h               # Hook初始化（特征码扫描+挂钩）
├── game_loop.h          # GameUpdate主循环逻辑
├── config_load.h        # INI配置热加载
├── core/
│   ├── config.h         # 配置结构体 & 全局变量
│   ├── signatures.h     # AOB特征码定义（多候选fallback）
│   └── pattern_scanner.h # 通用特征码扫描引擎
├── features/
│   ├── fps_unlock.h     # FPS/VSync
│   ├── visual_mods.h    # FOV/Fog/Perspective
│   ├── ui_hiding.h      # UID/横幅/伤害隐藏
│   ├── craft.h          # 便携合成
│   ├── cooking.h        # 便携烹饪
│   ├── expedition.h     # 便携派遣
│   ├── forge.h          # 便携锻造
│   ├── team.h           # 组队
│   └── mobile_ui.h      # 手机UI
└── MinHook/             # MinHook库
```

## 免责声明

本项目仅供学习研究逆向工程技术，请勿用于任何违反游戏服务条款的行为。使用本工具产生的一切后果由使用者自行承担。
