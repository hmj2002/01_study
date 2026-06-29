/*!
    \file    MyCAN.c
    \brief   GD32F407 CAN 驱动 - STM32 风格平替版实现
    \version V1.0.0

    关键设计：
    1. 所有硬件差异通过 MyCAN.h 中的宏配置，本文件无需修改
    2. 采用轮询方式收发，简单可靠，适合大多数应用场景
    3. 过滤器配置为接收所有消息（掩码全0），如需过滤请修改 MyCAN_Init()
*/

#include "can.h"          /* 包含头文件，获取宏配置 */
#include <string.h>         /* 用于 memcpy 数据拷贝 */

/* ============================================================
 *  【私有函数】仅在本文件内部使用
 * ============================================================ */

/**
 * @brief  配置 CAN 引脚为复用功能
 * @param  无（使用 MyCAN.h 中的宏）
 * @retval 无
 * @note   
 *   GD32 的 GPIO 配置分为三步：
 *   1. gpio_mode_set()      - 设置模式（输入/输出/复用/模拟）和上拉/下拉
 *   2. gpio_output_options_set() - 设置输出类型（推挽/开漏）和速度
 *   3. gpio_af_set()        - 设置复用功能号（AF9）
 *   
 *   与 STM32 的区别：
 *   STM32 用一个 GPIO_InitTypeDef 结构体一次性配置
 *   GD32 需要分三个函数调用，更灵活但更繁琐
 */
static void CAN_GPIO_Config(void)
{
    /* 第一步：使能 GPIO 时钟（对应 STM32 的 RCC_APB2PeriphClockCmd） */
    rcu_periph_clock_enable(CAN_GPIO_RCU);

    /* 第二步：配置 TX 引脚（PB13）
     * 
     * 参数说明：
     * - CAN_GPIO_PORT: GPIOB（端口）
     * - GPIO_MODE_AF:  复用功能模式（不是普通输出）
     * - GPIO_PUPD_PULLUP: 内部上拉（CAN 总线空闲时为高电平，上拉确保稳定）
     * - CAN_TX_PIN: GPIO_PIN_13（引脚号）
     */
    gpio_mode_set(CAN_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CAN_TX_PIN);

    /* 第三步：配置 TX 输出特性
     * 
     * 参数说明：
     * - GPIO_OTYPE_PP: 推挽输出（CAN 需要强驱动能力）
     * - GPIO_OSPEED_50MHZ: 50MHz 速度（CAN 最高 1Mbps，50M 足够）
     */
    gpio_output_options_set(CAN_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CAN_TX_PIN);

    /* 第四步：设置复用功能号
     * 
     * GD32 每个引脚有多个复用功能，需要显式指定：
     * - PB13 的 AF9 是 CAN0_TX（需查数据手册确认）
     * - 不同引脚可能用不同 AF 号，这是 GD32 与 STM32 的主要区别
     */
    gpio_af_set(CAN_GPIO_PORT, CAN_GPIO_AF, CAN_TX_PIN);

    /* 同理配置 RX 引脚（PB12）
     * 
     * 注意：RX 也是复用功能模式，不是纯输入！
     * CAN 引脚必须配置为 AF 模式，否则数据无法进入 CAN 外设
     */
    gpio_mode_set(CAN_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CAN_RX_PIN);
    gpio_output_options_set(CAN_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CAN_RX_PIN);
    gpio_af_set(CAN_GPIO_PORT, CAN_GPIO_AF, CAN_RX_PIN);
}

/* ============================================================
 *  【公共函数】对外接口
 * ============================================================ */

