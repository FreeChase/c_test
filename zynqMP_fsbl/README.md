ZynqMP平台，基于fsbl做了串口的在线升级 ，flash分为三个区域 fsbl bit app
上电默认为IDLE状态，接受到魔数 AA 55 AA 55进入到升级状态，走Ymodem协议，使用secureCRT-Ymodem进行升级
根据文件名来升级

userFirmwareUpdate.c
{"zsfsbl",IMAGE_BASE_ADDR_FSBL},
{"zsbit",IMAGE_BASE_ADDR_BIT},
{"zsapp",IMAGE_BASE_ADDR_APP},