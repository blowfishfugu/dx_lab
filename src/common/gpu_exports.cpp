#pragma once
#ifndef GPU_EXPORTS
#define GPU_EXPORTS
//for notebooks, choose graphicscard instead of embedded intel-graphics.
extern "C" {
       _declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
       _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#endif