/**
 * @brief  初始化 CAN 外设（STM32 风格）
 * @param  无
 * @retval 无
 * @note   
 *   初始化流程（与 STM32 对比）：
 *   
 *   STM32:                          GD32:
 *   1. RCC 使能时钟                 1. rcu_periph_clock_enable()
 *   2. GPIO_Init() 一次性配置       2. gpio_mode_set() + gpio_output_options_set() + gpio_af_set()
 *   3. CAN_DeInit()                 3. can_deinit()
 *   4. CAN_Init()                   4. can_init()
 *   5. CAN_FilterInit()             5. can_filter_init()
 *   
 *   关键区别：
 *   - GD32 需要显式设置复用功能号（AF9）
 *   - GD32 的 can_struct_para_init() 用于填充默认值
 *   - GD32 的过滤器参数结构体字段名不同
 */
void MyCAN_Init(void)
{
    /* 定义局部结构体变量，GD32 风格 */
    can_parameter_struct canParam;          /* CAN 初始化参数（对应 STM32 的 CAN_InitTypeDef） */
    can_filter_parameter_struct filterParam; /* 过滤器参数（对应 STM32 的 CAN_FilterInitTypeDef） */

    /* ========== 步骤1：使能时钟 ==========
     * 
     * GD32 的时钟使能分两组：
     * - RCU_CANx: CAN 外设时钟（在 APB1 总线上）
     * - RCU_GPIOx: GPIO 时钟（在 AHB1 总线上）
     * 
     * 必须先使能时钟，否则寄存器无法访问
     */
    rcu_periph_clock_enable(CAN_USED_RCU);  /* CAN0 或 CAN1 时钟 */
    CAN_GPIO_Config();                         /* 内部调用，使能 GPIO 时钟并配置引脚 */

    /* ========== 步骤2：复位 CAN 外设 ==========
     * 
     * 将 CAN 寄存器恢复到默认值，避免上次运行的配置残留
     * 类似于 STM32 的 CAN_DeInit()
     */
    can_deinit(CAN_USED_PERIPH);

    /* ========== 步骤3：填充 CAN 参数默认值 ==========
     * 
     * GD32 特有函数，将结构体所有字段设为默认值
     * 防止未初始化的字段导致异常
     * 类似于 memset(&canParam, 0, sizeof(canParam)) 但更安全
     */
    can_struct_para_init(CAN_INIT_STRUCT, &canParam);

    /* ========== 步骤4：配置波特率和模式 ==========
     * 
     * 波特率计算公式（与 STM32 相同）：
     * 波特率 = APB1时钟 / ((1 + BS1 + BS2) * Prescaler)
     * 
     * 当前配置：250kbps
     * 42MHz / ((1 + 6 + 1) * 21) = 42M / 168 = 250k
     * 
     * 参数对应 STM32：
     * - working_mode      -> CAN_Mode
     * - prescaler         -> CAN_Prescaler
     * - time_segment_1    -> CAN_BS1（实际值 = 宏值+1）
     * - time_segment_2    -> CAN_BS2（实际值 = 宏值+1）
     * - resync_jump_width -> CAN_SJW（实际值 = 宏值+1）
     * - auto_retrans      -> CAN_NART（ENABLE=允许重传，对应 DISABLE）
     */
    canParam.working_mode         = CAN_WORKING_MODE;   /* 正常模式或回环模式 */
    canParam.resync_jump_width    = CAN_SJW;             /* 同步跳转宽度 */
    canParam.time_segment_1       = CAN_BS1;             /* 时间段1 = 7 Tq */
    canParam.time_segment_2       = CAN_BS2;             /* 时间段2 = 2 Tq */
    canParam.prescaler            = CAN_PRESCALER;       /* 预分频器 = 21 */

    /* 其他参数保持默认或对应 STM32 配置：
     * 
     * STM32 CAN_NART = DISABLE  -> GD32 auto_retrans = ENABLE（命名相反，意义相同：允许自动重传）
     * STM32 CAN_RFLM = DISABLE  -> GD32 rec_fifo_overwrite = DISABLE（FIFO 不覆盖）
     * STM32 CAN_TXFP = DISABLE  -> GD32 trans_fifo_order = DISABLE（按 ID 优先级发送）
     * STM32 CAN_AWUM = DISABLE  -> GD32 auto_wake_up = DISABLE（手动唤醒）
     * STM32 CAN_TTCM = DISABLE  -> GD32 time_triggered = DISABLE（非时间触发）
     * STM32 CAN_ABOM = DISABLE  -> GD32 auto_bus_off_recovery = DISABLE（手动恢复总线关闭）
     */
    canParam.auto_retrans         = ENABLE;     /* 允许自动重传（仲裁失败时重试） */
    canParam.rec_fifo_overwrite   = DISABLE;   /* 接收 FIFO 满时不覆盖旧数据 */
    canParam.trans_fifo_order     = DISABLE;    /* 发送按 ID 优先级，不是 FIFO 顺序 */
    canParam.auto_wake_up         = DISABLE;    /* 手动从睡眠模式唤醒 */
    canParam.time_triggered       = DISABLE;    /* 禁用时间触发通信 */
    canParam.auto_bus_off_recovery = DISABLE;   /* 手动恢复总线关闭状态 */

    /* 执行初始化（对应 STM32 的 CAN_Init()）
     * 返回 SUCCESS 或 ERROR，这里不检查返回值简化代码
     */
    can_init(CAN_USED_PERIPH, &canParam);

    /* ========== 步骤5：配置过滤器 ==========
     * 
     * 过滤器决定哪些消息能进入接收 FIFO。
     * 这里配置为"接收所有"（掩码全0），方便测试。
     * 
     * 如需过滤特定 ID，修改 filter_mask_high/low 为对应掩码值。
     * 
     * 32位掩码模式工作原理：
     * - 接收 ID 先与 filter_mask 按位与
     * - 再与 filter_list 比较
     * - 相等则通过，进入 FIFO
     * - mask = 0x0000 时，任何 ID 都通过（因为与0比较总是相等）
     */
    can_struct_para_init(CAN_FILTER_STRUCT, &filterParam);  /* 填充默认值 */

    filterParam.filter_number       = 0;                    /* 过滤器编号 0（CAN0 可用 0~14） */
    filterParam.filter_mode         = CAN_FILTERMODE_MASK;  /* 掩码模式（不是列表模式） */
    filterParam.filter_bits         = CAN_FILTERBITS_32BIT; /* 32位宽度（不是16位） */
    filterParam.filter_list_high    = 0x0000;               /* 期望 ID 高16位 */
    filterParam.filter_list_low     = 0x0000;               /* 期望 ID 低16位 */
    filterParam.filter_mask_high    = 0x0000;               /* 掩码高16位：全0 = 忽略 */
    filterParam.filter_mask_low     = 0x0000;               /* 掩码低16位：全0 = 忽略 */
    filterParam.filter_fifo_number  = CAN_FIFO0;            /* 匹配的消息进入 FIFO0 */
    filterParam.filter_enable       = ENABLE;               /* 使能此过滤器 */

    can_filter_init(&filterParam);  /* 应用过滤器配置 */
}

