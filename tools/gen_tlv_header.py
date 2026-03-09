#!/usr/bin/env python3
"""Embed a TLV binary as a C const array header."""
import sys

def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <tlv_file> <array_name>", file=sys.stderr)
        sys.exit(1)
    tlv_file, array_name = sys.argv[1], sys.argv[2]
    data = open(tlv_file, 'rb').read()
    print('#pragma once')
    print('#include <stdint.h>')
    print(f'#define {array_name}_LEN {len(data)}u')
    print(f'static const uint8_t {array_name}[] = {{')
    for i, b in enumerate(data):
        if i % 16 == 0:
            print('    ', end='')
        print(f'0x{b:02x}', end='')
        if i < len(data) - 1:
            print(', ', end='')
        if (i + 1) % 16 == 0:
            print()
    print('\n};')

if __name__ == '__main__':
    main()
