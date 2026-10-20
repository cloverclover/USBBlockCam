# USBBlockCam

阻止 macOS 为问题摄像头加载驱动的 OpenCore kext，用于解决启动时 USB 摄像头导致的卡死问题。

## 问题背景

部分笔记本内置摄像头（Sunplus，`VID_0x174F & PID_0x246A`，"Integrated Camera"）会在 OpenCore 引导阶段触发故障：

1. 摄像头枚举成功后，字符串描述符请求（`type 0x03 index 2`）超时 5000ms
2. XHCI 命令环超时 → `hardwareException` → 控制器命令环 abort
3. 同一控制器上的启动盘枚举被阻塞
4. 卡在 `Still waiting for root device`，无法完成启动

## 方案

`USBBlockCam.kext` 以最高 `IOProbeScore`（1000000）注册两层 personality：

| Personality | IOProviderClass | 作用 |
|---|---|---|
| USBBlockCam Device | `IOUSBHostDevice` | 匹配 VID/PID，抢占设备层驱动 |
| USBBlockCam Interface | `IOUSBHostInterface` | 接口层兜底（C++ probe 内回溯父设备校验 VID/PID） |

匹配成功后 `probe` 返回非 NULL 占住设备，阻止 `AppleUSBCDCCompositeDevice` 等驱动绑定，避免触发超时的字符串描述符请求；非目标设备直接放行（`probe` 返回 NULL）。

## 目录结构

```
USBBlockCam/          kext 源码
  kmod_info.c         kmod_info 定义（现代 SDK 不再提供，OC 预链接必需）
  USBBlockCamDevice.* 设备层驱动
  USBBlockCamInterface.* 接口层驱动
  Makefile            本地/CI 构建
.github/workflows/    GitHub Actions 自动编译
XHC0/1/2.dsl          XHC USB 控制器 ACPI 参考（已注释）
```

## 构建

推送到 GitHub 后 Actions 自动编译（macOS runner + Xcode 16.4），或在仓库 Actions 页面手动触发 `Build USBBlockCam kext`。

产物：run 页面底部 Artifacts → `USBBlockCam-kext`，下载解压一次即为 `USBBlockCam.kext`（x86_64，ad-hoc 签名）。

## 安装

1. 将 `USBBlockCam.kext` 放入 `EFI/OC/Kexts/`
2. `config.plist` → `Kernel` → `Add` 添加：

```xml
<dict>
    <key>BundlePath</key>
    <string>USBBlockCam.kext</string>
    <key>Enabled</key>
    <true/>
    <key>ExecutablePath</key>
    <string>Contents/MacOS/USBBlockCam</string>
    <key>PlistPath</key>
    <string>Contents/Info.plist</string>
</dict>
```

3. 确认 SIP 允许未签名 kext（`csr-active-config` 含 `0x0A` 位即可，如 `430A0000`）；`SecureBootModel` 可保持默认
4. 重启验证

## 验证

OpenCore 日志中应出现：

```
OC: Prelinked injection USBBlockCam.kext (V1.0.0) - Success
```

`IOLog` 不会写入 OpenCore 日志，进系统后查看内核日志和加载状态：

```sh
log show --last boot --predicate 'process == "kernel" AND eventMessage CONTAINS "USBBlockCam"'
kmutil showloaded | grep -i usbblockcam
```

应出现 `USBBlockCamDevice: probe: ...` 和 `start: claimed ...`。

预期效果：摄像头（`174f:246a`）不再被 Apple 驱动匹配，启动不再出现 XHCI `timed out after 5000ms` / `Still waiting for root device`。

## 备选规避方案（无需 kext）

外接 USB 2.0 Hub/扩展坞，将启动 U 盘接到扩展坞再插入笔记本，可绕过内置 XHC 上的摄像头超时问题。

## 开发备注

- 现代 Xcode SDK 的 `libkmod.a` 已不提供 `_kmod_info` 符号，必须在 kext 内自行定义（见 `kmod_info.c`），否则 OpenCore 预链接会以 `Invalid Parameter` 静默失败（`OC: Prelinked injection ... - Invalid Parameter`）
- `super::` 在 kext 的 C++ 编译单元中未定义，需显式写父类名（`IOUSBHostDevice::probe` 等）
- OC 调试：`Misc → Debug` 开 `Target=67`，日志中 `OCAK:` 前缀为 kext 预链接阶段消息
