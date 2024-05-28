/**
@brief 姿态传感器模块
@version 3.0
*/

#include <math.h>
#include "Posture.h"

#define DeviceAddress      0x68 //设备的总线地址
#define DeviceWriteAddress 0xD0 //设备写地址
#define DeviceReadAddress  0xD1 //设备读地址
#define Power1Register     0x6B //电源配置寄存器1地址
#define Power2Register     0x6C //电源配置寄存器2地址
#define IdentityRegister   0x75 //出厂号寄存器地址
#define SampleRegister     0x19 //采样分频寄存器地址
#define ConfigRegister     0x1A //配置外部同步和低通滤波寄存器地址
#define AcceRegister       0x1C //加速度计寄存器地址
#define GyroRegister       0x1B //陀螺仪寄存器地址
#define InterRegister      0x38 //中断使能寄存器地址
#define AcceXHRegister     0x3B //加速度计X轴向高位数据寄存器地址

#define AcceY_Remedy 550  //加速度Y轴补偿值
#define AcceZ_Remedy 1450 //加速度Z轴补偿值
#define GyroX_Remedy 153  //角速度X轴补偿值

#define Dt      0.010F //采样间隔时间
#define Q_Angle 0.001F //角度数据置信度
#define Q_Gyro  0.003F //角速度数据置信度
#define R_Angle 0.500F //加速度计测量噪声的协方差

u8 posture_identity;  //校验数
s16 posture_acce_y;   //y轴加速度
s16 posture_acce_z;   //z轴加速度
float posture_roll;   //角度(°)

static const SoftIIC_Config softiic_config = {
     {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_3, GPIO_Mode_Out_OD},
     {RCC_APB2Periph_GPIOB, GPIOB, GPIO_Pin_4, GPIO_Mode_Out_OD}
};

/**
@brief 向指定寄存器写数据
*/
static void Posture_WriteRegister(const u8 address, const u8 data) {
     SoftIIC_Start(&softiic_config);                   //发送起始信号
     SoftIIC_Send(&softiic_config, DeviceWriteAddress);//呼叫从机进入写模式
     SoftIIC_Send(&softiic_config, address);           //修改从机寄存器指针
     SoftIIC_Send(&softiic_config, data);              //修改指定寄存器数据
     SoftIIC_End(&softiic_config);                     //发送结束信号
}

/**
@brief 从指定寄存器读数据
*/
static u8   Posture_ReadRegister(const u8 address) {
     u8 data;
     SoftIIC_Start(&softiic_config);                   //发送起始信号
     SoftIIC_Send(&softiic_config, DeviceWriteAddress);//呼叫从机进入写模式
     SoftIIC_Send(&softiic_config, address);           //修改从机寄存器指针
     SoftIIC_Start(&softiic_config);                   //发送起始信号
     SoftIIC_Send(&softiic_config, DeviceReadAddress); //呼叫从机进入读模式
     data = SoftIIC_Accept(&softiic_config, 1);        //读取指定寄存器数据
     SoftIIC_End(&softiic_config);                     //发送结束信号
     return data;
}

/**
@brief 初始化软件IIC通信并配置姿态传感器
*/
void Posture_Initialize(void) {
     SoftIIC_InitializeAndRemap(&softiic_config, GPIO_Remap_SWJ_JTAGDisable);//初始化软件IIC通信模块
     Posture_WriteRegister(Power1Register, 0x01);                            //退出睡眠模式 | 关闭温度测量 | 选择时钟源
     Posture_WriteRegister(Power2Register, 0x00);                            //启动6轴传感器
     Posture_WriteRegister(InterRegister, 0x00);                             //关闭所有中断
     Posture_WriteRegister(SampleRegister, 0x09);                            //配置采样分频
     Posture_WriteRegister(ConfigRegister, 0x06);                            //配置外部同步和低通滤波器
     Posture_WriteRegister(AcceRegister, 0x00);                              //配置加速度计量程为±2g
     Posture_WriteRegister(GyroRegister, 0x00);                              //配置陀螺仪为±250°/s
     posture_identity = Posture_ReadRegister(IdentityRegister);              //获取姿态传感器的校验数
}

/**
@brief 计算Roll最优角度和角速度
*/
void Posture_Refresh(void) {
     u8 initial_data_high, initial_data_low;//储存从传感器读出的高低字节数据
     
     /* 读取传感器Y轴加速度 */
     initial_data_high = Posture_ReadRegister(AcceXHRegister + 2);
     initial_data_low = Posture_ReadRegister(AcceXHRegister + 3);
     posture_acce_y = ((initial_data_high << 8) | initial_data_low) + AcceY_Remedy;//计算校准后的Y轴加速度
     
     /* 读取传感器Z轴加速度 */
     initial_data_high = Posture_ReadRegister(AcceXHRegister + 4);
     initial_data_low = Posture_ReadRegister(AcceXHRegister + 5);
     posture_acce_z = ((initial_data_high << 8) | initial_data_low) + AcceZ_Remedy;//计算校准后的Z轴加速度
     
     /* 读取传感器X轴角速度 */
     initial_data_high = Posture_ReadRegister(AcceXHRegister + 8);
     initial_data_low = Posture_ReadRegister(AcceXHRegister + 9);
     s16 gyro_x = ((initial_data_high << 8) | initial_data_low) + GyroX_Remedy;//计算校准后的X轴角速度
     
     static float bias, k0, k1, roll;//Q_bias:陀螺仪的偏差 | K_0:用于计算最优估计值 | K_1:用于计算最优估计值的偏差 | angle:最优Roll角度
     static float PP[2][2] = {{1, 0}, {0, 1}};//过程协方差矩阵P | 初始值为单位矩阵
     float posture_initial_roll_angle = atan((float)posture_acce_y / (float)posture_acce_z);//粗略计算Roll角度(rad)
     float posture_initial_roll_dt = ((float)gyro_x / 32768.0) * 250.0 * (acos(-1.0) / 180.0);//计算Roll角速度(rad/s)
     roll += (posture_initial_roll_dt - bias) * Dt;//角度值等于上次最优角度加角速度减零漂后积分
     PP[0][0] = PP[0][0] + Q_Angle - (PP[0][1] + PP[1][0]) * Dt;
     PP[0][1] = PP[0][1] - PP[1][1] * Dt;
     PP[1][0] = PP[1][0] - PP[1][1] * Dt;
     PP[1][1] = PP[1][1] + Q_Gyro;
     k0 = PP[0][0] / (PP[0][0] + R_Angle);
     k1 = PP[1][0] / (PP[0][0] + R_Angle);
     roll = roll + k0 * (posture_initial_roll_angle - roll);
     bias = bias + k1 * (posture_initial_roll_angle - roll);
     PP[0][0] = PP[0][0] - k0 * PP[0][0];
     PP[0][1] = PP[0][1] - k0 * PP[0][1];
     PP[1][0] = PP[1][0] - k1 * PP[0][0];
     PP[1][1] = PP[1][1] - k1 * PP[0][1];
     posture_roll = (roll / acos(-1.0)) * 180.0;
}
