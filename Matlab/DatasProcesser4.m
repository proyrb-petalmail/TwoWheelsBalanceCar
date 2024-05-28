clear; % 清空所有当前数据

import matlab.ble.* % 导入低功耗蓝牙设备包

global times;
global x_view;
global x_data;
global y_data_roll;
global y_data_target;
global y_data_ASpeed;
global y_data_BSpeed;
global y_data_APower;
global y_data_BPower;

x_view = 30; % 横坐标显示范围
roll_view = 100;
target_view = 100;
ASpeed_view = 100;
BSpeed_view = 100;
APower_view = 7200;
BPower_view = 7200;
times = x_view + 1; % 下一次接收并显示数据的时刻

figure; % 创建窗口
x_data = linspace(1, x_view, x_view); % 储存x轴数据
y_data_roll = zeros(1, x_view); % 储存y轴数据1
y_data_target = zeros(1, x_view); % 储存y轴数据2
y_data_ASpeed = zeros(1, x_view); % 储存y轴数据3
y_data_BSpeed = zeros(1, x_view); % 储存y轴数据4
y_data_APower = zeros(1, x_view); % 储存y轴数据5
y_data_BPower = zeros(1, x_view); % 储存y轴数据6

devices_list = blelist("Timeout", 3); % 通过设备名称搜索该设备
link = ble("@230904EF18"); % 尝试连接指定名称的设备
feature = characteristic(link, "FFF0", "FFF1"); % 获取设备特征
feature.DataAvailableFcn = @AcceptData; % 绑定广播时执行的回调函数
subscribe(feature); % 订阅蓝牙设备的通知

while true
    % Roll
    subplot(3, 2, 1);
    plot(x_data, y_data_roll, "-r"); % 绘制y轴数据1
    title('Roll(°)');
    xlim([x_data(1), x_data(end)]); % 调整坐标轴范围实现滚动显示
    ylim([-roll_view, roll_view]); % 调整坐标轴范围实现滚动显示

    % Target
    subplot(3, 2, 2);
    plot(x_data, y_data_target, "-g"); % 绘制y轴数据2
    title('Target Speed');
    xlim([x_data(1), x_data(end)]); % 调整坐标轴范围实现滚动显示
    ylim([-target_view, target_view]); % 调整坐标轴范围实现滚动显示

    % ASpeed
    subplot(3, 2, 3);
    plot(x_data, y_data_ASpeed, "-b"); % 绘制y轴数据3
    title('MotorA Speed');
    xlim([x_data(1), x_data(end)]); % 调整坐标轴范围实现滚动显示
    ylim([-ASpeed_view, ASpeed_view]); % 调整坐标轴范围实现滚动显示

    % BSpeed
    subplot(3, 2, 4);
    plot(x_data, y_data_BSpeed, "-c"); % 绘制y轴数据4
    title('MotorB Speed');
    xlim([x_data(1), x_data(end)]); % 调整坐标轴范围实现滚动显示
    ylim([-ASpeed_view, ASpeed_view]); % 调整坐标轴范围实现滚动显示

    % APower
    subplot(3, 2, 5);
    plot(x_data, y_data_APower, "-m"); % 绘制y轴数据5
    title('MotorA Power Without Error');
    xlim([x_data(1), x_data(end)]); % 调整坐标轴范围实现滚动显示
    ylim([-APower_view, APower_view]); % 调整坐标轴范围实现滚动显示

    % BPower
    subplot(3, 2, 6);
    plot(x_data, y_data_BPower, "-k"); % 绘制y轴数据6
    title('MotorB Power Without Error');
    xlim([x_data(1), x_data(end)]); % 调整坐标轴范围实现滚动显示
    ylim([-APower_view, APower_view]); % 调整坐标轴范围实现滚动显示
    drawnow; % 立刻绘制
end

% 接收到蓝牙通知时执行的回调函数
function AcceptData(src, evt)
    global times;
    global x_view;
    global x_data;
    global y_data_roll;
    global y_data_target;
    global y_data_ASpeed;
    global y_data_BSpeed;
    global y_data_APower;
    global y_data_BPower;

    initial_data = uint8(read(src, 'oldest')); % 获取广播的数据包
    x_data(1) = []; % 切除向量的第一个元素
    x_data(x_view) = times; % 更新时间坐标
    times = times + 1; % 更新时间
    y_data_roll(1) = []; % 切除向量的第一个元素
    y_data_roll(x_view) = typecast(initial_data(1, [1 2 3 4]), 'single'); % 解析得到y轴数据1
    y_data_target(1) = []; % 切除向量的第一个元素
    y_data_target(x_view) = typecast(initial_data(1, [5 6 7 8]), 'single'); % 解析得到y轴数据2
    y_data_ASpeed(1) = []; % 切除向量的第一个元素
    y_data_ASpeed(x_view) = typecast(initial_data(1, [9 10 11 12]), 'single'); % 解析得到y轴数据3
    y_data_BSpeed(1) = []; % 切除向量的第一个元素
    y_data_BSpeed(x_view) = typecast(initial_data(1, [13 14 15 16]), 'single'); % 解析得到y轴数据4
    y_data_APower(1) = []; % 切除向量的第一个元素
    y_data_APower(x_view) = typecast(initial_data(1, [17 18 19 20]), 'single'); % 解析得到y轴数据5
    y_data_BPower(1) = []; % 切除向量的第一个元素
    y_data_BPower(x_view) = typecast(initial_data(1, [21 22 23 24]), 'single'); % 解析得到y轴数据6
end