/**
 * @brief  发送 CAN 标准数据帧
 * @param  ID:     标准标识符（11位，范围 0x000 ~ 0x7FF）
 * @param  Length: 数据长度（0 ~ 8，CAN 标准规定最大 8 字节）
 * @param  Data:   数据缓冲区指针（至少 Length 字节有效）
 * @retval 无
 * @note   
 *   发送流程：
 *   1. 填充发送消息结构体（can_trasnmit_message_struct）
 *   2. 调用 can_message_transmit() 写入发送邮箱
 *   3. 轮询等待发送完成（检查发送状态）
 *   
 *   超时机制：
 *   - 如果总线繁忙或没有 ACK，发送可能失败
 *   - 超时计数器防止无限阻塞
 *   - 超时后函数返回，但消息可能仍在发送中
 *   
 *   与 STM32 的区别：
 *   - STM32 用 CanTxMsg 结构体，GD32 用 can_trasnmit_message_struct
 *   - 字段名不同：tx_sfid（标准ID）, tx_efid（扩展ID）, tx_ff（帧格式）, tx_ft（帧类型）
 */
void MyCAN_Transmit(uint32_t ID, uint8_t Length, uint8_t *Data)
{
    /* 定义 GD32 发送消息结构体
     * 
     * 结构体字段说明：
     * tx_sfid: 标准格式标识符（11位）
     * tx_efid: 扩展格式标识符（29位，标准帧时置0）
     * tx_ff:   帧格式（CAN_FF_STANDARD 或 CAN_FF_EXTENDED）
     * tx_ft:   帧类型（CAN_FT_DATA 或 CAN_FT_REMOTE）
     * tx_dlen: 数据长度码（0~8）
     * tx_data: 数据数组（8字节）
     */
    can_trasnmit_message_struct txMsg;

    /* 邮箱编号：GD32 有 3 个发送邮箱（0,1,2），函数返回使用的邮箱号
     * 如果返回 CAN_NOMAILBOX（0x03），表示没有空邮箱
     */
    uint8_t mailbox;

    /* 超时计数器，防止死循环
     * 实际超时时间取决于 CPU 频率，这里约 100000 个时钟周期
     */
    uint32_t timeout = 0;

    /* 步骤1：填充发送结构体
     * 
     * 先调用初始化函数填充默认值，防止未初始化字段
     */
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &txMsg);

    /* 配置标准帧 ID（11位）
     * 使用 & CAN_SFID_MASK（0x7FF）确保不超出范围
     */
    txMsg.tx_sfid = ID & CAN_SFID_MASK;     /* 标准 ID */
    txMsg.tx_efid = 0;                       /* 扩展 ID 置0（标准帧不用） */
    txMsg.tx_ff = CAN_FF_STANDARD;           /* 标准帧格式（不是扩展帧） */
    txMsg.tx_ft = CAN_FT_DATA;               /* 数据帧（不是远程帧） */

    /* 限制数据长度不超过 8 字节（CAN 协议规定）
     * 如果 Length > 8，只发送前 8 字节
     */
    txMsg.tx_dlen = (Length > 8) ? 8 : Length;

    /* 拷贝数据到结构体
     * 如果 Data 为 NULL 或 Length 为 0，不拷贝（发送空帧或远程帧）
     */
    if (Data != NULL && Length > 0) {
        memcpy(txMsg.tx_data, Data, txMsg.tx_dlen);
    }

    /* 步骤2：发送消息
     * 
     * can_message_transmit() 自动选择一个空邮箱：
     * - 优先选择编号小的空邮箱
     * - 如果 3 个邮箱都忙，返回 CAN_NOMAILBOX
     * 
     * 发送启动后，CAN 外设自动处理：
     * - 位填充
     * - CRC 计算
     * - 仲裁（多个设备同时发送时，ID 小的优先）
     * - ACK 检测
     * - 自动重传（如果使能）
     */
    mailbox = can_message_transmit(CAN_USED_PERIPH, &txMsg);

    /* 步骤3：等待发送完成（阻塞轮询）
     * 
     * can_transmit_states() 返回当前状态：
     * - CAN_TRANSMIT_OK:     发送成功（收到 ACK）
     * - CAN_TRANSMIT_FAILED: 发送失败（错误过多）
     * - CAN_TRANSMIT_PENDING: 正在发送
     * - CAN_TRANSMIT_NOMAILBOX: 没有空邮箱
     * 
     * 与 STM32 的 CAN_TransmitStatus() 对应
     */
    while ((CAN_TRANSMIT_OK != can_transmit_states(CAN_USED_PERIPH, mailbox)) 
           && (timeout < 100000))
    {
        timeout++;
    }

    /* 循环结束时：
     * - 如果 timeout < 100000：发送成功
     * - 如果 timeout >= 100000：超时，可能发送失败或仍在发送
     * 
     * 实际应用建议检查超时并处理错误
     */
}

