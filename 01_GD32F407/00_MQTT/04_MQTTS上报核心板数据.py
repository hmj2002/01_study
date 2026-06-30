import json
import time
import ssl
import serial
import paho.mqtt.client as mqtt
from typing import Optional, Tuple

# ==================== 串口配置 ====================
SERIAL_PORT = "COM14"         # 根据设备管理器修改你的串口号
BAUDRATE    = 115200
TIMEOUT     = 1

# ==================== 华为云 IoTDA 连接参数 ====================
client_id = "6a43b3f5e094d615924dd6bc_test001_0_0_2026063013"
username  = "6a43b3f5e094d615924dd6bc_test001"
device_id = username
password  = "4d14d1f25a13f0ce27c6edc472699e30af3eda920144d05c2df7bfa556d4f4f5"
hostname  = "0566b5c4e8.st1.iotda-device.cn-east-3.myhuaweicloud.com"
port      = 8883
pub_topic = f"$oc/devices/{device_id}/sys/properties/report"

# ==================== 初始化串口 ====================
print(f"[Serial] Opening {SERIAL_PORT} @ {BAUDRATE}...")
try:
    ser = serial.Serial(
        port=SERIAL_PORT,
        baudrate=BAUDRATE,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=TIMEOUT
    )
    print(f"[Serial] Opened successfully: {ser.name}")
except Exception as e:
    print(f"[Serial] Error: {e}")
    exit(1)

# ==================== 初始化 MQTT 客户端 ====================
try:
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1, client_id=client_id)
except AttributeError:
    client = mqtt.Client(client_id)

client.username_pw_set(username=username, password=password)
client.tls_set_context(ssl.create_default_context())

# ==================== 回调函数 ====================
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("[MQTT] Connected to Huawei Cloud IoTDA")
    else:
        print(f"[MQTT] Connection failed, code: {rc}")

client.on_connect = on_connect

# ==================== 建立 MQTT 连接 ====================
client.connect(hostname, port, keepalive=300)
client.loop_start()
time.sleep(1)

# ==================== 解析串口数据 ====================
def parse_sensor_data(raw_line: bytes) -> Tuple[Optional[float], Optional[float]]:
    """
    解析 GD32 发送的串口数据
    格式: TEMP:25.50,HUMI:60
    返回: (temperature, humidity) 或 (None, None) 解析失败
    """
    try:
        line = raw_line.decode("utf-8").strip()
        if not line.startswith("TEMP:"):
            return None, None

        body = line.replace("TEMP:", "")
        parts = body.split(",")
        if len(parts) != 2:
            return None, None

        temp = float(parts[0])
        humi = float(parts[1].replace("HUMI:", ""))
        return temp, humi
    except Exception as e:
        print(f"[Parse] Error: {e}, raw: {raw_line}")
        return None, None

# ==================== 主循环：串口读取 → 云端上报 ====================
print("[Main] Waiting for sensor data from GD32...")
print("[Main] Press Ctrl+C to exit\n")

try:
    while True:
        raw_line = ser.readline()
        if not raw_line:
            continue

        temp, humi = parse_sensor_data(raw_line)
        if temp is None or humi is None:
            continue

        print(f"[Serial] Temp={temp:.2f}C, Humi={humi:.2f}%")

        payload = {
            "services": [{
                "service_id": "base",
                "properties": {
                    "Temperature": temp,
                    "Humidity": humi
                }
            }]
        }

        msg = json.dumps(payload)
        client.publish(pub_topic, msg, qos=1)
        print(f"[MQTT] Published: {msg}\n")

except KeyboardInterrupt:
    print("\n[Main] Stopped by user")
except serial.SerialException as e:
    print(f"\n[Serial] Disconnected: {e}")
finally:
    ser.close()
    client.loop_stop()
    client.disconnect()
    print("[Main] Clean exit")