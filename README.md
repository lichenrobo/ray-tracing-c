# Ray Tracing in C

<p align="center">
  <a href="./README.md"><img alt="Read in English" src="https://img.shields.io/badge/English-README-0969da"></a>
  <a href="./README.zh-CN.md"><img alt="阅读简体中文版" src="https://img.shields.io/badge/%E7%AE%80%E4%BD%93%E4%B8%AD%E6%96%87-README-d73a49"></a>
</p>

<p align="center">
  <img alt="Language: C" src="https://img.shields.io/badge/language-C-00599C?logo=c&logoColor=white">
  <img alt="Build system: CMake" src="https://img.shields.io/badge/build-CMake-064F8C?logo=cmake&logoColor=white">
  <a href="./LICENSE"><img alt="License: MIT" src="https://img.shields.io/badge/license-MIT-green.svg"></a>
</p>

This project was inspired by YouTuber [**Daniel Hirsch**](https://www.bilibili.com/video/BV1Qq7m6PEuA?spm_id_from=333.1245.0.0) and his video [Ray Tracing in C](https://www.youtube.com/watch?v=2BLRLuczykM).

This is a small interactive ray-tracing demo written in C. It visualizes rays emitted by a movable light source, their collisions with an obstacle and the window boundaries, and their reflected paths. The project uses [TIGR](https://github.com/erkkah/tigr) for window creation, drawing, and mouse input.

## Demo

<video src="./ray_tracing_c_demo.mp4" controls width="100%">
  Your browser does not support embedded video. Open the link below to watch the demo.
</video>

[▶ Watch the demo video](./ray_tracing_c_demo.mp4)

## Features

- Emits rays in all directions from a point light source.
- Reflects rays from the window boundaries and a circular obstacle.
- Estimates obstacle surface normals from nearby boundary pixels using PCA.
- Reduces ray intensity after each reflection to simulate energy loss.
- Lets you move the light source with the left mouse button.
- Lets you move the obstacle with the right mouse button.

## Why This Is a Good C Practice Project

The code is compact enough to understand without a large framework, while still bringing together several useful topics:

- C structs, functions, pointers, loops, and numerical calculations.
- 2D vectors, dot products, covariance matrices, and reflection geometry.
- A lightweight graphics library for drawing and mouse input.
- A cross-platform CMake configuration.
- Platform- and link-time dependencies on Windows and Linux.

It is a practical next step after command-line exercises because the results are visual and changes to the math can be seen immediately.

## How It Works

1. The light source emits evenly spaced rays over a full circle.
2. Each ray advances across the screen one pixel-sized step at a time.
3. When a ray reaches a window boundary, the corresponding direction component is reversed.
4. When a ray reaches the obstacle, nearby boundary pixels are sampled. Their covariance is used to estimate the surface normal with principal component analysis (PCA).
5. The reflected direction is calculated with:

   ```text
   reflected = incident - 2 * dot(incident, normal) * normal
   ```

6. The ray opacity is reduced after a reflection, giving a simple approximation of energy loss.

This is an educational 2D visualization of ray reflection rather than a physically based 3D renderer.

## Requirements

- A C99-compatible compiler
- CMake 3.10 or newer
- Platform graphics development libraries listed below

## Build on Linux (Ubuntu)

Install the compiler, CMake, OpenGL, GLU, and X11 development packages:

```bash
sudo apt update
sudo apt install build-essential cmake libgl1-mesa-dev libglu1-mesa-dev libx11-dev
```

> [!IMPORTANT]
> Before building on Linux, use the following right-mouse-button check in `Process_Mouse_Evevt`:
>
> ```c
> if (mouse->button == 2)
> ```
>
> The bundled TIGR backends use different button bit values: Linux/X11 maps the right button to `2` and the middle button to `4`, while Windows maps the middle button to `2` and the right button to `4`.

Create a build directory, generate the Makefile with CMake, and compile and run the project:

```bash
mkdir build
cd build
cmake ..
make
./raytracer
```

The Linux target links against `GLU`, `GL`, `X11`, and the math library `m`.

## Build on Windows

Visual Studio is the recommended way to build the project on Windows:

> [!IMPORTANT]
> Before building on Windows, make sure `Process_Mouse_Evevt` uses the following right-mouse-button check:
>
> ```c
> if (mouse->button == 4)
> ```
>
> The value differs from Linux because the bundled TIGR Windows backend maps the right and middle mouse buttons to different bit values.

1. Install Visual Studio with the **Desktop development with C++** workload and the CMake tools.
2. Open Visual Studio and select **Open a local folder**.
3. Select the project folder containing `CMakeLists.txt`.
4. Wait for Visual Studio to detect the CMake project and finish configuring it automatically.
5. Select `raytracer.exe` as the startup item, then build or run the project from Visual Studio.

## Controls

| Action | Control |
| --- | --- |
| Move the light source | Hold the left mouse button and drag |
| Move the obstacle | Hold the right mouse button and drag |
| Exit | Close the window |

## Adjustable Parameters

The following macros are defined near the top of `ray_tracing.c`. They control the ray count, reflection behavior, and surface-normal estimation:

| Parameter | Default | Description |
| --- | ---: | --- |
| `RAY_NUM` | `360` | Number of rays emitted around the light source. A higher value produces denser, more detailed rays but requires more computation. |
| `REF_NUM` | `1` | Reflection-depth setting for each ray. Increasing it allows more collisions and reflections to be processed, at the cost of additional computation. |
| `SAMPLE_RADIUS` | `10` | Sampling radius around a collision point when PCA is used to estimate the obstacle's surface normal. A larger value can produce smoother normals, but samples a wider area and requires more computation. |
| `BORDER_ALBEDO` | `0.75` | Fraction of ray intensity retained after reflection from a window boundary. Lower values make boundary reflections fade faster. |
| `OBSTACLE_ALBEDO` | `0.75` | Fraction of ray intensity retained after reflection from the obstacle. Lower values make obstacle reflections fade faster. |

The albedo values should normally stay between `0.0` and `1.0`: `1.0` preserves the full intensity, while `0.0` makes the ray fully transparent after reflection.

## Project Structure

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

## License

This project is released under the [MIT License](./LICENSE).
