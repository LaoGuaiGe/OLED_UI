#!/usr/bin/env python3
"""
字库串口烧录工具（ACK 握手协议）

协议：
  1. 打开串口，等待固件发送 "READY" 字样
  2. 发送启动字节 0x06（告诉固件进入握手模式）
  3. 每次发送 256 字节
  4. 等待固件回复 ACK '.'（表示页写入完成）
  5. 发送下一页，直到文件发完

用法:
  python flash_sender.py <bin文件> [串口] [波特率]

示例:
  python flash_sender.py ../output/ALL_FONTS.bin COM3 115200
  python flash_sender.py ../output/HZK20.bin COM3 115200
"""

import sys
import os
import time
import serial

PAGE_SIZE = 256
ACK_BYTE = b'.'
START_BYTE = b'\x06'
ACK_TIMEOUT = 5.0  # 等待 ACK 超时秒数（页写入+擦除最慢不超过这个）


def find_ready(ser):
    """等待固件发送 READY 提示"""
    print("等待设备就绪 (READY)...")
    buf = b""
    while True:
        ch = ser.read(1)
        if ch:
            buf += ch
            # 打印固件输出（ID、擦除进度等）
            try:
                sys.stdout.write(ch.decode('ascii', errors='replace'))
                sys.stdout.flush()
            except:
                pass
            if b"READY" in buf[-64:]:
                # 读完这一行
                while True:
                    ch = ser.read(1)
                    if ch:
                        try:
                            sys.stdout.write(ch.decode('ascii', errors='replace'))
                            sys.stdout.flush()
                        except:
                            pass
                        if ch == b'\n':
                            break
                    else:
                        break
                return True


def send_file(ser, filepath):
    """ACK 握手方式发送文件"""
    filesize = os.path.getsize(filepath)
    total_pages = (filesize + PAGE_SIZE - 1) // PAGE_SIZE

    print(f"\n文件: {os.path.basename(filepath)}")
    print(f"大小: {filesize} 字节 ({filesize/1024:.1f} KB)")
    print(f"总页数: {total_pages}")
    print(f"预计时间: ~{total_pages * 0.025:.0f} 秒\n")

    # 发送启动字节，进入握手模式
    ser.write(START_BYTE)
    time.sleep(0.01)

    with open(filepath, "rb") as f:
        page_num = 0
        start_time = time.time()

        while True:
            chunk = f.read(PAGE_SIZE)
            if not chunk:
                break

            # 发送一页数据
            ser.write(chunk)
            page_num += 1

            # 等待 ACK
            ack = ser.read(1)
            if ack != ACK_BYTE:
                if not ack:
                    print(f"\n错误: 第 {page_num} 页等待 ACK 超时!")
                else:
                    print(f"\n错误: 第 {page_num} 页收到非预期回复: {ack!r}")
                return False

            # 进度显示
            elapsed = time.time() - start_time
            pct = page_num / total_pages * 100
            speed = (page_num * PAGE_SIZE) / elapsed / 1024 if elapsed > 0 else 0
            remaining = (elapsed / page_num) * (total_pages - page_num) if page_num > 0 else 0
            print(f"\r  [{pct:5.1f}%] {page_num}/{total_pages} 页 | "
                  f"{speed:.1f} KB/s | 剩余 {remaining:.0f}s", end="", flush=True)

    elapsed = time.time() - start_time
    print(f"\n\n发送完成! 耗时 {elapsed:.1f} 秒, "
          f"平均 {filesize/elapsed/1024:.1f} KB/s")

    # 读取固件 Done 消息
    time.sleep(0.5)
    remaining = ser.read(ser.in_waiting or 256)
    if remaining:
        print(f"设备: {remaining.decode('ascii', errors='replace').strip()}")

    return True


def main():
    if len(sys.argv) < 2:
        print("用法: python flash_sender.py <bin文件> [串口] [波特率]")
        print("示例: python flash_sender.py ../output/ALL_FONTS.bin COM3 115200")
        sys.exit(1)

    filepath = sys.argv[1]
    port = sys.argv[2] if len(sys.argv) > 2 else "COM3"
    baud = int(sys.argv[3]) if len(sys.argv) > 3 else 115200

    if not os.path.isfile(filepath):
        print(f"错误: 文件不存在 {filepath}")
        sys.exit(1)

    print(f"串口: {port} @ {baud}")
    print(f"文件: {filepath}")
    print("-" * 50)

    try:
        ser = serial.Serial(port, baud, timeout=ACK_TIMEOUT)
    except serial.SerialException as e:
        print(f"错误: 无法打开串口 {port}: {e}")
        sys.exit(1)

    try:
        find_ready(ser)
        success = send_file(ser, filepath)
        if not success:
            print("\n烧录失败!")
            sys.exit(1)
    except KeyboardInterrupt:
        print("\n\n用户中断")
    finally:
        ser.close()


if __name__ == "__main__":
    main()