/**
 * @brief  检查接收 FIFO 是否有数据（STM32 风格）
 * @param  无
 * @retval 1: FIFO 中有消息待读取
 * @retval 0: FIFO 为空
 * @note   
 *   非阻塞查询，适合主循环轮询方式：
 *   
 *   if (MyCAN_ReceiveFlag()) {
 *       MyCAN_Receive(...);  // 读取数据
 *   }
 *   
 *   与 STM32 的 CAN_MessagePending() 对应
 */
uint8_t MyCAN_ReceiveFlag(void)
{
    /* can_receive_message_length_get() 返回 FIFO 中消息数量（0,1,2,3）
     * GD32 每个 FIFO 深度为 3 帧
     * 
     * 与 STM32 区别：
     * - STM32: CAN_MessagePending(CAN1, CAN_FIFO0) 返回数量
     * - GD32:  can_receive_message_length_get(CAN0, CAN_FIFO0) 返回数量
     */
    return (can_receive_message_length_get(CAN_USED_PERIPH, CAN_FIFO0) > 0) ? 1 : 0;
}

/**
 * @brief  从接收 FIFO 读取一帧数据（STM32 风格）
 * @param  ID:     输出参数，存储接收到的标准标识符
 * @param  Length: 输出参数，存储接收到的数据长度（0~8）
 * @param  Data:   输出参数，存储接收到的数据（至少 8 字节缓冲区）
 * @retval 无
 * @note   
 *   读取流程：
 *   1. 定义 GD32 接收结构体
 *   2. 调用 can_message_receive() 从 FIFO 读取
 *   3. 提取字段到输出参数
 *   
 *   重要：
 *   - 读取后 FIFO 自动释放（can_message_receive 内部已处理）
 *   - 如果 FIFO 为空时调用，读取结果不确定
 *   - 建议先调用 MyCAN_ReceiveFlag() 确认有数据
 *   
 *   与 STM32 的区别：
 *   - STM32 用 CanRxMsg 结构体
 *   - GD32 用 can_receive_message_struct，字段名不同
 */
