# Aria2 Manager <span style="{float:right}"> [[中文]](README.cn.md)</span>

Aria2 Manager is a useful tool that allows user to run [Aria2](https://github.com/aria2/aria2) in the background easily. (Only for Windows)

## Usage (only for local use)

- For *power user*, it is recommended to download the [**Enhanced Version**](#enhanced-features), which has more features, better experience, one-click installation, automatic updates, saving time and effort for you.

- For *fresh new user*, if you had only installed [Aria2 Explorer](https://github.com/alexhua/aria2-explorer), please download **setup version**. After installation, you could enjoy all the Aria2 download features. 

- For *DIY user*, please download the **full version** zip which already contains the latest **aria2c.exe** and configuration files. Just extract it and run **Aria2Manager.exe**. Then, the extension should work well with default RPC settings.

- For Aria2 *veteran user*, please download **standalone version** and extract **Aria2Manager.exe** to the directory where **aria2c.exe** exists, and rename your configuration file to **aria2.conf**. Then, you could start up Aria2 in the background by running **Aria2Manager.exe**.

> To [Aria2 Explorer](https://github.com/alexhua/aria2-explorer) user, regarding the `Start Aria2` and `Open Download Folder` features, the extension needs to implement them through the use of **Aria2 Manager**. To enable these features, please run **Aria2 Manager.exe** as an administrator and click on `Register` in the option menu to set **Aria2Manager.exe** as the default application for the `aria2://` protocol at first time. Once the registration is successful, users could click on `Unregister` to unregister **Aria2Manager.exe** from the `aria2://` protocol if you do not need it anymore. （Requirement: Version ≥ v1.1）

## Screenshot

![Aria2 Manager](./Screenshot/aria2manager.png)

## Enhanced version 

**Aria2 Manager** enhanced version with built-in task management UI has been launched to Microsoft Store.

### Enhanced features

- 🛠️ Some Aria2 bug fixes
- 🔄️ Auto-startup on boot
- 🪟 Built-in task management UI
- 🔀 BT and DHT port mapping via UPnP
- 👆 One-click installation and automatic update
- 🔕 Suppress task notification on Aria2 startup
- ⚡ Click on the task name to open the downloaded file directly
- 🧹 Support for removing **.aria2** control file and downloaded file(s) ( <kbd>Shift</kbd> + **Remove Task** )

### Installation

[<img src="https://get.microsoft.com/images/en-us%20dark.svg" height="56"/>](https://apps.microsoft.com/detail/Aria2%20Manager/9P5WQ68Q20WV?launch=true&cid=github)

The enhanced version will always keep the Aria2 binary latest and update BT tracker list opportunistically. Also, there will be more features in the future.

## Reference 

- https://github.com/phuslu/taskbar
- https://github.com/RossWang/Aria2-Integration
