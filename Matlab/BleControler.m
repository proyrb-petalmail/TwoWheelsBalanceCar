clear; % 清空所有当前数据

import matlab.ble.* % 导入低功耗蓝牙设备包

devices_list = blelist("Timeout", 3); % 通过设备名称搜索该设备
link = ble("@230904EF18") % 尝试连接指定名称的设备
feature = characteristic(link, "FFF0", "FFF2"); % 获取设备特征

while true
    for times = 1:1:30
        write(feature, 1); % 前进
    end
    for times = 1:1:30
        write(feature, 2); % 后退
    end
end