void MyCAN_Receive(uint32_t *ID, uint8_t *Length, uint8_t *Data)
{
    /* GD32 接收结构体（对应 STM32 的 CanRxMsg）
     * 
     * 字段说明：
     * rx_sfid: 标准标识符（11位）
     * rx_efid: 扩展标识符（29位）
     * rx_ff:   帧格式（CAN_FF_STANDARD 或 CAN_FF_EXTENDED）
     * rx_ft:   帧类型（CAN_FT_DATA 或 CAN_FT_REMOTE）
     * rx_dlen: 数据长度（0~8）
     * rx_data: 数据数组（8字节）
     * rx_fi:   过滤器索引（0~27，表示通过哪个过滤器）
     */
    can_receive_message_struct rxMsg;

    /* 填充默认值，防止未初始化 */
    can_struct_para_init(CAN_RX_MESSAGE_STRUCT, &rxMsg);

    /* 从 FIFO0 读取一帧消息
     * 
     * 内部操作：
     * 1. 从 FIFO 输出邮箱读取数据到结构体
     * 2. 释放 FIFO（清除 RFOM 位，允许下一帧进入）
     * 
     * 与 STM32 的 CAN_Receive(CAN1, CAN_FIFO0, &RxMessage) 对应
     */
    can_message_receive(CAN_USED_PERIPH, CAN_FIFO0, &rxMsg);

    /* 提取标准 ID 到输出参数
     * 如果接收的是扩展帧，需要读取 rx_efid
     * 这里假设只接收标准帧（与发送对应）
     */
    if (ID != NULL) {
        *ID = rxMsg.rx_sfid;    /* 标准标识符 */
    }

    /* 提取数据长度 */
    if (Length != NULL) {
        *Length = rxMsg.rx_dlen;
    }

    /* 提取数据内容
     * 如果 Data 为 NULL，不拷贝（只读取 ID 和长度）
     * 拷贝长度以 rx_dlen 为准，最多 8 字节
     */
    if (Data != NULL && rxMsg.rx_dlen > 0) {
        memcpy(Data, rxMsg.rx_data, rxMsg.rx_dlen);
    }
}