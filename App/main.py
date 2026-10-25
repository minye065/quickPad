import os
import subprocess
import threading
import time
import hid
VENDOR_ID = 0xFEED
PRODUCT_ID = 0x0000
USAGE_PAGE = 0xFF60
USAGE = 0x61
REPORT_SIZE = 32
EVENT_KEY = 0x01
EVENT_KNOB = 0x02
EVENT_STATE = 0x03
EVENT_SYNC = 0x04
KNOB_CLOCKWISE = 0x01
KEY_TL = 0
KEY_TM = 1
KEY_TR = 2
KEY_ML = 3
KEY_MM = 4
KEY_MR = 5
KEY_BL = 6
KEY_BM = 7
KEY_BR = 8
MODES = ["base", "file", "clock"]
WORK_MINUTES = 25
REST_MINUTES = 8
ZEN_WINDOW_COUNT = 3

state = {"mode": 0, "timer": None}

def runCommand(command, wait=False):
    startupInfo = subprocess.STARTUPINFO()
    startupInfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
    startupInfo.wShowWindow = subprocess.SW_HIDE
    flags = subprocess.CREATE_NO_WINDOW | subprocess.CREATE_NEW_PROCESS_GROUP
    process = subprocess.Popen(command, shell=True, stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, startupinfo=startupInfo, creationflags=flags)
    if wait:
        return process.wait()
    return process


def notify(message):
    script = "Add-Type -AssemblyName System.Windows.Forms; $n = New-Object System.Windows.Forms.NotifyIcon; $n.Icon = [System.Drawing.SystemIcons]::Information; $n.Visible = $true; $n.ShowBalloonTip(2000, 'quickpad', '" + message + "', [System.Windows.Forms.ToolTipIcon]::Info); Start-Sleep 3; $n.Dispose()"
    runCommand('powershell -NoProfile -NonInteractive -WindowStyle Hidden -Command "' + script + '"')

def clearClipboard():
    runCommand('powershell -NoProfile -NonInteractive -Command "Set-Clipboard -Value $null"')

def emptyRecycleBin():
    runCommand('powershell -NoProfile -NonInteractive -Command "Clear-RecycleBin -Force"')

def openZenWindows():
    zenPath = os.path.join(os.environ["LOCALAPPDATA"], "Zen Browser", "zen.exe")
    for i in range(ZEN_WINDOW_COUNT):
        subprocess.Popen([zenPath, "--new-window"], stdin=subprocess.DEVNULL, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, creationflags=subprocess.DETACHED_PROCESS)
        time.sleep(1.5)


def startTimer(name, minutes):
    if state["timer"]:
        state["timer"].cancel()
    timer = threading.Timer(minutes * 60, notify, args=[name + " timer done"])
    timer.daemon = True
    timer.start()
    state["timer"] = timer
    notify(name + " timer started: " + str(minutes) + "m")

def endTimers():
    if state["timer"]:
        state["timer"].cancel()
        state["timer"] = None
        notify("Timer stopped")

def startWorkTimer():
    startTimer("Work", WORK_MINUTES)


def startRestTimer():
    startTimer("Rest", REST_MINUTES)


def temporary():
    notify("TEMPORARY")


ACTIONS = {0: {KEY_TM: temporary, KEY_TR: temporary, KEY_ML: temporary, KEY_MM: temporary, KEY_MR: temporary, KEY_BL: temporary, KEY_BM: temporary, KEY_BR: temporary}, 1: {KEY_TM: emptyRecycleBin, KEY_TR: openZenWindows, KEY_ML: temporary, KEY_MM: temporary, KEY_MR: temporary, KEY_BL: temporary, KEY_BM: temporary, KEY_BR: temporary}, 2: {KEY_TM: startWorkTimer, KEY_TR: startRestTimer, KEY_ML: endTimers, KEY_MM: temporary, KEY_MR: temporary, KEY_BL: temporary, KEY_BM: temporary, KEY_BR: temporary}}

def setMode(mode):
    state["mode"] = mode % len(MODES)
    notify("Mode changed to " + MODES[state["mode"]])

def handleEvent(eventType, value):
    if eventType == EVENT_STATE:
        setMode(value)
    elif eventType == EVENT_SYNC:
        state["mode"] = value % len(MODES)
    elif eventType == EVENT_KEY:
        action = ACTIONS[state["mode"]].get(value)
        if action:
            threading.Thread(target=action, daemon=True).start()

def openDevice():
    for info in hid.enumerate(VENDOR_ID, PRODUCT_ID):
        if info["usage_page"] == USAGE_PAGE and info["usage"] == USAGE:
            device = hid.device()
            device.open_path(info["path"])
            return device
    return None

def requestSync(device):
    device.write(bytes([0, EVENT_SYNC]) + bytes(REPORT_SIZE - 1))

def listen():
    while True:
        device = openDevice()
        if device is None:
            time.sleep(2)
            continue
        try:
            requestSync(device)
            while True:
                data = device.read(REPORT_SIZE, 500)
                if data:
                    handleEvent(data[0], data[1])
        except (OSError, ValueError):
            pass
        finally:
            device.close()

def main():
    try:
        listen()
    except KeyboardInterrupt:
        pass

if __name__ == "__main__":
    main()
