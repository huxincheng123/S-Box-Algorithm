# S-box Affine Equivalence Checker

## English

### Overview
`main.cpp` checks affine equivalence between pairs of S-boxes. It reads multiple test cases, computes ranks, and if equivalent prints the affine transforms (L1/L2) and the affine shifts (c1/c2).

## Target Platform
- **Linux**（GCC ≥ 7 / Clang ≥ 6）
- Windows users are recommended to run the program inside WSL2 or a Docker container for full compatibility.

### Build & Run
- Build: `make -f Makefiles.txt -j`
- Run: `./sbox_eq`
- Input source: by default `main` uses `freopen("data_703.txt", "r", stdin);` and logs to `result.txt`. Change the filename in `main.cpp` or comment out `freopen` to read from stdin.
- Example run with a file: `./sbox_eq` (set `freopen` to `[your_sbox_data].txt`).
- Output goes to both the terminal and `result.txt` (via the dual printf wrapper). Change `g_log_file = fopen("result.txt", "w");` if you want a different log target.

### Data Format
- Line 1: integer `T`, number of test cases.
- For each test case:
  - Line: integer `n`, the bit-width of the S-box (e.g., 8 in `data_703.txt`).
  - Line: `2^n` integers for the first S-box mapping (values in `[0, 2^n - 1]`).
  - Line: `2^n` integers for the second S-box mapping (same range).
- `data_703.txt` follows this format with `T = 703` and `n = 8` in each test.

### Outputs
- For each test pair: prints ranks of both S-boxes, whether they are affine equivalent, the S-box tables, L1/L2 matrices, and the recovered 8-bit binary c1/c2. Each test block ends with a separator line.
- After all tests: prints total time and number of successful equivalence checks.

---

## 中文

### 概览
`main.cpp` 用于判断一对 S-box 是否仿射等价，读取多组测试，计算秩并在等价时输出仿射变换 L1/L2 以及平移 c1/c2。

## 运行平台
- **Linux**（GCC ≥ 7 / Clang ≥ 6）
- Windows 用户请使用 WSL2 或 Docker 容器，以保证兼容性与性能。

### 编译与运行
- 编译：`make -f Makefiles.txt -j`
- 运行：`./sbox_eq`
- 输入来源：默认在 `main` 中 `freopen("data_703.txt", "r", stdin);`，并将输出写入 `result.txt`。可修改该文件名或注释掉 `freopen` 以改用标准输入。
- 文件运行示例：`./sbox_eq`（把 `freopen` 路径改成 `[你的s盒数据文件].txt`）。
- 输出同时写到终端和 `result.txt`（通过自定义 printf）。如需更改日志目标，修改或删除 `g_log_file = fopen("result.txt", "w");`。

### 数据格式
- 第 1 行：整数 `T`，表示测试组数。
- 每组测试包含：
  - 1 行：整数 `n`，S-box 位宽（如 `data_703.txt` 中为 8）。
  - 1 行：`2^n` 个整数，第一组 S-box 映射值（范围 `[0, 2^n - 1]`）。
  - 1 行：`2^n` 个整数，第二组 S-box 映射值（同范围）。
- `data_703.txt` 按此格式编写，`T = 703`，每组 `n = 8`。

### 输出说明
- 每组测试会打印两侧 S-box 的秩、是否仿射等价、S-box 表、L1/L2 矩阵及 8 位二进制的 c1/c2，并以分隔线结束该组输出。
- 所有测试结束后会输出耗时与成功次数。
