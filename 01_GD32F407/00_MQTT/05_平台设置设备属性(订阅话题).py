import json
import time
import ssl
import re
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

# 属性设置下发 Topic（平台 -> 设备），使用通配符 # 匹配所有 request_id
# 格式：$oc/devices/{设备ID}/sys/properties/set/request_id={request_id}
sub_topic = f"$oc/devices/{device_id}/sys/properties/set/#"

# 属性设置响应 Topic 前缀（设备 -> 平台），实际 Topic 需拼接 request_id
# 格式：$oc/devices/{设备ID}/sys/properties/set/response/request_id={request_id}
rsp_topic_prefix = f"$oc/devices/{device_id}/sys/properties/set/response"

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

# ==================== 回调函数 ====================
def on_connect(client, userdata, flags, rc):
    """
    连接结果回调函数
    rc (return code): 0 表示连接成功，非 0 表示失败
    """
    if rc == 0:
        print("Connected to Huawei Cloud IoTDA")
        # 连接成功后立即订阅属性设置 Topic
        client.subscribe(sub_topic, qos=1)
        print(f"Subscribed to: {sub_topic}")
    else:
        print(f"Connection failed, code: {rc}")

def on_message(client, userdata, msg):
    """
    消息接收回调函数：处理平台下发的属性设置指令
    """
    topic = msg.topic
    payload = msg.payload.decode("utf-8")
    print(f"\n[Received] Topic: {topic}")
    print(f"[Received] Payload: {payload}")

    try:
        # 解析平台下发的 JSON 报文
        data = json.loads(payload)
        
        # 从 Topic 中提取 request_id，用于构造响应 Topic
        # Topic 格式示例：.../sys/properties/set/request_id=xxxx
        match = re.search(r"request_id=([^/]+)$", topic)
        if not match:
            print("Error: request_id not found in topic")
            return
        
        request_id = match.group(1)
        
        # 提取并应用属性值（根据物模型定义解析 services）
        # 示例：{"services": [{"service_id": "base", "properties": {"Switch": true, "Brightness": 80}}]}
        services = data.get("services", [])
        for svc in services:
            service_id = svc.get("service_id", "unknown")
            properties = svc.get("properties", {})
            
            # 在此处执行实际的硬件/业务逻辑（如控制 GPIO、更新状态等）
            print(f"  -> Service [{service_id}] properties update:")
            for prop_name, prop_value in properties.items():
                print(f"       {prop_name} = {prop_value}")
                # 实际应用中，在此处调用设备驱动接口设置属性
                # 例如：light.set_brightness(prop_value) 或 gpio.write(prop_value)
        
        # 构造响应报文，result_code: 0 表示成功
        response = {
            "result_code": 0,
            "result_desc": "Property set successfully"
        }
        
        # 发布响应到平台
        rsp_topic = f"{rsp_topic_prefix}/request_id={request_id}"
        rsp_msg = json.dumps(response)
        client.publish(rsp_topic, rsp_msg, qos=1)
        print(f"[Response] -> {rsp_topic}: {rsp_msg}")

    except json.JSONDecodeError as e:
        print(f"Error: Failed to parse JSON payload - {e}")
    except Exception as e:
        print(f"Error: Failed to process property set - {e}")

# 将回调函数绑定到客户端
client.on_connect = on_connect
client.on_message = on_message

# ==================== 建立连接 ====================
# connect() 发起 TCP 连接并发送 CONNECT 报文
# keepalive=300 表示每 300 秒发送一次心跳，维持长连接
client.connect(hostname, port, keepalive=300)

# 启动网络循环线程，在后台自动处理网络收发（心跳、重连、回调触发等）
client.loop_start()

# ==================== 主循环保持运行 ====================
# 属性设置是被动接收模式，主线程保持存活即可
print("Device is running, waiting for property set commands from platform...")
print("Press Ctrl+C to exit\n")

try:
    while True:
        # 可在此处添加设备状态维护、心跳上报或其他周期性任务
        time.sleep(1)

except KeyboardInterrupt:
    # 捕获 Ctrl+C 中断信号，优雅退出
    print("\nStopped by user")

finally:
    # 无论正常结束还是异常中断，都执行清理：
    # 1. loop_stop(): 停止后台网络线程
    # 2. disconnect(): 向服务器发送 DISCONNECT 报文，断开 TCP 连接
    client.loop_stop()
    client.disconnect()
    print("Disconnected")