#!/usr/bin/env python3
"""
合并字库工具：将 HZK16 + U2G_MAP + HZK12 + HZK20 按 Flash 地址对齐拼成一个文件，
用于串口一次性烧录。中间空隙用 0xFF 填充（与已擦除 Flash 一致，写入等于空操作）。

Flash 地址分配：
  0x000000  HZK16      261698 字节
  0x040000  U2G_MAP     41984 字节
  0x050000  HZK12      196274 字节
  0x080000  HZK20      490682 字节

输出文件总大小: 0x080000 + 490682 = 1,014,970 字节 (~991KB)

用法: python merge_fonts.py
输出: ../output/ALL_FONTS.bin
"""

import os
import sys

# 各段配置: (文件名, Flash起始地址, 期望大小)
SEGMENTS = [
    ("HZK16.bin",              0x000000, 261698),
    ("unicode_gb2312_map.bin", 0x040000, 41984),
    ("HZK12.bin",              0x050000, 196274),
    ("HZK20.bin",              0x080000, 490682),
]

def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_dir = os.path.join(script_dir, "..", "output")

    # 计算总大小 = 最后一段起始 + 最后一段大小
    last = SEGMENTS[-1]
    total_size = last[1] + last[2]

    # 初始化为全 0xFF（等于已擦除的 Flash）
    merged = bytearray(b'\xFF' * total_size)

    for filename, start_addr, expected_size in SEGMENTS:
        path = os.path.join(output_dir, filename)
        if not os.path.isfile(path):
            print(f"错误: 找不到 {path}")
            sys.exit(1)

        with open(path, "rb") as f:
            data = f.read()

        if len(data) != expected_size:
            print(f"警告: {filename} 大小 {len(data)} != 期望 {expected_size}")

        # 写入对应偏移
        merged[start_addr:start_addr + len(data)] = data
        print(f"  {filename:30s}  @ 0x{start_addr:06X}  {len(data):>7d} 字节")

    out_path = os.path.join(output_dir, "ALL_FONTS.bin")
    with open(out_path, "wb") as f:
        f.write(merged)

    print(f"\n合并完成: {out_path}")
    print(f"总大小: {len(merged)} 字节 ({len(merged)/1024:.1f} KB)")
    print(f"115200 baud 约 {len(merged)*10/115200:.0f} 秒")

if __name__ == "__main__":
    main()
