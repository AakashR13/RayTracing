-- premake5.lua
workspace "Halide"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Halide"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
include "Walnut/WalnutExternal.lua"

include "Halide"