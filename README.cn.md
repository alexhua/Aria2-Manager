# Aria2 Manager <span style="{float:right}"> [[返回]](README.md)</span>

Aria2 Manager 是一个能够让 [Aria2](https://github.com/aria2/aria2) 在后台运行的小工具（限Windows平台）。

## 使用方法 (限本地使用) 

- 对于小白用户，如果你只安装了**Aria2 Explorer**，请下载Setup安装包，完成安裝后即可使用所有功能。

- 对于初级用户，请下载完全版，其中已经包含**aria2c.exe**最新版和相关配置文件。只需解压并运行**Aria2Manager.exe**，即可使用默认RPC设置通过扩展下载网络资源。

- 对于高级用户，请下载独立版，将**Aria2Manager.exe**解压到**aria2c.exe**所在的目录，并将你的配置文件重命名为**aria2.conf**。即可通过**Aria2Manager.exe**在后台运行Aria2。

- 致 [Aria2 Explorer](https://github.com/alexhua/aria2-explorer) 用户, 关于**启动Aria2**和**打开下载文件**功能，扩展需要通过最新版**Aria2 Manager**来完成。第一次使用需要以管理员身份运行**Aria2 Manager.exe**，并在菜单中点击`注册`，将**Aria2Manger.exe**注册为`aria2://`协议的默认打开程序。注册成功后，可以通过点击`注销`来将**Aria2 Manager**从`aria2://`协议注销。（版本要求：v1.1及以上）

## 截图

![Aria2 Manager](./Screenshot/aria2manager.png)

## 安装 Windows 商店版 

[<img src="https://get.microsoft.com/images/en-us%20dark.svg" height="56"/>](https://apps.microsoft.com/detail/Aria2%20Manager/9P5WQ68Q20WV?launch=true&cid=github)

商店版将会定期更新Aria2主程序和BT Tracker信息，未来还会加入更多的功能。

## 参考 

- https://github.com/phuslu/taskbar
- https://github.com/RossWang/Aria2-Integration
