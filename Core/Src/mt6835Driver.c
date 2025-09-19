//通过SPI 与MT6835通讯，控制MT6835

/*
SPI操作：
            command[4] + addr[12] + data[8]
    1、单字节读寄存器
    SEND    | 0011       addr[12]
    RESIVE  |                       data[8]

    2、单字节写寄存器
    SEND    | 0110       addr[12]   data[8]
    RESIVE  |                       

    3、烧录EEPROM
    SEND    | 1100       00..[12]   
    RESIVE  |                       0x55[8] resived

    4、设置当前角度为0点
    SEND    | 0101       00..[12]   
    RESIVE  |                       0x55[8] resived

    5、单字节读角度 需要读4次获得一次位置
        0x003[7:0]    -0x004[7:0]   -0x005[7:0]             -0x006[7:0]
        angle[20:13]  angle[12:5]   Agl[4:0]+STATUS[2:0]    CRC[7:0]

        Theta = sum[0:20] Agl[i] * 2^i / 2^21  * 360deg

        CRC校验 angle[20:0] + STATUS[2:0] 共24bit
        CRC公式 x8 + x2 + x + 1   | ANGLE[20]先进
    SEND    | 0011       addr[12]
    RESIVE  |                       data[8]

    6、角度连续读取
        持续返回0x003 - 0x006的内容，直到通讯被终止
    SEND    | 1010       0x003[12]
    RESIVE  |                       data[8]

地址表：
    地址    |   7 - 0                                                   |  位置
    0x001   | USER_ID                                                   | EEPROM 给用户使用的，可自己读写
    0x002   | NA
    0x003   | ANGLE[20:13]                                              | 只读
    0x004   | ANGLE[12:5]                                               | 只读
    0x005   | ANGLE[4:0] + STAT[2:0]                                    | 只读
    0x006   | CRC[7:0]                                                  | 只读
    0x007   | ABZ_RES[13:6]                                             | EEPROM
    0x008   | ABZ_RES[5:0] + ABZ_OFF + AB_SWAP                          | EEPROM
    0x009   | ZERO_POS[11:4]                                            | EEPROM
    0x00A   | ZERO_POS[3:0] + Z_EDGE + Z_PUL_WID[2:0]                   | EEPROM
    0x00B   | Z_PHASE[1:0] + UVW_MUX + UVW_OFF + UVW_RES[3:0]           | EEPROM
    0x00C   | XnotForUSER[2] + NLC_EN + PWM_FQ + PWM_POL + PWM_SEL[2:0] | EEPROM
    0x00D   | XnotForUSER[4] + ROT_DIR + HYST[2:0]                      | EEPROM
    0x00E   | GPIO_DS + AUTOCAL_FREQ[2:0] + XnotForUSER[4]              | EEPROM
    0x00F   | XnotForUSER[8]                                            | EEPROM
    0x010   | XnotForUSER[8]                                            | EEPROM
    0x011   | XnotForUSER[5] + BW[2:0]                                  | EEPROM
    0x012   | XnotForUSER[8]                                            | EEPROM
    0x013   |                                                           | EEPROM
    ...     |      NLC Byte 0x00~0xBF(192Bytes)                         | EEPROM
    0x0D2   |                                                           | EEPROM

说明：
    Status[2:0]:
        2 == 1 : 芯片欠压报警
        1 == 1 : 外部磁场太弱
        0 == 1 : 转速过高，超过12w转
    ABZ_RES[13:0]:
        Value   |   AB分辨率（脉冲/圈）
        0x0000  |       1
        0x0001  |       2
        ...
        0x3FFF  |       16384
    ABZ_OFF:
        0x0 |   ABZ输出
        0x1 |   ABZ不输出
    AB_SWAP:
        0x0 |   AB信号不互换
        0x1 |   AB信号互换
    ZERO_POS[11:0]:
        Value   |   绝对角度值（°）
        0x000   |       0
        0x001   |       0.088
        ...
        0xFFE   |       359.824
        0xFFF   |       359.912
    Z_EDGE: 
        Z脉冲和角度0之间的关系
        0x0 |   上升沿对齐0°
        0x1 |   下降沿对齐0°
    Z_PUL_WID[2:0]:
        Z脉冲宽度
        0x0 |   1°
        0x1 |   2
        0x2 |   4
        0x3 |   8
        0x4 |   16
        0x5 |   60
        0x6 |   120
        0x7 |   180
    Z_PHASE[1:0]:
        Z相位与AB脉冲之间的相位关系 参考规格书
    UVW_MUX:
        定义1#-3#引脚输出状态
        0x0 |   UVW
        0x1 |   -A -B -Z
    UVW_OFF(不用时可关，减少干扰):
        0x0 |   WUV输出
        0x1 |   WUV不输出
    UVW_RES[3:0]:
        UVW输出分辨率的对级数
        0x0 |   1
        ...
        0xF |   16
    NLC_EN:
        控制NLC校准开启
        0x0 | 关闭
        0x1 | 开启
    PWM_FQ:
        输出PWM频率
        0x0 |   994hz
        0x1 |   497hz
    PWM_POL:
        输出PWM有效电平
        0x0 |   高有效
        0x1 |   低有效
    PWM_SEL[2:0]:
        PWM输出内容
        0x0     |   12bit角度数据
        0x2     |   12bit速度数据
        others  |   工厂测试专用
    ROT_DIR:
        0x0 |   逆时针输出角度增加
        0x1 |   顺时针输出角度增加
    HYST[2:0]:
        输出角度的之后窗口：
        0x0 |   0.022°
        0x1 |   0.044
        0x2 |   0.088
        0x3 |   0.176
        0x4 |   0
        0x5 |   0.003
        0x6 |   0.006
        0x7 |   0.011
    GPIO_DS:
        IO输出能力：
        0x0 |   默认驱动能力
        0x1 |   默认驱动能力x2
    AUTOCAL_FREQ[2:0]:
        标定转速区间
        0x0 |   3200-6400
        0x1 |   1600-3200
        0x2 |   800-1600
        0x3 |   400-800
        0x4 |   200-400
        0x5 |   100-200
        0x6 |   50-100
        0x7 |   25-50
    BW[2:0]:
        MT6835的系统带宽：
        0x0 |   基础带宽    |慢 噪音小
        0x1 |   x2
        0x2 |   x4
        0x3 |   x8
        0x4 |   x16
        0x5 |   x32
        0x6 |   x64
        0x7 |   x128        |快 噪音大

自校准：
    自动会写入到EEPROM 
    通常INL会降低到±0.07°以下

    1、电机运输旋转
        自校准 速度区间 可设置：
        配置AUTO_CAL_FREQ[2:0] 
            0x0 3200-6400
            0x1 1600-3200
            0x2 800-1600
            0x3 400-800
            0x4 200-400
            0x5 100-200
            0x6 50-100
            0x7 25-50
    2、需要速度稳定后
    3、CAL_EN : 由低到高，
        运行64圈以上结束；
        PWM引脚用于检测标定结果：
            占空比    |     结果
            50%      |      进行中ing
            25%      |      失败
            >99%     |      成功 
        或 SPI读取结果 0x113[7:6]：
            00      没有运行校准
            01      进行中ing
            10      失败
            11      成功
    
    校准成功后（>6s），务必给芯片断电
    芯片重新上电，才可以进行别的操作
*/

/*
结构拆分

BOTTOM FUNC：
    单字节 READ
    单字节 WRITE

Level1 FUNC：
    读取位置 （4个字节+CRC校验）

读取角度
*/

