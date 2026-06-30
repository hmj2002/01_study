import json
import time
import ssl
import paho.mqtt.client as mqtt

# ==================== 连接参数（生产环境请使用环境变量或密钥管理服务，不要明文硬编码） ====================
# client_id 格式：{设备ID}_0_0_{时间戳}，华为云 IoTDA 要求此格式
client_id = "6a43b3f5e094d615924dd6bc_test001_0_0_2026063013"
# 用户名固定为设备 ID
username  = "6a43b3f5e094d615924dd6bc_test001"
device_id = username
# 密码为设备密钥，在 IoTDA 控制台创建设备时生成
password  = "4d14d1f25a13f0ce27c6edc472699e30af3eda920144d05c2df7bfa556d4f4f5"
# 华为云 IoTDA 平台接入域名，在控制台"总览"页获取
hostname  = "0566b5c4e8.st1.iotda-device.cn-east-3.myhuaweicloud.com"
# MQTTS端口
port      = 8883

# 属性上报 Topic，固定格式：$oc/devices/{设备ID}/sys/properties/report
# 平台订阅此 Topic 以接收设备上报的物模型属性数据
pub_topic = f"$oc/devices/{device_id}/sys/properties/report"

# ==================== 初始化 MQTT 客户端 ====================
# paho-mqtt 2.x 版本引入了 CallbackAPIVersion 参数，与 1.x 不兼容
# 此处通过 try/except 实现版本自适应，确保代码在两种版本下都能运行
try:
    # 2.x 版本写法：必须显式指定回调 API 版本
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, client_id=client_id)
except AttributeError:
    # 1.x 版本写法：无 CallbackAPIVersion 参数
    client = mqtt.Client(client_id)

# 设置连接时的用户名和密码，对应华为云 IoTDA 设备的身份认证信息
client.username_pw_set(username=username, password=password)

# ==================== 启用 TLS/SSL ====================
# 使用系统默认的 CA 证书验证服务器身份
client.tls_set_context(ssl.create_default_context())

# 如果 paho-mqtt 版本较旧，也可以简写为：
# client.tls_set()

# ==================== 回调函数 ====================
def on_connect(client, userdata, flags, rc):
    """
    连接结果回调函数
    rc (return code): 0 表示连接成功，非 0 表示失败
    常见错误码：1-协议版本错误 2-无效客户端ID 3-服务器不可用 4-用户名密码错误 5-未授权
    """
    if rc == 0:
        print("Connected to Huawei Cloud IoTDA")
    else:
        print(f"Connection failed, code: {rc}")

# 将回调函数绑定到客户端
client.on_connect = on_connect

# ==================== 建立连接 ====================
# connect() 发起 TCP 连接并发送 CONNECT 报文
# keepalive=300 表示每 300 秒发送一次心跳，维持长连接
client.connect(hostname, port, keepalive=300)

# 启动网络循环线程，在后台自动处理网络收发（心跳、重连、回调触发等）
client.loop_start()

# 等待 1 秒，确保 on_connect 回调执行完毕，连接状态就绪
time.sleep(1)

# ==================== 循环上报属性 ====================
try:
    for i in range(10):
        # 构造物模型属性上报报文，必须符合华为云 IoTDA 的 JSON 格式
        # services: 服务列表；service_id: 产品模型中的服务 ID
        # properties: 该服务下的属性键值对，必须与控制台定义的物模型一致
        payload = {
            "services": [{
                "service_id": "base",               # 对应产品模型中的服务名称
                "properties": {
                    "Temperature": 20.5 + i,        # 温度属性，数值型
                    "Humidity": 10 + i              # 湿度属性，数值型
                }
            }]
        }
        
        # 将 Python 字典序列化为 JSON 字符串
        msg = json.dumps(payload)
        
        # 发布消息到属性上报 Topic
        # qos=1: 至少送达一次，平台会回复 PUBACK 确认，适合关键业务数据
        # 若允许偶发丢失可改为 qos=0，减少网络开销
        client.publish(pub_topic, msg, qos=1)
        print(f"Published [{i}]: {msg}")
        
        # 每 2 秒上报一次，避免触发平台流控
        time.sleep(2)

except KeyboardInterrupt:
    # 捕获 Ctrl+C 中断信号，优雅退出
    print("Stopped by user")

finally:
    # 无论正常结束还是异常中断，都执行清理：
    # 1. loop_stop(): 停止后台网络线程
    # 2. disconnect(): 向服务器发送 DISCONNECT 报文，断开 TCP 连接
    client.loop_stop()
    client.disconnect()
    print("Disconnected")