# wear-level 简介
这模块功能，主要实现`flash`的均衡磨损算法，包括但不限于`spiflash`。注意相关具体驱动需要自己实现。<br>
基本设计思路为，在`flash`中开辟一段区域，然后将其划分为N个`Unit`,一个`Unit`即包含了所有用户存储信息+校验码+计数标志,
然后循环写入`Unit`即可，读取时，循环判断校验码+计数标志即可获得最新写入数据

## 配置宏定义

```c
#define FLASH_SECTOR_SIZE       4096    // Example: 4KB sector size
#define WEAR_LEVEL_AREA_START   0x10000 // Example: Start address of wear-leveling area (e.g., after firmware)
#define NUM_LOGICAL_BLOCKS      16      // Number of logical blocks in the wear-leveling area.

#define DATA_UNIT_SIZE          1024                                                        // data block basic size
#define DATA_UNIT_NUMBER        (FLASH_SECTOR_SIZE/DATA_UNIT_SIZE*NUM_LOGICAL_BLOCKS)       // data block total size

```

## 数据结构

```c
typedef struct {
    uint16_t  magic_number;     // A unique number to identify valid headers (e.g., 0xA55A)
    uint32_t  write_counter;    // Increments with each write, used to find the latest valid block
    uint16_t  checksum;         // CRC or simple sum for data integrity
    ProductData_t data;         // The actual product data
} FlashBlockHeader_t;
```
```c
typedef struct {
    uint32_t power_on_seconds; // The actual power-on time in seconds
    uint32_t test_value_a;     // Additional test data
    uint16_t test_value_b;
} ProductData_t;

```
用户自定义修改 `ProductData_t`，储存想要的用户数据
**注意**:`FlashBlockHeader_t`不要超过`DATA_UNIT_SIZE`


```c
typedef struct {
    uint32_t curDataUnitIndex;      //current Data Unit index
    uint32_t current_write_counter; //current  be written Data Unit index
    int validDataUnit;              // info valid flag
    uint32_t last_valid_counter;    // Track counter of latest valid block
} WearLevelState_t;

static WearLevelState_t wear_state = {
    .curDataUnitIndex = 0,
    .current_write_counter = 0,
    .validDataUnit = -1,
    .last_valid_counter = 0
};


```
## 使用函数


### flash_wear_level_init

### flash_wear_level_write

### flash_wear_level_read

```c
// 先判断标志位，看当前信息是否合法，否则调用Init函数，重新建立信息
if((wear_state.validDataUnit != -1) || (flash_wear_level_init() != -1) )
```


## 基本操作流程

1. 调用`flash_wear_level_init`，进行管理结构体`wear_state`初始化，此结构体包含信息有效标志位+当前读索引+当前写索引
2. 调用`flash_wear_level_read`初始化管理结构体`wear_state`
3. 调用`flash_wear_level_write`更新用户数据