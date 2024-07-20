# Aria2 Manager <span style="{float:right}"> [[返回]](README.md)</span>

Aria2 Manager 是一个能够让 [Aria2](https://github.com/aria2/aria2) 在后台运行的小工具（限Windows平台）。

## 使用方法 (限本地使用)

- 对于*重度用户*，推荐下载 [**增强版**](#增强特性)，功能更多，体验更好，一键安装，自动更新，省时省力。

- 对于*小白用户*，如果你只安装了**Aria2 Explorer**，请下载**Setup安装包**，完成安裝后即可使用所有Aria2下载功能。

- 对于*初级用户*，请下载**完全版**，其中已经包含**aria2c.exe**最新版和相关配置文件。只需解压并运行**Aria2Manager.exe**，即可使用默认RPC设置通过扩展下载网络资源。

- 对于*DIY用户*，请下载**独立版**，将**Aria2Manager.exe**解压到**aria2c.exe**所在的目录，并将你的配置文件重命名为**aria2.conf**。即可通过**Aria2Manager.exe**在后台运行Aria2。

> 致 [Aria2 Explorer](https://github.com/alexhua/aria2-explorer) 用户, 关于**启动Aria2**和**打开下载文件**功能，扩展需要通过最新版**Aria2 Manager**来完成。第一次使用需要以管理员身份运行**Aria2 Manager.exe**，并在菜单中点击`注册`，将**Aria2Manger.exe**注册为`aria2://`协议的默认打开程序。注册成功后，可以通过点击`注销`来将**Aria2 Manager**从`aria2://`协议注销。（版本要求：v1.1及以上）

## 截图

![Aria2 Manager](./Screenshot/aria2manager.png)

## 增强版 

**Aria2 Manager** 增强版内置任务管理界面，一键安装即可享轻松受完整的下载体验，目前已上架微软商店。

### 增强特性

- 🔄️ 支持开机自启动
- 🪟 内置任务管理界面
- 👆 一键安装并自动更新
- 🛠️ 一些 Aria2 问题修复
- ⚡ 点击任务名直接打开已下载文件
- 🔕 禁止 Aria2 开启时的已完成任务通知
- 🔀 支持通过 UPnP 进行 BT 和 DHT 端口映射，以提升 BT 下载的连通性
- 🧹 支持删除 **.aria2** 控制文件和已下载文件（ <kbd>Shift</kbd> + **删除任务** ）

### 安装

[<img src="https://get.microsoft.com/images/en-us%20dark.svg" height="56"/>](https://apps.microsoft.com/detail/Aria2%20Manager/9P5WQ68Q20WV?launch=true&cid=github)

商店版将会不定期更新 Aria2 主程序和 BT Tracker 列表，未来还会加入更多的功能，敬请期待。

## 参考 

- https://github.com/phuslu/taskbar
- https://github.com/RossWang/Aria2-Integration
