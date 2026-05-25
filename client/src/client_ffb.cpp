#include "client_ffb.h"
#include <stdio.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0800

#define Rectangle NATIVE_Rectangle
#define CloseWindow NATIVE_CloseWindow
#define ShowCursor NATIVE_ShowCursor

#include <dinput.h>
#include <windows.h>

#undef Rectangle
#undef CloseWindow
#undef ShowCursor

extern "C" {
void *GetWindowHandle(void);
}

#ifndef DIPROPLONG
typedef struct DIPROPLONG {
  DIPROPHEADER diph;
  LONG lData;
} DIPROPLONG, *LPDIPROPLONG;
#endif

static LPDIRECTINPUTDEVICE8 g_pDevice = nullptr;
static LPDIRECTINPUTEFFECT g_pEffect = nullptr;
static DWORD g_dwSteerAxisID = 0;

BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef) {
  if (lpddoi->guidType == GUID_XAxis) {
    g_dwSteerAxisID = lpddoi->dwType;
    return DIENUM_STOP;
  }
  return DIENUM_CONTINUE;
}
#endif

void ffb_init() {
#if defined(_WIN32)
  LPDIRECTINPUT8 pDI = nullptr;

  if (FAILED(
          DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID *) &pDI, NULL))) {
    return;
  }

  pDI->EnumDevices(
      DI8DEVCLASS_GAMECTRL,
      [](LPCDIDEVICEINSTANCE pInst, LPVOID Ctx) -> BOOL {
        ((LPDIRECTINPUT8) Ctx)->CreateDevice(pInst->guidInstance, &g_pDevice, NULL);
        return DIENUM_STOP;
      },
      pDI, DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);

  if (!g_pDevice) {
    pDI->Release();
    return;
  }

  g_pDevice->SetDataFormat(&c_dfDIJoystick);

  HWND hwnd = (HWND) GetWindowHandle();
  if (FAILED(g_pDevice->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_EXCLUSIVE))) {
    pDI->Release();
    return;
  }

  g_pDevice->Acquire();
  g_pDevice->EnumObjects(EnumAxesCallback, NULL, DIDFT_AXIS);

  DWORD rgdwAxes[1] = {g_dwSteerAxisID};
  LONG rglDirection[1] = {0};
  DICONSTANTFORCE diConstantForce;
  diConstantForce.lMagnitude = 0;

  DIEFFECT diEffect;
  ZeroMemory(&diEffect, sizeof(diEffect));
  diEffect.dwSize = sizeof(DIEFFECT);
  diEffect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTIDS;
  diEffect.dwDuration = INFINITE;
  diEffect.dwSamplePeriod = 0;
  diEffect.dwGain = 10000;
  diEffect.dwTriggerButton = DIEB_NOTRIGGER;
  diEffect.dwTriggerRepeatInterval = 0;
  diEffect.cAxes = 1;
  diEffect.rgdwAxes = rgdwAxes;
  diEffect.rglDirection = rglDirection;
  diEffect.lpEnvelope = NULL;
  diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
  diEffect.lpvTypeSpecificParams = &diConstantForce;

  HRESULT hr = g_pDevice->CreateEffect(GUID_ConstantForce, &diEffect, &g_pEffect, NULL);
  if (SUCCEEDED(hr)) {
    g_pEffect->Start(1, 0);
  }

  g_pDevice->SendForceFeedbackCommand(DISFFC_SETACTUATORSON);
  pDI->Release();
#endif
}

void ffb_update(float force) {
#if defined(_WIN32)
  if (!g_pDevice || !g_pEffect)
    return;

  LONG magnitude = (LONG) (force * 10000.0f);
  if (magnitude > 10000)
    magnitude = 10000;
  if (magnitude < -10000)
    magnitude = -10000;

  DICONSTANTFORCE diConstantForce;
  diConstantForce.lMagnitude = magnitude;

  DIEFFECT diEffect;
  ZeroMemory(&diEffect, sizeof(diEffect));
  diEffect.dwSize = sizeof(DIEFFECT);
  diEffect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
  diEffect.lpvTypeSpecificParams = &diConstantForce;

  HRESULT hr = g_pEffect->SetParameters(&diEffect, DIEP_TYPESPECIFICPARAMS);

  if (FAILED(hr) && hr == DIERR_INPUTLOST) {
    g_pDevice->Acquire();
  }
#endif
}

void ffb_close() {
#if defined(_WIN32)
  if (g_pEffect) {
    g_pEffect->Unload();
    g_pEffect->Release();
    g_pEffect = nullptr;
  }
  if (g_pDevice) {
    g_pDevice->SendForceFeedbackCommand(DISFFC_RESET);
    g_pDevice->Unacquire();
    g_pDevice->Release();
    g_pDevice = nullptr;
  }
#endif
}
