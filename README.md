# Assault Cube Base

This project aims to analyse, reverse engineer and manipulate the game Assault Cube.
Then, we create an working PoC with multiples exploits in an injectable dll.

## How does it work?

We did a lot of reverse engineering to be able to understand the code of the game.
We then use Windows API to modify its behavior in our favor.

#### Hack Environment :
- We wanted the game to run the hack itself before every frame, we identified the function [wglSwapBuffers](https://learn.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-swapbuffers) which run before the drawing of each frame. The hack is in a trampoline hook to have perfect accuracy.

#### Triggerbot :
- We identified and detoured a call to TraceRay, this hook allow use to know everytime an entity is on our crosshair and we even get the ent id.
- We use the SendInput function to click

#### Aimbot :
- We used Reclass to reverse the Player class, the headPos is at offset 0x04
- With Cheat Engine we figured the entityList adress
- The hack loop through entityList, figure out the closest player, and then assign the new angle.

## Getting Started

### Installation

The project is a Visual Studio DLL solution, it's a Windows only project and the only prerequisite is Windows API.
The project needs to be compiled in x86 to have the same ABI as Assault Cube

### Usage

The project doesn't have its own injector at the moment, you can inject it inside Assault Cube using any DLL Injector (e.g [ProcessHacker](https://processhacker.sourceforge.io/)
The triggerbot is always enabled and aimbot is activated with Mouse5 (Mouse Extra Left Button)

> Made with ❤️ by Nestyles