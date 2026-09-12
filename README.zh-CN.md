# 使用 C 语言实现光线追踪

<p align="center">
  <a href="./README.md"><img alt="Read in English" src="https://img.shields.io/badge/English-README-0969da"></a>
  <a href="./README.zh-CN.md"><img alt="阅读简体中文版" src="https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-README-d73a49"></a>
</p>

<p align="center">
  <img alt="编程语言：C" src="https://img.shields.io/badge/language-C-00599C?logo=c&logoColor=white">
  <img alt="构建系统：CMake" src="https://img.shields.io/badge/build-CMake-064F8C?logo=cmake&logoColor=white">
  <a href="./LICENSE"><img alt="许可证：MIT" src="https://img.shields.io/badge/license-MIT-green.svg"></a>
</p>

本项目的灵感来自 YouTuber [**Daniel Hirsch**](https://www.bilibili.com/video/BV1Qq7m6PEuA?spm_id_from=333.1245.0.0) 及其视频 [Ray Tracing in C](https://www.youtube.com/watch?v=2BLRLuczykM)。

这是一个使用 C 语言编写的小型交互式光线追踪演示程序。它展示了可移动光源发出的光线、光线与障碍物和窗口边界的碰撞，以及反射后的传播路径。项目使用 [TIGR](https://github.com/erkkah/tigr) 完成窗口创建、图形绘制和鼠标输入。

## 演示

<video src="./ray_tracing_c_demo.mp4" controls width="100%">
  当前浏览器不支持嵌入视频，请通过下面的链接观看演示。
</video>

[▶ 观看演示视频](./ray_tracing_c_demo.mp4)

## 功能

- 从点光源向各个方向发射光线。
- 计算光线在窗口边界和圆形障碍物上的反射。
- 使用 PCA 根据附近边界像素估算障碍物表面法线。
- 每次反射后降低光线强度，模拟能量损失。
- 按住鼠标左键拖动光源。
- 按住鼠标右键拖动障碍物。

## 为什么适合作为 C 语言练手项目

项目规模不大，无需理解复杂框架即可阅读完整代码，同时包含了多个实用知识点：

- C 语言结构体、函数、指针、循环和数值计算。
- 二维向量、点积、协方差矩阵和反射几何。
- 使用轻量级图形库完成绘图和鼠标输入。
- 使用 CMake 管理跨平台构建。
- 处理 Windows 和 Linux 平台不同的编译与链接依赖。

相比纯命令行练习，这个项目可以直观展示计算结果，修改数学算法后也能立即看到效果。

## 实现原理

1. 光源在完整圆周内均匀发射多条光线。
2. 每条光线以接近一个像素的步长在屏幕上前进。
3. 光线到达窗口边界时，反转对应方向上的分量。
4. 光线碰到障碍物时，程序采样碰撞点附近的边界像素，并通过协方差和主成分分析（PCA）估算表面法线。
5. 使用以下公式计算反射方向：

   ```text
   reflected = incident - 2 * dot(incident, normal) * normal
   ```

6. 每次反射后降低光线的不透明度，以简单模拟能量损失。

这是一个用于学习光线反射的二维可视化项目，并非基于物理的三维渲染器。

## 环境要求

- 支持 C99 的 C 编译器
- CMake 3.10 或更高版本
- 下文列出的平台图形开发库

## 在 Linux（Ubuntu）上构建

安装编译器、CMake、OpenGL、GLU 和 X11 开发包：

```bash
sudo apt update
sudo apt install build-essential cmake libgl1-mesa-dev libglu1-mesa-dev libx11-dev
```

> [!IMPORTANT]
> 在 Linux 上编译前，请确保 `Process_Mouse_Evevt` 中使用以下右键判断：
>
> ```c
> if (mouse->button == 2)
> ```
>
> 项目内置的 TIGR 后端使用了不同的按键位：Linux/X11 将右键映射为 `2`、中键映射为 `4`，而 Windows 将中键映射为 `2`、右键映射为 `4`。

创建构建目录，使用 CMake 生成 Makefile，然后编译并运行项目：

```bash
mkdir build
cd build
cmake ..
make
./raytracer
```

Linux 目标会链接 `GLU`、`GL`、`X11` 和数学库 `m`。

## 在 Windows 上构建

建议在 Windows 上使用 Visual Studio 构建本项目：

> [!IMPORTANT]
> 在 Windows 上编译前，请确保 `Process_Mouse_Evevt` 中使用以下右键判断：
>
> ```c
> if (mouse->button == 4)
> ```
>
> 该值与 Linux 不同，因为项目内置的 TIGR Windows 后端为鼠标右键和中键使用了不同的按键位。

1. 安装 Visual Studio，并勾选 **使用 C++ 的桌面开发** 工作负载和 CMake 工具。
2. 打开 Visual Studio，选择 **打开本地文件夹**。
3. 选择包含 `CMakeLists.txt` 的项目文件夹。
4. 等待 Visual Studio 自动检测 CMake 项目并完成配置。
5. 将 `raytracer.exe` 设为启动项，然后直接在 Visual Studio 中构建或运行项目。

## 操作方法

| 操作 | 控制方式 |
| --- | --- |
| 移动光源 | 按住鼠标左键并拖动 |
| 移动障碍物 | 按住鼠标右键并拖动 |
| 退出 | 关闭窗口 |

## 可调参数简介

以下宏定义位于 `ray_tracing.c` 顶部，可用于调整光线数量、反射效果和法线估算：

| 参数 | 默认值 | 说明 |
| --- | ---: | --- |
| `RAY_NUM` | `360` | 光源在 360° 范围内发射的光线数量。数值越大，光线分布越密集、画面越细致，但计算量也会增加。 |
| `REF_NUM` | `1` | 光线的反射深度参数。增大该值可以继续处理更多次碰撞与反射，但会增加计算量。 |
| `SAMPLE_RADIUS` | `10` | 使用 PCA 估算障碍物表面法线时，碰撞点周围的采样半径。较大的值通常能让法线更平滑，但会扩大采样范围并增加计算量。 |
| `BORDER_ALBEDO` | `0.75` | 光线在窗口边界反射后的强度保留比例。取值越小，光线在边界反射后衰减得越快。 |
| `OBSTACLE_ALBEDO` | `0.75` | 光线在障碍物表面反射后的强度保留比例。取值越小，光线在障碍物上反射后衰减得越快。 |

反照率参数建议设置在 `0.0` 到 `1.0` 之间：`1.0` 表示不衰减，`0.0` 表示反射后完全不可见。

## 项目结构

```text
.
├── CMakeLists.txt
├── LICENSE
├── README.md
├── README.zh-CN.md
├── ray_tracing.c
├── ray_tracing_c_demo.mp4
└── tigr/
    ├── tigr.c
    └── tigr.h
```

## 开源协议

本项目使用 [MIT License](./LICENSE) 开源。
